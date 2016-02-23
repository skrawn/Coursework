/*******************************************************************************
* File Name: LED_Mux.h
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

#if !defined(CY_AMUX_LED_Mux_H)
#define CY_AMUX_LED_Mux_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void LED_Mux_Start(void) ;
#define LED_Mux_Init() LED_Mux_Start()
void LED_Mux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void LED_Mux_Stop(void); */
/* void LED_Mux_Select(uint8 channel); */
/* void LED_Mux_Connect(uint8 channel); */
/* void LED_Mux_Disconnect(uint8 channel); */
/* void LED_Mux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define LED_Mux_CHANNELS  2u
#define LED_Mux_MUXTYPE   1
#define LED_Mux_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define LED_Mux_NULL_CHANNEL 0xFFu
#define LED_Mux_MUX_SINGLE   1
#define LED_Mux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if LED_Mux_MUXTYPE == LED_Mux_MUX_SINGLE
# if !LED_Mux_ATMOSTONE
#  define LED_Mux_Connect(channel) LED_Mux_Set(channel)
# endif
# define LED_Mux_Disconnect(channel) LED_Mux_Unset(channel)
#else
# if !LED_Mux_ATMOSTONE
void LED_Mux_Connect(uint8 channel) ;
# endif
void LED_Mux_Disconnect(uint8 channel) ;
#endif

#if LED_Mux_ATMOSTONE
# define LED_Mux_Stop() LED_Mux_DisconnectAll()
# define LED_Mux_Select(channel) LED_Mux_FastSelect(channel)
void LED_Mux_DisconnectAll(void) ;
#else
# define LED_Mux_Stop() LED_Mux_Start()
void LED_Mux_Select(uint8 channel) ;
# define LED_Mux_DisconnectAll() LED_Mux_Start()
#endif

#endif /* CY_AMUX_LED_Mux_H */


/* [] END OF FILE */
