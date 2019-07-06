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
#include "string.h"

uint32_t clock;
uint32_t CH[4];
uint8_t cnt;
uint32_t flag=0;
char ch[50];
void CHSet(uint8_t counter,uint32_t duty);
void UART0Send(const uint8_t *pucBuffer,uint32_t ulCount);
void UART3Send(const uint8_t *pucBuffer,uint32_t ulCount);
void GPIOPWM_Init();
void UART0_Init();
void UART3_Init();

int main (void)
{

    FPUEnable();
    FPULazyStackingEnable();

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    clock=SysCtlClockGet();
    GPIOPWM_Init();
    UART0_Init();
    UART3_Init();

    while(1)
    {
        if(strstr(ch,"PIT")!=NULL)
        {
            cnt=1;
            CH[cnt]=100000*(0.05)*(100+(ch[3]-'0')*100+(ch[4]-'0')*10+(ch[5]-'0'))/100;
            CHSet(cnt,CH[cnt]);
            //memset(ch,0,sizeof(ch));
        }
        if(strstr(ch,"ROL")!=NULL)
        {
            cnt=2;
            CH[cnt]=100000*(0.05)*(100+(ch[3]-'0')*100+(ch[4]-'0')*10+(ch[5]-'0'))/100;
            CHSet(cnt,CH[cnt]);
            //memset(ch,0,sizeof(ch));
        }
        if(strstr(ch,"UPD")!=NULL)
        {
            cnt=3;
            CH[cnt]=100000*(0.05)*(100+(ch[3]-'0')*100+(ch[4]-'0')*10+(ch[5]-'0'))/100;
            CHSet(cnt,CH[cnt]);
            //memset(ch,0,sizeof(ch));
        }
        if(strstr(ch,"YAW")!=NULL)
        {
            cnt=4;
            CH[cnt]=100000*(0.05)*(100+(ch[3]-'0')*100+(ch[4]-'0')*10+(ch[5]-'0'))/100;
            CHSet(cnt,CH[cnt]);
            //memset(ch,0,sizeof(ch));
        }
        if(strstr(ch,"LOCK")!=NULL)
        {
            CHSet(1,7500);
            CHSet(2,7500);
            CHSet(3,5000);
            CHSet(4,5000);
            //memset(ch,0,sizeof(ch));
        }
        if(strstr(ch,"UNL")!=NULL)
        {
            CHSet(1,7500);
            CHSet(2,7500);
            CHSet(3,5000);
            CHSet(4,7500);//�Ȱ���ͨ������һ�Σ�������������
            CHSet(4,10000);
            //memset(ch,0,sizeof(ch));
        }
    }

}

void CHSet(uint8_t counter,uint32_t duty)
{
    uint32_t OUT;
    switch(counter)
    {
    case 1:{OUT=PWM_OUT_2;break;}
    case 2:{OUT=PWM_OUT_3;break;}
    case 3:{OUT=PWM_OUT_4;break;}
    case 4:{OUT=PWM_OUT_5;break;}
    }
    PWMPulseWidthSet(PWM0_BASE,OUT,duty);
}


void GPIOPWM_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE,GPIO_PIN_4);
    GPIOPinTypePWM(GPIO_PORTB_BASE,GPIO_PIN_5);
    GPIOPinTypePWM(GPIO_PORTE_BASE,GPIO_PIN_4);
    GPIOPinTypePWM(GPIO_PORTE_BASE,GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);
    GPIOPinConfigure(GPIO_PE4_M0PWM4);
    GPIOPinConfigure(GPIO_PE5_M0PWM5);

    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_4,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_5,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_5,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

    PWMGenConfigure(PWM0_BASE,PWM_GEN_1,PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE,PWM_GEN_2,PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_1,100000);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_2,100000);

    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,10000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_3,10000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,5000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_5,5000);


    /*PWMPulseWidthSet(PWM0_BASE,PWM_OUT_2,10000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_3,10000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_4,5000);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_5,10000);*/

    PWMOutputState(PWM0_BASE, (PWM_OUT_2_BIT | PWM_OUT_3_BIT | PWM_OUT_4_BIT | PWM_OUT_5_BIT), true);

    PWMGenEnable(PWM0_BASE,PWM_GEN_1);
    PWMGenEnable(PWM0_BASE,PWM_GEN_2);
}

void UART0Send(const uint8_t *pucBuffer,uint32_t ulCount)
{
    while(ulCount--)
    {
        UARTCharPutNonBlocking(UART0_BASE,*pucBuffer++);
    }
}

void UART3Send(const uint8_t *pucBuffer,uint32_t ulCount)
{
    while(ulCount--)
    {
        UARTCharPutNonBlocking(UART3_BASE,*pucBuffer++);
    }
}

void UART0_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE,UART_INT_RX|UART_INT_RT);
    IntMasterEnable();
    UART0Send((uint8_t*)"Welcome!\r",25);
    UARTCharPut(UART0_BASE,'\n');
}

void UART3_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE,GPIO_PIN_6|GPIO_PIN_7);

    UARTConfigSetExpClk(UART3_BASE,SysCtlClockGet(),9600,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

    UARTFIFOLevelSet(UART3_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8);
    IntEnable(INT_UART3);
    UARTIntEnable(UART3_BASE,UART_INT_RX|UART_INT_RT);
    IntMasterEnable();
    UART3Send((uint8_t*)"Welcome!\r",25);
    UARTCharPut(UART3_BASE,'\n');
}

void UART0IntHandler(void)
{
    uint32_t ulStatus;
    uint8_t i=0;
    ulStatus=UARTIntStatus(UART0_BASE,true);
    UARTIntClear(UART0_BASE,ulStatus);


    while(UARTCharsAvail(UART0_BASE))
    {
        ch[i] = UARTCharGet(UART0_BASE);
        UARTCharPut(UART0_BASE,ch[i]);
        i++;
    }
    UARTCharPutNonBlocking(UART0_BASE,'\r');
    UARTCharPutNonBlocking(UART0_BASE,'\n');

}

void UART3IntHandler(void)
{
    uint32_t ulStatus;
    uint8_t i=0;
    ulStatus=UARTIntStatus(UART3_BASE,true);
    UARTIntClear(UART3_BASE,ulStatus);


    while(UARTCharsAvail(UART3_BASE))
    {
        ch[i] = UARTCharGet(UART3_BASE);
        UARTCharPut(UART3_BASE,ch[i]);
        i++;
    }
    UARTCharPutNonBlocking(UART3_BASE,'\r');
    UARTCharPutNonBlocking(UART3_BASE,'\n');

}
