/*******************************************************************************
* File Name: Thermistor_Calc_1.h
* Version 1.1
*
* Description:
*  This header file contains definitions associated with the Thermistor component.
*
* Note: none
*
********************************************************************************
* Copyright (2011), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/
#if !defined(Thermistor_Calc_1_H) 
#define Thermistor_Calc_1_H

#include "cytypes.h"
#include "cylib.h"
#include "cyfitter.h"

/**************************************
* Parameter Defaults    
* Parameters and Co-efficients (or LUT) obtained from the customizer
**************************************/
#define Thermistor_Calc_1_K2C 					273.15
#define Thermistor_Calc_1_SCALE 					100
#define Thermistor_Calc_1_REF_RES               10000
#define Thermistor_Calc_1_IMPL 				   0
#define Thermistor_Calc_1_ACCURACY              0.1
#define Thermistor_Calc_1_MIN_TEMP              0
#define	Thermistor_Calc_1_LUTSIZE	500
#define	Thermistor_Calc_1_LUT	{27395, 27277, 27161, 27045, 26929, 26814, 26700, 26586, 26472, 26359, 26247, 26136, 26024, 25914, 25804, 25694, 25585, 25476, 25368, 25261, 25154, 25047, 24941, 24836, 24731, 24627, 24523, 24419, 24316, 24214, 24112, 24010, 23909, 23809, 23709, 23609, 23510, 23411, 23313, 23215, 23118, 23021, 22925, 22829, 22734, 22639, 22544, 22450, 22357, 22264, 22171, 22079, 21987, 21895, 21804, 21714, 21624, 21534, 21444, 21356, 21267, 21179, 21091, 21004, 20917, 20831, 20745, 20659, 20574, 20489, 20405, 20321, 20237, 20154, 20071, 19989, 19907, 19825, 19744, 19663, 19582, 19502, 19422, 19343, 19264, 19185, 19107, 19029, 18951, 18874, 18797, 18720, 18644, 18568, 18493, 18418, 18343, 18269, 18194, 18121, 18047, 17974, 17901, 17829, 17757, 17685, 17614, 17543, 17472, 17401, 17331, 17261, 17192, 17123, 17054, 16985, 16917, 16849, 16782, 16714, 16647, 16581, 16514, 16448, 16382, 16317, 16252, 16187, 16122, 16058, 15994, 15930, 15867, 15804, 15741, 15678, 15616, 15554, 15492, 15431, 15369, 15309, 15248, 15188, 15127, 15068, 15008, 14949, 14890, 14831, 14773, 14714, 14656, 14599, 14541, 14484, 14427, 14370, 14314, 14258, 14202, 14146, 14091, 14036, 13981, 13926, 13872, 13817, 13763, 13710, 13656, 13603, 13550, 13497, 13445, 13392, 13340, 13288, 13237, 13185, 13134, 13083, 13033, 12982, 12932, 12882, 12832, 12782, 12733, 12684, 12635, 12586, 12538, 12489, 12441, 12393, 12346, 12298, 12251, 12204, 12157, 12110, 12064, 12018, 11972, 11926, 11880, 11835, 11790, 11744, 11700, 11655, 11611, 11566, 11522, 11478, 11435, 11391, 11348, 11305, 11262, 11219, 11176, 11134, 11092, 11050, 11008, 10966, 10925, 10884, 10843, 10802, 10761, 10720, 10680, 10640, 10600, 10560, 10520, 10480, 10441, 10402, 10363, 10324, 10285, 10247, 10208, 10170, 10132, 10094, 10056, 10019, 9981, 9944, 9907, 9870, 9833, 9797, 9760, 9724, 9688, 9652, 9616, 9580, 9545, 9509, 9474, 9439, 9404, 9369, 9334, 9300, 9266, 9231, 9197, 9163, 9130, 9096, 9062, 9029, 8996, 8963, 8930, 8897, 8864, 8832, 8800, 8767, 8735, 8703, 8671, 8640, 8608, 8577, 8545, 8514, 8483, 8452, 8421, 8391, 8360, 8330, 8299, 8269, 8239, 8209, 8179, 8150, 8120, 8091, 8061, 8032, 8003, 7974, 7945, 7917, 7888, 7860, 7831, 7803, 7775, 7747, 7719, 7691, 7664, 7636, 7609, 7581, 7554, 7527, 7500, 7473, 7447, 7420, 7393, 7367, 7341, 7314, 7288, 7262, 7236, 7211, 7185, 7159, 7134, 7108, 7083, 7058, 7033, 7008, 6983, 6958, 6934, 6909, 6885, 6860, 6836, 6812, 6788, 6764, 6740, 6716, 6693, 6669, 6646, 6622, 6599, 6576, 6553, 6530, 6507, 6484, 6461, 6438, 6416, 6393, 6371, 6349, 6327, 6304, 6282, 6261, 6239, 6217, 6195, 6174, 6152, 6131, 6109, 6088, 6067, 6046, 6025, 6004, 5983, 5963, 5942, 5921, 5901, 5880, 5860, 5840, 5820, 5800, 5780, 5760, 5740, 5720, 5700, 5681, 5661, 5642, 5622, 5603, 5584, 5565, 5546, 5527, 5508, 5489, 5470, 5451, 5433, 5414, 5396, 5377, 5359, 5341, 5322, 5304, 5286, 5268, 5250, 5233, 5215, 5197, 5179, 5162, 5144, 5127, 5110, 5092, 5075, 5058, 5041, 5024, 5007, 4990, 4973, 4956, 4940, 4923, 4906, 4890, 4873, 4857, 4841, 4824, 4808, 4792, 4776, 4760, 4744, 4728, 4712, 4697, 4681, 4665, 4650, 4634, 4619, 4603, 4588, 4573, 4558, 4542, 4527, 4512, 4497, 4482, 4467, 4453, 4438, 4423, 4409, 4394, 4379, 4365, 4350, 4336, 4322, 4308, 4293, 4279, 4265, 4251, 4237, 4223, 4209, 4195} 

/***************************************
*        Function Prototypes 
***************************************/

/*Function to convert voltage to resistance*/
int32 Thermistor_Calc_1_GetResistance(int32 Vref, int32 VT);

/*Function to convert resistance to temperature*/
int32 Thermistor_Calc_1_GetTemperature(int32 ResT);
#endif /* CY_Thermistor_Calc_1_H */										



//[] END OF FILE
