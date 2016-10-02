

// This code is only valid for the FRDM board
#ifdef FRDM

#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_smc.h"
#include "fsl_uart.h"
#include "uart.h"

#define DEFAULT_BAUDRATE	115200
#define DEFAULT_UART_CLKSRC	kCLOCK_BusClk
#define KL25Z_XTAL0_CLK_HZ	8000000U

/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData);

uart_handle_t g_uartHandle;

uint8_t g_tipString[] =
    "Uart interrupt example\r\nBoard receives 8 characters then sends them out\r\nNow please input:\r\n";

/* System clock frequency. */
extern uint32_t SystemCoreClock;

volatile uint8_t rxBufferEmpty = 1;
volatile uint8_t txBufferFull = 0;
volatile uint8_t txOnGoing = 0;
volatile uint8_t rxOnGoing = 0;

void uart_init(void)
{
	uart_config_t config;
	uart_transfer_t xfer;

	/* Initialize UART1 pins below */
	/* Ungate the port clock */
	CLOCK_EnableClock(kCLOCK_PortE);
	/* Affects PORTE_PCR0 register */
	PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);
	/* Affects PORTE_PCR1 register */
	PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);

    /*
    * Core clock: 48MHz
    * Bus clock: 24MHz
    */
    const mcg_pll_config_t pll0Config = {
        .enableMode = 0U, .prdiv = 0x1U, .vdiv = 0x0U,
    };
    const sim_clock_config_t simConfig = {
        .pllFllSel = 1U,        /* PLLFLLSEL select PLL. */
        .er32kSrc = 3U,         /* ERCLK32K selection, use LPO. */
        .clkdiv1 = 0x10010000U, /* SIM_CLKDIV1. */
    };

    CLOCK_SetSimSafeDivs();
    const osc_config_t oscConfig = {.freq = KL25Z_XTAL0_CLK_HZ,
                                    .capLoad = 0,
                                    .workMode = kOSC_ModeOscLowPower,
                                    .oscerConfig = {
                                        .enableMode = kOSC_ErClkEnable,
#if (defined(FSL_FEATURE_OSC_HAS_EXT_REF_CLOCK_DIVIDER) && FSL_FEATURE_OSC_HAS_EXT_REF_CLOCK_DIVIDER)
                                        .erclkDiv = 0U,
#endif
                                    }};

    CLOCK_InitOsc0(&oscConfig);

    /* Passing the XTAL0 frequency to clock driver. */
    CLOCK_SetXtal0Freq(KL25Z_XTAL0_CLK_HZ);

    CLOCK_BootToPeeMode(kMCG_OscselOsc, kMCG_PllClkSelPll0, &pll0Config);

    CLOCK_SetInternalRefClkConfig(kMCG_IrclkEnable, kMCG_IrcSlow, 0);
    CLOCK_SetSimConfig(&simConfig);

    SystemCoreClock = 48000000U;

    /*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = DEFAULT_BAUDRATE;
	config.enableTx = 1;
	config.enableRx = 1;

	UART_Init(UART1, &config, CLOCK_GetFreq(DEFAULT_UART_CLKSRC));
	UART_TransferCreateHandle(UART1, &g_uartHandle, UART_UserCallback, NULL);

	/* Send g_tipString out. */
	xfer.data = g_tipString;
	xfer.dataSize = sizeof(g_tipString) - 1;
	txOnGoing = true;
	UART_TransferSendNonBlocking(UART1, &g_uartHandle, &xfer);

}

/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing = false;
    }

    if (kStatus_UART_RxIdle == status)
    {
        rxBufferEmpty = false;
        rxOnGoing = false;
    }
}

#endif

