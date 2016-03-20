/*******************************************************************************
* File Name: Sensor_Mux.h
* Version 1.80
*
*  Description:
*    This file contains the constants and function prototypes for the Analog
*    Multiplexer User Module AMux.
*
*   Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_AMUX_Sensor_Mux_H)
#define CY_AMUX_Sensor_Mux_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void Sensor_Mux_Start(void) ;
#define Sensor_Mux_Init() Sensor_Mux_Start()
void Sensor_Mux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void Sensor_Mux_Stop(void); */
/* void Sensor_Mux_Select(uint8 channel); */
/* void Sensor_Mux_Connect(uint8 channel); */
/* void Sensor_Mux_Disconnect(uint8 channel); */
/* void Sensor_Mux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define Sensor_Mux_CHANNELS  3u
#define Sensor_Mux_MUXTYPE   1
#define Sensor_Mux_ATMOSTONE 1

/***************************************
*             API Constants
***************************************/

#define Sensor_Mux_NULL_CHANNEL 0xFFu
#define Sensor_Mux_MUX_SINGLE   1
#define Sensor_Mux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if Sensor_Mux_MUXTYPE == Sensor_Mux_MUX_SINGLE
# if !Sensor_Mux_ATMOSTONE
#  define Sensor_Mux_Connect(channel) Sensor_Mux_Set(channel)
# endif
# define Sensor_Mux_Disconnect(channel) Sensor_Mux_Unset(channel)
#else
# if !Sensor_Mux_ATMOSTONE
void Sensor_Mux_Connect(uint8 channel) ;
# endif
void Sensor_Mux_Disconnect(uint8 channel) ;
#endif

#if Sensor_Mux_ATMOSTONE
# define Sensor_Mux_Stop() Sensor_Mux_DisconnectAll()
# define Sensor_Mux_Select(channel) Sensor_Mux_FastSelect(channel)
void Sensor_Mux_DisconnectAll(void) ;
#else
# define Sensor_Mux_Stop() Sensor_Mux_Start()
void Sensor_Mux_Select(uint8 channel) ;
# define Sensor_Mux_DisconnectAll() Sensor_Mux_Start()
#endif

#endif /* CY_AMUX_Sensor_Mux_H */


/* [] END OF FILE */
