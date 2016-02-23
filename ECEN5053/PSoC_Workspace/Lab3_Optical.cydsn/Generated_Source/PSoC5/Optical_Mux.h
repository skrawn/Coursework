/*******************************************************************************
* File Name: Optical_Mux.h
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

#if !defined(CY_AMUX_Optical_Mux_H)
#define CY_AMUX_Optical_Mux_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cyfitter_cfg.h"


/***************************************
*        Function Prototypes
***************************************/

void Optical_Mux_Start(void) ;
#define Optical_Mux_Init() Optical_Mux_Start()
void Optical_Mux_FastSelect(uint8 channel) ;
/* The Stop, Select, Connect, Disconnect and DisconnectAll functions are declared elsewhere */
/* void Optical_Mux_Stop(void); */
/* void Optical_Mux_Select(uint8 channel); */
/* void Optical_Mux_Connect(uint8 channel); */
/* void Optical_Mux_Disconnect(uint8 channel); */
/* void Optical_Mux_DisconnectAll(void) */


/***************************************
*         Parameter Constants
***************************************/

#define Optical_Mux_CHANNELS  4u
#define Optical_Mux_MUXTYPE   1
#define Optical_Mux_ATMOSTONE 0

/***************************************
*             API Constants
***************************************/

#define Optical_Mux_NULL_CHANNEL 0xFFu
#define Optical_Mux_MUX_SINGLE   1
#define Optical_Mux_MUX_DIFF     2


/***************************************
*        Conditional Functions
***************************************/

#if Optical_Mux_MUXTYPE == Optical_Mux_MUX_SINGLE
# if !Optical_Mux_ATMOSTONE
#  define Optical_Mux_Connect(channel) Optical_Mux_Set(channel)
# endif
# define Optical_Mux_Disconnect(channel) Optical_Mux_Unset(channel)
#else
# if !Optical_Mux_ATMOSTONE
void Optical_Mux_Connect(uint8 channel) ;
# endif
void Optical_Mux_Disconnect(uint8 channel) ;
#endif

#if Optical_Mux_ATMOSTONE
# define Optical_Mux_Stop() Optical_Mux_DisconnectAll()
# define Optical_Mux_Select(channel) Optical_Mux_FastSelect(channel)
void Optical_Mux_DisconnectAll(void) ;
#else
# define Optical_Mux_Stop() Optical_Mux_Start()
void Optical_Mux_Select(uint8 channel) ;
# define Optical_Mux_DisconnectAll() Optical_Mux_Start()
#endif

#endif /* CY_AMUX_Optical_Mux_H */


/* [] END OF FILE */
