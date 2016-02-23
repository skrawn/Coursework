/*******************************************************************************
* File Name: cyfitter_cfg.h
* 
* PSoC Creator  3.3 CP1
*
* Description:
* This file provides basic startup and mux configration settings
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2015 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#ifndef CYFITTER_CFG_H
#define CYFITTER_CFG_H

#include "cytypes.h"

extern void cyfitter_cfg(void);

/* Analog Set/Unset methods */
extern void SetAnalogRoutingPumps(uint8 enabled);
extern void LED_Mux_Set(uint8 channel);
extern void LED_Mux_Unset(uint8 channel);
/* LED_Mux__addrTable is an implementation detail of the AMux.
   Code that depends on it may be incompatible with other versions
   of PSoC Creator. */
extern uint8 CYXDATA * const CYCODE LED_Mux__addrTable[4];
/* LED_Mux__maskTable is an implementation detail of the AMux.
   Code that depends on it may be incompatible with other versions
   of PSoC Creator. */
extern const uint8 CYCODE LED_Mux__maskTable[4];


#endif /* CYFITTER_CFG_H */

/*[]*/
