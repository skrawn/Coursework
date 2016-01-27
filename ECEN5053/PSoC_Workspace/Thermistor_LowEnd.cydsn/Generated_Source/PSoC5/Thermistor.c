/*******************************************************************************
* File Name: Thermistor.c
* Version 1.20
*
* Description:
*  This file provides the source code to the API for the ThermistorCalc
*  Component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Thermistor.h"


/*******************************************************************************
* Function Name: Thermistor_GetResistance
********************************************************************************
*
* Summary:
*  The digital values of the voltages across the reference resistor and the
*  thermistor are passed to this function as parameters. The function returns
*  the resistance, based on the voltage values.
*
* Parameters:
*  vReference: the voltage across the reference resistor;
*  vThermistor: the voltage across the thermistor.
*  The ratio of these two voltages is used by this function. Therefore, the
*  units for both parameters must be the same.
*
* Return:
*  The return value is the resistance across the thermistor. The value returned
*  is the resistance in Ohms.
*
*******************************************************************************/
uint32 Thermistor_GetResistance(int16 vReference, int16 vThermistor)
                                      
{
    int32 resT;
    int16 temp;

    /* Calculate thermistor resistance from the voltages */
    resT = Thermistor_REF_RESISTOR * ((int32)vThermistor);
    if (vReference < 0)
    {
        temp = -vReference;
        temp = (int16)((uint16)((uint16)temp >> 1u));
        temp = -temp;
    }
    else
    {
        temp = (int16)((uint16)((uint16)vReference >> 1u));
    }
    resT += temp;
    resT /= vReference;

    /* The ordering of Reference resistor value is specifically designed to keep the result from overflowing */
    return ((uint32)((uint32)resT << Thermistor_REF_RES_SHIFT));
}


/*******************************************************************************
* Function Name: Thermistor_GetTemperature
********************************************************************************
*
* Summary:
*  The value of the thermistor resistance is passed to this function as a
*  parameter. The function returns the temperature, based on the resistance
*  value. The method used to calculate the temperature is dependent on whether
*  Equation or LUT was selected in the Customizer.
*
* Parameters:
*  resT: the resistance across the thermistor in Ohms.
*
* Return:
*  The return value is the temperature in 1/100ths of degrees C. For example,
*  the return value is 2345, when the actual temperature is 23.45 degrees C.
*
*******************************************************************************/
int16 Thermistor_GetTemperature(uint32 resT) 
{
    int16 tempTR;
    static const uint32 CYCODE Thermistor_LUT[] = { 195652u, 190302u, 185115u, 180086u, 175208u, 170478u, 165891u, 161441u,
 157124u, 152936u, 148873u, 144930u, 141103u, 137389u, 133785u, 130286u, 126889u, 123592u, 120390u, 117281u, 114261u,
 111329u, 108480u, 105714u, 103026u, 100414u, 97877u, 95412u, 93016u, 90687u, 88424u, 86224u, 84085u, 82006u, 79985u,
 78019u, 76108u, 74249u, 72441u, 70682u, 68972u, 67308u, 65689u, 64113u, 62581u, 61089u, 59637u, 58224u, 56849u, 55510u,
 54207u, 52938u, 51703u, 50500u, 49328u, 48187u, 47076u, 45994u, 44940u, 43913u, 42912u, 41937u, 40987u, 40062u, 39160u,
 38281u, 37424u, 36589u, 35775u, 34981u, 34207u, 33453u, 32717u, 32000u, 31300u, 30618u, 29952u, 29303u, 28670u, 28052u,
 27450u, 26862u, 26288u, 25728u, 25182u, 24649u, 24128u, 23620u, 23124u, 22640u, 22168u, 21707u, 21256u, 20816u, 20387u,
 19967u, 19558u, 19158u, 18767u, 18385u, 18012u, 17648u, 17292u, 16944u, 16605u, 16272u, 15948u, 15631u, 15321u, 15018u,
 14722u, 14432u, 14149u, 13873u, 13602u, 13338u, 13079u, 12826u, 12579u, 12337u, 12101u, 11869u, 11643u, 11421u, 11205u,
 10993u, 10786u, 10583u, 10384u, 10190u, 10000u, 9814u, 9632u, 9454u, 9279u, 9109u, 8941u, 8778u, 8618u, 8461u, 8307u,
 8157u, 8010u, 7865u, 7724u, 7586u, 7450u, 7318u, 7188u, 7060u, 6936u, 6814u, 6694u, 6576u, 6462u, 6349u, 6239u, 6130u,
 6024u, 5920u, 5819u, 5719u, 5621u, 5525u, 5431u, 5339u, 5248u, 5160u, 5073u, 4988u, 4904u, 4822u, 4742u, 4663u, 4586u,
 4510u, 4436u, 4363u, 4291u, 4221u, 4152u, 4084u, 4018u, 3953u, 3889u, 3826u, 3765u, 3705u, 3645u, 3587u, 3530u, 3474u,
 3419u, 3365u, 3312u, 3260u, 3209u, 3159u, 3110u, 3062u, 3014u, 2968u, 2922u, 2877u, 2833u, 2789u, 2747u, 2705u, 2664u,
 2623u, 2584u, 2545u, 2507u, 2469u, 2432u, 2396u, 2360u, 2325u, 2291u, 2257u, 2224u, 2191u, 2159u, 2127u, 2096u, 2066u,
 2036u, 2006u, 1977u, 1949u, 1921u, 1893u, 1866u, 1839u, 1813u, 1787u, 1762u, 1737u, 1713u, 1689u, 1665u, 1642u, 1619u,
 1596u, 1574u, 1552u, 1531u, 1510u, 1489u, 1468u, 1448u, 1428u, 1409u, 1390u, 1371u, 1352u, 1334u, 1316u, 1299u, 1281u,
 1264u, 1247u, 1231u, 1214u, 1198u, 1182u, 1167u, 1151u, 1136u, 1121u, 1107u, 1092u, 1078u, 1064u, 1051u, 1037u, 1024u,
 1010u, 998u, 985u, 972u, 960u, 948u, 936u, 924u, 912u, 901u, 889u, 878u, 867u, 857u, 846u, 835u, 825u, 815u, 805u,
 795u, 785u, 776u, 766u, 757u, 748u, 739u, 730u, 721u, 712u, 704u, 695u, 687u, 679u, 671u, 663u, 655u, 647u, 640u, 632u,
 625u, 617u, 610u, 603u, 596u, 589u, 582u, 576u, 569u, 562u, 556u, 550u, 543u, 537u, 531u };


    #if (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD)

        float32 stEqn;
        float32 logrT;

        /* Calculate thermistor resistance from the voltages */
        #if(!CY_PSOC3)
            logrT = (float32)log((float64)resT);
        #else
            logrT = log((float32)resT);
        #endif  /* (!CY_PSOC3) */

        /* Calculate temperature from the resistance of thermistor using Steinhart-Hart Equation */
        #if(!CY_PSOC3)
            stEqn = (float32)(Thermistor_THA + (Thermistor_THB * logrT) + 
                             (Thermistor_THC * pow((float64)logrT, (float32)3)));
        #else
            stEqn = (float32)(Thermistor_THA + (Thermistor_THB * logrT) + 
                             (Thermistor_THC * pow(logrT, (float32)3)));
        #endif  /* (!CY_PSOC3) */

        tempTR = (int16)((float32)((((1.0 / stEqn) - Thermistor_K2C) * (float32)Thermistor_SCALE) + 0.5));

    #else /* Thermistor_IMPLEMENTATION == Thermistor_LUT_METHOD */

        uint16 mid;
        uint16 first = 0u;
        uint16 last = Thermistor_LUT_SIZE - 1u;

        /* Calculate temperature from the resistance of thermistor by using the LUT */
        while (first < last)
        {
            mid = (first + last) >> 1u;

            if ((0u == mid) || ((Thermistor_LUT_SIZE - 1u) == mid) || (resT == Thermistor_LUT[mid]))
            {
                last = mid;
                break;
            }
            else if (Thermistor_LUT[mid] > resT)
            {
                first = mid + 1u;
            }
            else /* (Thermistor_LUT[mid] < resT) */
            {
                last = mid - 1u;
            }
        }

        /* Calculation the closest entry in the LUT */
        if ((Thermistor_LUT[last] > resT) && (last < (Thermistor_LUT_SIZE - 1u)) &&
           ((Thermistor_LUT[last] - resT) > (resT - Thermistor_LUT[last + 1u])))
        {
            last++;
        }
        else if ((Thermistor_LUT[last] < resT) && (last > 0u) &&
                ((Thermistor_LUT[last - 1u] - resT) < (resT - Thermistor_LUT[last])))
        {
            last--;
        }
        else
        {
            /* Closest entry in the LUT already found */
        }

        tempTR = Thermistor_MIN_TEMP + (int16)((uint16)(last * Thermistor_ACCURACY));

    #endif /* (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD) */

    return (tempTR);
}


/* [] END OF FILE */
