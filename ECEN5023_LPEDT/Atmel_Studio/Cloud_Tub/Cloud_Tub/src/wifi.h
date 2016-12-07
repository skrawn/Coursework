/*
 * wifi.h
 *
 * Created: 12/4/2016 9:41:06 PM
 *  Author: Sean
 */ 

#ifndef WIFI_H_
#define WIFI_H_

/**
 * @brief Initializes the WiFi stack
 */
void wifi_init(void);

/**
 * @brief Wifi 3 second task
 */
void wifi_task_3s(void);

/**
 * @brief Wifi 1 second task
 */
void wifi_task_1s(void);

/**
 * @brief Wifi idle task
 */
void wifi_task_idle(void);


#endif /* WIFI_H_ */