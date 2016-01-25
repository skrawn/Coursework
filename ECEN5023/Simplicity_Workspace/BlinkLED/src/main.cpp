#include "mbed.h"
#include "em_chip.h"

/* Defining LED outputs */
DigitalOut myLed0(LED0);
DigitalOut myLed1(LED1);

Serial pc(USBTX, USBRX);

void ledToggler(void);  /*prototype function*/

/**************************************************************************//**
 * @brief Configure trace output for energyAware Profiler
 * @note  Enabling trace will add 80uA current for the EFM32_Gxxx_STK.
 *        DK's needs to be initialized with SPI-mode:
 * @verbatim BSP_Init(BSP_INIT_DK_SPI); @endverbatim
 *****************************************************************************/
void BSP_TraceSwoSetup(void)
{
  /* Enable GPIO clock */
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

  /* Enable Serial wire output pin */
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

  /* Set correct location */
  /* This location is valid for GG, LG and WG! */
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;

  /* Enable output on correct pin. */
  /* This pin is valid for GG, LG and WG! */
  GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
  GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;

  /* Enable debug clock AUXHFRCO */
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

  /* Wait until clock is ready */
  while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY)) ;

  /* Enable trace in core debug */
  CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  /* Enable PC and IRQ sampling output */
  DWT->CTRL = 0x400113FF;

  /* Set TPIU prescaler to 16. */
  TPI->ACPR = 15;

  /* Set protocol to NRZ */
  TPI->SPPR = 2;

  /* Disable continuous formatting */
  TPI->FFCR = 0x100;

  /* Unlock ITM and output data */
  ITM->LAR = 0xC5ACCE55;
  ITM->TCR = 0x10009;

  /* ITM Channel 0 is used for UART output */
  ITM->TER |= (1UL << 0);
}

int main() {
    /* always add this line at the beginning to insert any chip errata */
    CHIP_Init();

    pc.printf("Initiating blinking LEDs\n\r");

    BSP_TraceSwoSetup();

    myLed0 = 0;
    myLed1 = 0;
    while(1) {
        if (myLed0 ==0) {
            wait(0.5);
            ledToggler();
            }
        else {
            wait(2.0);  /* make the toggling 1/4 ratio */
            ledToggler();
            }
        }
}


void ledToggler(void) {
    myLed0 = !myLed0;
    myLed1 = !myLed0;
}



