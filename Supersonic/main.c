#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/fpu.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "string.h"


void TIMER_Init();

uint32_t ui32Period,cnt;
uint8_t flag1,flag2;
int tick1,tick2,ticks;
float distance;

int main(void)
{
	SysCtlClockSet(SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_SYSDIV_40);//主频5M
	ui32Period=SysCtlClockGet();
	TIMER_Init();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_6);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
	//GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_4);
	//GPIOPinConfigure(GPIO_PB4_T1CCP0);
	GPIOPinConfigure(GPIO_PB6_T0CCP0);

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_2);
	//GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);

	while(1)
	{
	    if(flag1&&flag2)
	    {
            flag1=0;
            flag2=0;
	        ticks=(tick1-tick2);
	        if(ticks>=0)
	        {
	            distance=ticks*0.034;//单位mm
	        }
	        cnt=0;
	        IntEnable(INT_TIMER0A);
	    }
	}

}


void TIMER0AIntHandler()
{
    uint32_t status;
    status=TimerIntStatus(TIMER0_BASE,true);
    TimerIntClear(TIMER0_BASE,status);



    if(cnt==0)
    {
        tick1=TimerValueGet(TIMER0_BASE,TIMER_A);
        flag1=1;
    }
    else
    {
        tick2=TimerValueGet(TIMER0_BASE,TIMER_A);
        flag2=1;
        IntDisable(INT_TIMER0A);
    }
    cnt++;
}

void TIMER1AIntHandler()
{
    //uint32_t status;
    //status=TimerIntStatus(TIMER1_BASE,true);
    TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);

    if(GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);
    }
}

/*void TIMER1AIntHandler()
{
    uint32_t status;
    status=TimerIntStatus(TIMER1_BASE,true);
    TimerIntClear(TIMER1_BASE,status);

    tick2=TimerValueGet(TIMER1_BASE,TIMER_A);
    flag2=1;
}*/

void TIMER_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_CAP_TIME);
    TimerControlEvent(TIMER0_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);
    TimerLoadSet(TIMER0_BASE,TIMER_A,65535);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    /*TimerConfigure(TIMER1_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_CAP_TIME);
    TimerControlEvent(TIMER1_BASE,TIMER_A,TIMER_EVENT_NEG_EDGE);
    TimerLoadSet(TIMER1_BASE,TIMER_A,50000);*/

    TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE,TIMER_A,ui32Period/100-1);//Timer1用作产生10μs的Trigger

    //TimerSynchronize(TIMER0_BASE,TIMER_0A_SYNC|TIMER_1A_SYNC);
    //TimerSynchronize(TIMER0_BASE,TIMER_0A_SYNC|TIMER_0B_SYNC);

    TimerIntEnable(TIMER0_BASE,TIMER_CAPA_EVENT);
    //TimerIntEnable(TIMER1_BASE,TIMER_CAPA_EVENT);
    TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER1A);
    IntMasterEnable();

    TimerEnable(TIMER0_BASE,TIMER_A);
    TimerEnable(TIMER1_BASE,TIMER_A);

}


