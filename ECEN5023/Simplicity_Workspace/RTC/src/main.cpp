#include "mbed.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "sleepmodes.h"

#define CntNumber       16000
#define deBug           true

DigitalOut myled0(LED0);
DigitalOut myled1(LED1);
Serial pc(USBTX, USBRX);

void RTC_Initialization(void);

void RTC_Initialization(void) {
    RTC_Init_TypeDef RTC_InitValues;

    /* Turn on clocks */
    CMU_OscillatorEnable(cmuOsc_LFXO,true,true);
    CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO);
    CMU_ClockEnable(cmuClock_RTC,true);

    /* initialize RTC */
    RTC_InitValues.enable = false;
    RTC_InitValues.debugRun = false;
    RTC_InitValues.comp0Top = true;
    RTC_Init(&RTC_InitValues);

    RTC_CompareSet(0,CntNumber);

    /* Clear Interrupts and then enable COMP0 match interrupt */
    RTC->IFC = RTC_IFC_OF
        | RTC_IFC_COMP0
        | RTC_IFC_COMP1;
    RTC->IEN = RTC_IEN_COMP0;

    NVIC_EnableIRQ(RTC_IRQn);
 }

void RTC_IRQHandler() {
    int intflags;

    intflags = RTC->IF;
    RTC->IFC = intflags;

    if(deBug) pc.printf("Interrupt flags = %i\n\r",intflags);

    myled1 = !myled1;
}

int main() {
    CHIP_Init();
    blockSleepMode(EM3);
    myled0 = 0;
    myled1 = 0;

    if(deBug) pc.printf("Program starting\n\r");

    RTC_Initialization();
    blockSleepMode(EM2);
    RTC_Enable(true);

    if(deBug) blockSleepMode(EM1);

    while(1) {
        sleep();
    }
}
