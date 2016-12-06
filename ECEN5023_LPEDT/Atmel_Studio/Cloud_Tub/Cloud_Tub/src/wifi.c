/*
 * wifi.c
 *
 * Created: 12/4/2016 9:40:14 PM
 *  Author: Sean
 */ 

#include <stdlib.h>

#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "PubNub.h"

#include "display.h"
#include "jsmn.h"
#include "main.h"
#include "thermal.h"
#include "wifi.h"

#define IPV4_BYTE(val, index) ((val >> (index * 8)) & 0xFF)
#define HEX2ASCII(x) (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))

typedef enum wifi_status {
    WifiStateInit,
    WifiStateWaitingProv,
    WifiStateConnecting,
    WifiStateConnected,
    WifiStateDisConnected
} wifi_status_t;

/** WiFi status variable. */
volatile wifi_status_t gWifiState = WifiStateInit;

/** SysTick counter to avoid busy wait delay. */
volatile uint32_t gu32MsTicks = 0;

/** PubNub global variables. */
static const char PubNubPublishKey[] = MAIN_PUBNUB_PUBLISH_KEY;
static const char PubNubSubscribeKey[] = MAIN_PUBNUB_SUBSCRIBE_KEY;
static char PubNubChannel[] = MAIN_PUBNUB_CHANNEL;
static pubnub_t *pPubNubCfg;

jsmn_parser parser;
uint8_t num_tokens = 10;    // Start with 10 tokens, program will allocate more if needed

typedef struct {
    char *json_string;
    uint8_t string_size;
    void (*handler)(char *msg, jsmntok_t *key, jsmntok_t *value);
} wifi_handler_t;

static void temp_setpoint_handler(char *msg, jsmntok_t *key, jsmntok_t *value) 
{
    char set_point[4] = {0};

    memcpy(set_point, &msg[value->start], value->end - value->start);
    thermal_set_temperature((uint8_t) atoi(set_point));
}

// Handler for input data
const wifi_handler_t wifi_handlers[] = {
    { "temp_setpoint",  sizeof("temp_setpoint"), temp_setpoint_handler },
    { 0,  0, NULL }
};

/**
 * \brief Callback of gethostbyname function.
 *
 * \param[in] doamin_name Domain name.
 * \param[in] server_ip IP of server.
 */
static void socket_resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	printf("socket_resolve_cb: %s resolved with IP %d.%d.%d.%d\r\n",
			hostName,
			(int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
	handle_dns_found((char *)hostName, hostIp);
}

/**
 * \brief Callback to get the Socket event.
 *
 * \param[in] Socket descriptor.
 * \param[in] msg_type type of Socket notification. Possible types are:
 *  - [SOCKET_MSG_CONNECT](@ref SOCKET_MSG_CONNECT)
 *  - [SOCKET_MSG_BIND](@ref SOCKET_MSG_BIND)
 *  - [SOCKET_MSG_LISTEN](@ref SOCKET_MSG_LISTEN)
 *  - [SOCKET_MSG_ACCEPT](@ref SOCKET_MSG_ACCEPT)
 *  - [SOCKET_MSG_RECV](@ref SOCKET_MSG_RECV)
 *  - [SOCKET_MSG_SEND](@ref SOCKET_MSG_SEND)
 *  - [SOCKET_MSG_SENDTO](@ref SOCKET_MSG_SENDTO)
 *  - [SOCKET_MSG_RECVFROM](@ref SOCKET_MSG_RECVFROM)
 * \param[in] msg_data A structure contains notification informations.
 */
static void m2m_tcp_socket_handler(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	handle_tcpip(sock, u8Msg, pvMsg);
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CURRENT_RSSI](@ref M2M_WIFI_RESP_CURRENT_RSSI)
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_RESP_CONNTION_STATE](@ref M2M_WIFI_RESP_CONNTION_STATE)
 *  - [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
 *  - [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
 *  - [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS)
 *  - [M2M_WIFI_RESP_IP_CONFIGURED](@ref M2M_WIFI_RESP_IP_CONFIGURED)
 *  - [M2M_WIFI_RESP_IP_CONFLICT](@ref M2M_WIFI_RESP_IP_CONFLICT)
 *  - [M2M_WIFI_RESP_P2P](@ref M2M_WIFI_RESP_P2P)
 *  - [M2M_WIFI_RESP_AP](@ref M2M_WIFI_RESP_AP)
 *  - [M2M_WIFI_RESP_CLIENT_INFO](@ref M2M_WIFI_RESP_CLIENT_INFO)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type. Existing types are:
 *  - tstrM2mWifiStateChanged
 *  - tstrM2MWPSInfo
 *  - tstrM2MP2pResp
 *  - tstrM2MAPResp
 *  - tstrM2mScanDone
 *  - tstrM2mWifiscanResult
 */
static void m2m_wifi_state(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("m2m_wifi_state: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("m2m_wifi_state: M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
			if (WifiStateConnected == gWifiState) {
				gWifiState = WifiStateDisConnected;
				m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
						MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
			}
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;		
		printf("m2m_wifi_state: M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		gWifiState = WifiStateConnected;

		break;
	}

	default:
	{
		break;
	}
	}
}

static void set_dev_name_to_mac(uint8 *name, uint8 *mac_addr)
{
    /* Name must be in the format WINC1500_00:00 */
    uint16 len;

    len = m2m_strlen(name);
    if (len >= 5) {
        name[len - 1] = HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
        name[len - 2] = HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
        name[len - 4] = HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
        name[len - 5] = HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
    }
}

void wifi_init(void)
{
    tstrWifiInitParam wifiInitParam;
    int8_t s8InitStatus;
    uint8 mac_addr[6];
    uint8 u8IsMacAddrValid;    

    /* Initialize the Wi-Fi BSP. */
    nm_bsp_init();

    /* Initialize Wi-Fi parameters structure. */
    memset((uint8_t *)&wifiInitParam, 0, sizeof(tstrWifiInitParam));
    wifiInitParam.pfAppWifiCb = m2m_wifi_state;
    
    /* Initialize WINC1500 Wi-Fi driver with data and status callbacks. */
    s8InitStatus = m2m_wifi_init(&wifiInitParam);
    if (M2M_SUCCESS != s8InitStatus) {
        printf("main: m2m_wifi_init call error!\r\n");
        while (1) {
        }
    }

    /* Initialize Socket API. */
    socketInit();
    registerSocketCallback(m2m_tcp_socket_handler, socket_resolve_cb);

    /* Read MAC address to customize device name and AP name if enabled. */
    m2m_wifi_get_otp_mac_address(mac_addr, &u8IsMacAddrValid);
    if (!u8IsMacAddrValid) {
        printf("main: MAC address fuse bit has not been configured!\r\n");
        printf("main: Use m2m_wifi_set_mac_address() API to set MAC address via software.\r\n");
        while (1) {
        }
    }
    m2m_wifi_get_mac_address(mac_addr);
    set_dev_name_to_mac((uint8 *)PubNubChannel, mac_addr);
    printf("\r\n");

    /* Initialize PubNub API. */
    printf("main: PubNub configured with following settings:\r\n");
    printf("main:  - Publish key: \"%s\", Subscribe key: \"%s\", Channel: \"%s\".\r\n\r\n",
    PubNubPublishKey, PubNubSubscribeKey, PubNubChannel);
    pPubNubCfg = pubnub_get_ctx(0);
    pubnub_init(pPubNubCfg, PubNubPublishKey, PubNubSubscribeKey);

    /* Connect to AP using Wi-Fi settings from main.h. */
    printf("main: Wi-Fi connecting to AP using hardcoded credentials...\r\n");
    m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID),
    MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
}

void wifi_task_3s(void)
{    
    uint16_t light = 0;
    char buf[256] = {0};
    display_state_t *disp = display_get_display_state();

    if (gWifiState == WifiStateConnected) {        
        /*sprintf(buf, "{\"device\":\"%s\", \"water_temp\":\"%d\", \"heater_on\":\"%d\", \"water_pump_on\":\"%s\"}",
            PubNubChannel,
            thermal_get_water_temp(),
            (((4096 - light) * 100) / 4096),
            port_pin_get_output_level(LED0_PIN) ? "0" : "1");*/
        
        sprintf(buf, "{\"device\":\"%s\", \"water_temp\":\"%d\"}",
            PubNubChannel,
            thermal_get_water_temp());
            
        close(pPubNubCfg->tcp_socket);
        pPubNubCfg->state = PS_IDLE;
        pPubNubCfg->last_result = PNR_IO_ERROR;
        pubnub_publish(pPubNubCfg, PubNubChannel, buf);
    }

}

void wifi_task_1s(void)
{
    uint8_t handler_index = 0;
    //jsmntok_t tokens[10];
    jsmntok_t *tokens;
    jsmntok_t *key_index;
    int jsmn_retval;
    char *msg;

    /* Device is connected to AP. */
    if (gWifiState == WifiStateConnected) {
        /* PubNub: read event from the cloud. */
        if (pPubNubCfg->state == PS_IDLE) {
            /* Subscribe at the beginning and re-subscribe after every publish. */
            if ((pPubNubCfg->trans == PBTT_NONE) ||
            (pPubNubCfg->trans == PBTT_PUBLISH && pPubNubCfg->last_result == PNR_OK)) {
                printf("main: subscribe event, PNR_OK\r\n");
                pubnub_subscribe(pPubNubCfg, PubNubChannel);
            }

            /* Process any received messages from the channel we subscribed. */
            while (1) {    
check_msg:                    
                msg = (char *) pubnub_get(pPubNubCfg);
                if (NULL == msg) {
                    /* No more message to process. */
                    break;
                }

                if (0 == (strncmp(&msg[2], wifi_handlers[handler_index].json_string, wifi_handlers[handler_index].string_size))) {
                    
                }

realloc_tokens:
                tokens = malloc(sizeof(jsmntok_t) * num_tokens);
                if (tokens == NULL) {
                    printf("wifi_task_1s: could not allocate memory for tokens\n");
                    break;
                }

                // Initialize JSMN parser
                jsmn_init(&parser);

                jsmn_retval = jsmn_parse(&parser, msg, strlen(msg), tokens, num_tokens);
                if (jsmn_retval == JSMN_ERROR_NOMEM) {
                    // Didn't allocate enough tokens. Try again 
                    num_tokens++;
                    free(tokens);
                    goto realloc_tokens;
                }
                else if (jsmn_retval == JSMN_ERROR_INVAL) {
                    // Invalid data. Just return
                    free(tokens);
                }
                else if (jsmn_retval == JSMN_ERROR_PART) {
                    // JSON string is too short. Just return.
                    free(tokens);
                }
                else {    
                    key_index = tokens;
                    if (key_index->type == JSMN_OBJECT) 
                        // Object shows up as the first item in the token list. Skip it
                        key_index++;

                    // For some reason, our own messages show up. So, if the next token is "device", ignore the entire
                    // message.                    
                    if (0 == (strncmp(&msg[key_index->start], "device", strlen("device")))) {
                        free(tokens);
                        goto check_msg;
                    }
                    
                    // Search through the handlers for a valid handler for this key-value pair
                    while (key_index->type != JSMN_UNDEFINED) {
                        while (wifi_handlers[handler_index].json_string) {
                            if (0 == (strncmp(&msg[key_index->start], wifi_handlers[handler_index].json_string, key_index->end - key_index->start))) {
                                // Call the handler
                                wifi_handlers[handler_index].handler(msg, key_index, key_index + 1);                                
                                break;
                            }
                            handler_index++;
                        }
                        key_index += 2;
                        handler_index = 0;
                    }

                    //if (0 == (strncmp(&msg[2], "led", strlen("led")))) {
                        ///* LED control message. */
                        //printf("main: received LED control message: %s\r\n", msg);
                        //if (0 == (strncmp(&msg[8], "on", strlen("on")))) {
                            //port_pin_set_output_level(LED0_PIN, LED0_ACTIVE);
                            //} else if (0 == (strncmp(&msg[8], "off", strlen("off")))) {
                            //port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);
                        //}
                        //} else {
                        ///* Any other type of JSON message. */
                        //printf("main: received message: %s\r\n", msg);
                    //}
                    free(tokens);
                }
            }

            /* Subscribe to receive pending messages. */
            pubnub_subscribe(pPubNubCfg, PubNubChannel);
            
        }
    }
}

void wifi_task_idle(void)
{
    m2m_wifi_handle_events(NULL);   

}
