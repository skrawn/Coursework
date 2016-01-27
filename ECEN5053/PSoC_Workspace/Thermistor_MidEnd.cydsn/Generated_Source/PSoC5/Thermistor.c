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
    static const uint32 CYCODE Thermistor_LUT[] = { 195652u, 194569u, 193492u, 192422u, 191359u, 190302u, 189252u, 188208u,
 187171u, 186140u, 185115u, 184097u, 183085u, 182079u, 181079u, 180086u, 179098u, 178117u, 177141u, 176172u, 175208u,
 174251u, 173299u, 172353u, 171413u, 170478u, 169550u, 168627u, 167709u, 166797u, 165891u, 164990u, 164095u, 163205u,
 162320u, 161441u, 160567u, 159698u, 158835u, 157977u, 157124u, 156276u, 155434u, 154596u, 153764u, 152936u, 152113u,
 151296u, 150483u, 149675u, 148873u, 148074u, 147281u, 146493u, 145709u, 144930u, 144155u, 143385u, 142620u, 141859u,
 141103u, 140351u, 139604u, 138862u, 138123u, 137389u, 136660u, 135935u, 135214u, 134497u, 133785u, 133077u, 132373u,
 131673u, 130978u, 130286u, 129599u, 128915u, 128236u, 127561u, 126889u, 126222u, 125559u, 124899u, 124244u, 123592u,
 122944u, 122300u, 121659u, 121023u, 120390u, 119761u, 119135u, 118513u, 117895u, 117281u, 116670u, 116062u, 115458u,
 114858u, 114261u, 113668u, 113078u, 112491u, 111908u, 111329u, 110752u, 110179u, 109610u, 109043u, 108480u, 107921u,
 107364u, 106811u, 106261u, 105714u, 105170u, 104629u, 104092u, 103557u, 103026u, 102498u, 101972u, 101450u, 100931u,
 100414u, 99901u, 99391u, 98883u, 98379u, 97877u, 97378u, 96883u, 96389u, 95899u, 95412u, 94927u, 94445u, 93966u,
 93490u, 93016u, 92545u, 92076u, 91611u, 91148u, 90687u, 90229u, 89774u, 89322u, 88872u, 88424u, 87979u, 87537u, 87097u,
 86659u, 86224u, 85792u, 85361u, 84934u, 84508u, 84085u, 83665u, 83247u, 82831u, 82418u, 82006u, 81598u, 81191u, 80787u,
 80385u, 79985u, 79587u, 79192u, 78799u, 78408u, 78019u, 77633u, 77248u, 76866u, 76486u, 76108u, 75732u, 75358u, 74986u,
 74617u, 74249u, 73883u, 73520u, 73158u, 72799u, 72441u, 72085u, 71732u, 71380u, 71030u, 70682u, 70337u, 69993u, 69650u,
 69310u, 68972u, 68635u, 68301u, 67968u, 67637u, 67308u, 66980u, 66655u, 66331u, 66009u, 65689u, 65370u, 65053u, 64738u,
 64425u, 64113u, 63803u, 63495u, 63189u, 62884u, 62581u, 62279u, 61979u, 61681u, 61384u, 61089u, 60795u, 60503u, 60213u,
 59924u, 59637u, 59352u, 59067u, 58785u, 58504u, 58224u, 57946u, 57670u, 57395u, 57121u, 56849u, 56578u, 56309u, 56041u,
 55775u, 55510u, 55247u, 54985u, 54724u, 54465u, 54207u, 53950u, 53695u, 53442u, 53189u, 52938u, 52688u, 52440u, 52193u,
 51947u, 51703u, 51460u, 51218u, 50977u, 50738u, 50500u, 50263u, 50027u, 49793u, 49560u, 49328u, 49098u, 48868u, 48640u,
 48413u, 48187u, 47963u, 47739u, 47517u, 47296u, 47076u, 46858u, 46640u, 46423u, 46208u, 45994u, 45781u, 45569u, 45358u,
 45148u, 44940u, 44732u, 44526u, 44320u, 44116u, 43913u, 43711u, 43509u, 43309u, 43110u, 42912u, 42715u, 42519u, 42324u,
 42130u, 41937u, 41745u, 41554u, 41364u, 41175u, 40987u, 40800u, 40614u, 40429u, 40245u, 40062u, 39880u, 39698u, 39518u,
 39338u, 39160u, 38982u, 38805u, 38630u, 38455u, 38281u, 38108u, 37935u, 37764u, 37593u, 37424u, 37255u, 37087u, 36920u,
 36754u, 36589u, 36424u, 36261u, 36098u, 35936u, 35775u, 35614u, 35455u, 35296u, 35138u, 34981u, 34825u, 34669u, 34514u,
 34360u, 34207u, 34055u, 33903u, 33752u, 33602u, 33453u, 33304u, 33156u, 33009u, 32863u, 32717u, 32572u, 32428u, 32284u,
 32142u, 32000u, 31858u, 31718u, 31578u, 31439u, 31300u, 31162u, 31025u, 30889u, 30753u, 30618u, 30483u, 30350u, 30217u,
 30084u, 29952u, 29821u, 29691u, 29561u, 29432u, 29303u, 29175u, 29048u, 28921u, 28795u, 28670u, 28545u, 28421u, 28298u,
 28175u, 28052u, 27931u, 27809u, 27689u, 27569u, 27450u, 27331u, 27213u, 27095u, 26978u, 26862u, 26746u, 26631u, 26516u,
 26402u, 26288u, 26175u, 26062u, 25950u, 25839u, 25728u, 25618u, 25508u, 25399u, 25290u, 25182u, 25074u, 24967u, 24860u,
 24754u, 24649u, 24544u, 24439u, 24335u, 24231u, 24128u, 24026u, 23924u, 23822u, 23721u, 23620u, 23520u, 23421u, 23321u,
 23223u, 23124u, 23027u, 22929u, 22833u, 22736u, 22640u, 22545u, 22450u, 22356u, 22262u, 22168u, 22075u, 21982u, 21890u,
 21798u, 21707u, 21616u, 21525u, 21435u, 21345u, 21256u, 21167u, 21079u, 20991u, 20903u, 20816u, 20730u, 20643u, 20557u,
 20472u, 20387u, 20302u, 20218u, 20134u, 20051u, 19967u, 19885u, 19802u, 19720u, 19639u, 19558u, 19477u, 19397u, 19317u,
 19237u, 19158u, 19079u, 19000u, 18922u, 18844u, 18767u, 18690u, 18613u, 18537u, 18461u, 18385u, 18310u, 18235u, 18160u,
 18086u, 18012u, 17939u, 17866u, 17793u, 17720u, 17648u, 17576u, 17505u, 17433u, 17363u, 17292u, 17222u, 17152u, 17082u,
 17013u, 16944u, 16876u, 16807u, 16740u, 16672u, 16605u, 16538u, 16471u, 16404u, 16338u, 16272u, 16207u, 16142u, 16077u,
 16012u, 15948u, 15884u, 15820u, 15757u, 15694u, 15631u, 15568u, 15506u, 15444u, 15382u, 15321u, 15260u, 15199u, 15138u,
 15078u, 15018u, 14958u, 14899u, 14839u, 14781u, 14722u, 14663u, 14605u, 14547u, 14490u, 14432u, 14375u, 14318u, 14262u,
 14205u, 14149u, 14094u, 14038u, 13983u, 13928u, 13873u, 13818u, 13764u, 13710u, 13656u, 13602u, 13549u, 13496u, 13443u,
 13390u, 13338u, 13286u, 13234u, 13182u, 13130u, 13079u, 13028u, 12977u, 12927u, 12876u, 12826u, 12776u, 12727u, 12677u,
 12628u, 12579u, 12530u, 12482u, 12433u, 12385u, 12337u, 12289u, 12242u, 12195u, 12147u, 12101u, 12054u, 12007u, 11961u,
 11915u, 11869u, 11823u, 11778u, 11733u, 11688u, 11643u, 11598u, 11554u, 11509u, 11465u, 11421u, 11378u, 11334u, 11291u,
 11248u, 11205u, 11162u, 11120u, 11077u, 11035u, 10993u, 10951u, 10909u, 10868u, 10827u, 10786u, 10745u, 10704u, 10663u,
 10623u, 10583u, 10543u, 10503u, 10463u, 10424u, 10384u, 10345u, 10306u, 10267u, 10229u, 10190u, 10152u, 10114u, 10076u,
 10038u, 10000u, 9962u, 9925u, 9888u, 9851u, 9814u, 9777u, 9741u, 9704u, 9668u, 9632u, 9596u, 9560u, 9525u, 9489u,
 9454u, 9419u, 9384u, 9349u, 9314u, 9279u, 9245u, 9211u, 9176u, 9142u, 9109u, 9075u, 9041u, 9008u, 8975u, 8941u, 8908u,
 8876u, 8843u, 8810u, 8778u, 8746u, 8713u, 8681u, 8649u, 8618u, 8586u, 8555u, 8523u, 8492u, 8461u, 8430u, 8399u, 8368u,
 8338u, 8307u, 8277u, 8247u, 8217u, 8187u, 8157u, 8127u, 8098u, 8068u, 8039u, 8010u, 7981u, 7952u, 7923u, 7894u, 7865u,
 7837u, 7809u, 7780u, 7752u, 7724u, 7696u, 7669u, 7641u, 7613u, 7586u, 7559u, 7531u, 7504u, 7477u, 7450u, 7424u, 7397u,
 7371u, 7344u, 7318u, 7292u, 7265u, 7239u, 7214u, 7188u, 7162u, 7137u, 7111u, 7086u, 7060u, 7035u, 7010u, 6985u, 6960u,
 6936u, 6911u, 6887u, 6862u, 6838u, 6814u, 6789u, 6765u, 6741u, 6718u, 6694u, 6670u, 6647u, 6623u, 6600u, 6576u, 6553u,
 6530u, 6507u, 6484u, 6462u, 6439u, 6416u, 6394u, 6371u, 6349u, 6327u, 6304u, 6282u, 6260u, 6239u, 6217u, 6195u, 6173u,
 6152u, 6130u, 6109u, 6088u, 6067u, 6045u, 6024u, 6003u, 5983u, 5962u, 5941u, 5920u, 5900u, 5879u, 5859u, 5839u, 5819u,
 5799u, 5778u, 5758u, 5739u, 5719u, 5699u, 5679u, 5660u, 5640u, 5621u, 5602u, 5582u, 5563u, 5544u, 5525u, 5506u, 5487u,
 5468u, 5450u, 5431u, 5412u, 5394u, 5375u, 5357u, 5339u, 5321u, 5302u, 5284u, 5266u, 5248u, 5231u, 5213u, 5195u, 5177u,
 5160u, 5142u, 5125u, 5107u, 5090u, 5073u, 5056u, 5039u, 5022u, 5005u, 4988u, 4971u, 4954u, 4937u, 4921u, 4904u, 4888u,
 4871u, 4855u, 4838u, 4822u, 4806u, 4790u, 4774u, 4758u, 4742u, 4726u, 4710u, 4694u, 4679u, 4663u, 4647u, 4632u, 4616u,
 4601u, 4586u, 4570u, 4555u, 4540u, 4525u, 4510u, 4495u, 4480u, 4465u, 4450u, 4436u, 4421u, 4406u, 4392u, 4377u, 4363u,
 4348u, 4334u, 4319u, 4305u, 4291u, 4277u, 4263u, 4249u, 4235u, 4221u, 4207u, 4193u, 4179u, 4166u, 4152u, 4138u, 4125u,
 4111u, 4098u, 4084u, 4071u, 4058u, 4044u, 4031u, 4018u, 4005u, 3992u, 3979u, 3966u, 3953u, 3940u, 3927u, 3915u, 3902u,
 3889u, 3877u, 3864u, 3851u, 3839u, 3826u, 3814u, 3802u, 3789u, 3777u, 3765u, 3753u, 3741u, 3729u, 3717u, 3705u, 3693u,
 3681u, 3669u, 3657u, 3645u, 3634u, 3622u, 3610u, 3599u, 3587u, 3576u, 3564u, 3553u, 3542u, 3530u, 3519u, 3508u, 3497u,
 3485u, 3474u, 3463u, 3452u, 3441u, 3430u, 3419u, 3408u, 3398u, 3387u, 3376u, 3365u, 3355u, 3344u, 3333u, 3323u, 3312u,
 3302u, 3291u, 3281u, 3271u, 3260u, 3250u, 3240u, 3230u, 3219u, 3209u, 3199u, 3189u, 3179u, 3169u, 3159u, 3149u, 3139u,
 3130u, 3120u, 3110u, 3100u, 3091u, 3081u, 3071u, 3062u, 3052u, 3043u, 3033u, 3024u, 3014u, 3005u, 2995u, 2986u, 2977u,
 2968u, 2958u, 2949u, 2940u, 2931u, 2922u, 2913u, 2904u, 2895u, 2886u, 2877u, 2868u, 2859u, 2850u, 2841u, 2833u, 2824u,
 2815u, 2807u, 2798u, 2789u, 2781u, 2772u, 2764u, 2755u, 2747u, 2738u, 2730u, 2722u, 2713u, 2705u, 2697u, 2688u, 2680u,
 2672u, 2664u, 2656u, 2648u, 2640u, 2631u, 2623u, 2615u, 2608u, 2600u, 2592u, 2584u, 2576u, 2568u, 2560u, 2553u, 2545u,
 2537u, 2529u, 2522u, 2514u, 2507u, 2499u, 2491u, 2484u, 2476u, 2469u, 2462u, 2454u, 2447u, 2439u, 2432u, 2425u, 2417u,
 2410u, 2403u, 2396u, 2389u, 2381u, 2374u, 2367u, 2360u, 2353u, 2346u, 2339u, 2332u, 2325u, 2318u, 2311u, 2304u, 2297u,
 2291u, 2284u, 2277u, 2270u, 2263u, 2257u, 2250u, 2243u, 2237u, 2230u, 2224u, 2217u, 2210u, 2204u, 2197u, 2191u, 2184u,
 2178u, 2171u, 2165u, 2159u, 2152u, 2146u, 2140u, 2133u, 2127u, 2121u, 2115u, 2108u, 2102u, 2096u, 2090u, 2084u, 2078u,
 2072u, 2066u, 2060u, 2053u, 2047u, 2042u, 2036u, 2030u, 2024u, 2018u, 2012u, 2006u, 2000u, 1994u, 1989u, 1983u, 1977u,
 1971u, 1966u, 1960u, 1954u, 1949u, 1943u, 1937u, 1932u, 1926u, 1921u, 1915u, 1910u, 1904u, 1899u, 1893u, 1888u, 1882u,
 1877u, 1871u, 1866u, 1861u, 1855u, 1850u, 1845u, 1839u, 1834u, 1829u, 1824u, 1818u, 1813u, 1808u, 1803u, 1798u, 1792u,
 1787u, 1782u, 1777u, 1772u, 1767u, 1762u, 1757u, 1752u, 1747u, 1742u, 1737u, 1732u, 1727u, 1722u, 1718u, 1713u, 1708u,
 1703u, 1698u, 1693u, 1689u, 1684u, 1679u, 1674u, 1670u, 1665u, 1660u, 1656u, 1651u, 1646u, 1642u, 1637u, 1632u, 1628u,
 1623u, 1619u, 1614u, 1610u, 1605u, 1601u, 1596u, 1592u, 1587u, 1583u, 1578u, 1574u, 1570u, 1565u, 1561u, 1556u, 1552u,
 1548u, 1544u, 1539u, 1535u, 1531u, 1526u, 1522u, 1518u, 1514u, 1510u, 1505u, 1501u, 1497u, 1493u, 1489u, 1485u, 1481u,
 1477u, 1472u, 1468u, 1464u, 1460u, 1456u, 1452u, 1448u, 1444u, 1440u, 1436u, 1432u, 1428u, 1425u, 1421u, 1417u, 1413u,
 1409u, 1405u, 1401u, 1397u, 1394u, 1390u, 1386u, 1382u, 1379u, 1375u, 1371u, 1367u, 1364u, 1360u, 1356u, 1352u, 1349u,
 1345u, 1341u, 1338u, 1334u, 1331u, 1327u, 1323u, 1320u, 1316u, 1313u, 1309u, 1306u, 1302u, 1299u, 1295u, 1292u, 1288u,
 1285u, 1281u, 1278u, 1274u, 1271u, 1267u, 1264u, 1261u, 1257u, 1254u, 1251u, 1247u, 1244u, 1241u, 1237u, 1234u, 1231u,
 1227u, 1224u, 1221u, 1218u, 1214u, 1211u, 1208u, 1205u, 1201u, 1198u, 1195u, 1192u, 1189u, 1186u, 1182u, 1179u, 1176u,
 1173u, 1170u, 1167u, 1164u, 1161u, 1158u, 1155u, 1151u, 1148u, 1145u, 1142u, 1139u, 1136u, 1133u, 1130u, 1127u, 1124u,
 1121u, 1119u, 1116u, 1113u, 1110u, 1107u, 1104u, 1101u, 1098u, 1095u, 1092u, 1090u, 1087u, 1084u, 1081u, 1078u, 1075u,
 1073u, 1070u, 1067u, 1064u, 1062u, 1059u, 1056u, 1053u, 1051u, 1048u, 1045u, 1042u, 1040u, 1037u, 1034u, 1032u, 1029u,
 1026u, 1024u, 1021u, 1018u, 1016u, 1013u, 1010u, 1008u, 1005u, 1003u, 1000u, 998u, 995u, 992u, 990u, 987u, 985u, 982u,
 980u, 977u, 975u, 972u, 970u, 967u, 965u, 962u, 960u, 957u, 955u, 953u, 950u, 948u, 945u, 943u, 940u, 938u, 936u, 933u,
 931u, 929u, 926u, 924u, 922u, 919u, 917u, 915u, 912u, 910u, 908u, 905u, 903u, 901u, 899u, 896u, 894u, 892u, 889u, 887u,
 885u, 883u, 881u, 878u, 876u, 874u, 872u, 870u, 867u, 865u, 863u, 861u, 859u, 857u, 854u, 852u, 850u, 848u, 846u, 844u,
 842u, 840u, 838u, 835u, 833u, 831u, 829u, 827u, 825u, 823u, 821u, 819u, 817u, 815u, 813u, 811u, 809u, 807u, 805u, 803u,
 801u, 799u, 797u, 795u, 793u, 791u, 789u, 787u, 785u, 783u, 781u, 780u, 778u, 776u, 774u, 772u, 770u, 768u, 766u, 764u,
 763u, 761u, 759u, 757u, 755u, 753u, 751u, 750u, 748u, 746u, 744u, 742u, 740u, 739u, 737u, 735u, 733u, 732u, 730u, 728u,
 726u, 724u, 723u, 721u, 719u, 717u, 716u, 714u, 712u, 711u, 709u, 707u, 705u, 704u, 702u, 700u, 699u, 697u, 695u, 694u,
 692u, 690u, 689u, 687u, 685u, 684u, 682u, 680u, 679u, 677u, 676u, 674u, 672u, 671u, 669u, 668u, 666u, 664u, 663u, 661u,
 660u, 658u, 656u, 655u, 653u, 652u, 650u, 649u, 647u, 646u, 644u, 643u, 641u, 640u, 638u, 637u, 635u, 634u, 632u, 631u,
 629u, 628u, 626u, 625u, 623u, 622u, 620u, 619u, 617u, 616u, 614u, 613u, 612u, 610u, 609u, 607u, 606u, 604u, 603u, 602u,
 600u, 599u, 597u, 596u, 595u, 593u, 592u, 590u, 589u, 588u, 586u, 585u, 584u, 582u, 581u, 580u, 578u, 577u, 576u, 574u,
 573u, 572u, 570u, 569u, 568u, 566u, 565u, 564u, 562u, 561u, 560u, 558u, 557u, 556u, 555u, 553u, 552u, 551u, 550u, 548u,
 547u, 546u, 545u, 543u, 542u, 541u, 540u, 538u, 537u, 536u, 535u, 533u, 532u, 531u };


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
