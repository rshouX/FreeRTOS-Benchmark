/******************************************************************************
Filename    : test.h
Author      : hrs 
Date        : 14/07/2024
Licence     : The Unlicense; see LICENSE for details.
Description : FreeRTOS init functions.
******************************************************************************/

/* Include *******************************************************************/
#include "ch32v30x.h"
#include "debug.h"
#include "core_riscv.h"
#include "FreeRTOSConfig.h"
/* End Include ***************************************************************/

/* Define ********************************************************************/
#define Print_Int(INT)          Int_Print((int)(INT))
#define Print_Str(STR)          Str_Print((const signed char*)(STR))

#define PUTCHAR(CHAR)           putchar(CHAR)
/* End Define ****************************************************************/

/* Global ********************************************************************/
void Int_Handler(void);
TIM_TimeBaseInitTypeDef TIM4_Handle={0};
NVIC_InitTypeDef NVIC_InitStruture={0};
/* End Global ****************************************************************/

/* Function:Low_Lvl_Init ******************************************************
Description : Initialize underlying hardware.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Low_Lvl_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    USART_Printf_Init(115200U);
}
/* End Function:Low_Lvl_Init *************************************************/

/* Function:Timer_Init ********************************************************
Description : Initialize the timer for timing measurements. This function needs
              to be adapted to your specific hardware.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Timer_Init(void)
{
    /* TIM1 clock = CPU clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_InternalClockConfig(TIM1);
    TIM_Cmd(TIM1, ENABLE);
}
/* End Function:Timer_Init ***************************************************/

/* Function:Int_Init **********************************************************
Description : Initialize an periodic interrupt source. This function needs
              to be adapted to your specific hardware.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Int_Init(void)
{
    /* TIM4 clock = CPU clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM4_Handle.TIM_Prescaler = 0;
    TIM4_Handle.TIM_CounterMode = TIM_CounterMode_Down;
    TIM4_Handle.TIM_Period = 14400*4;
    TIM4_Handle.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM4_Handle.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4,&TIM4_Handle);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    NVIC_InitStruture.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruture.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruture.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruture);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}
void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    Int_Handler();
}
/* End Function:Int_Init *****************************************************/

/* Function:Int_Disable *******************************************************
Description : Disable the periodic interrupt source. This function needs
              to be adapted to your specific hardware.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Int_Disable(void)
{
    /* Disable timer 4 interrupt */
    NVIC_DisableIRQ(TIM4_IRQn);
}
/* End Function:Int_Disable **************************************************/

/* Function:Int_Print *********************************************************
Description : Print a signed integer on the debugging console. This integer is
              printed as decimal with sign.
Input       : int Int - The integer to print.
Output      : None.
Return      : int - The length of the string printed.
******************************************************************************/
int Int_Print(int Int)
{
    int Num;
    int Abs;
    int Iter;
    int Count;
    int Div;
    
    /* Exit on zero */
    if(Int==0)
    {
        PUTCHAR('0');
        return 1;
    }
    /* Correct all negatives into positives */
    if(Int<0)
    {
        PUTCHAR('-');
        Abs=-Int;
        Num=1;
    }
    else
    {
        Abs=Int;
        Num=0;
    }

    /* How many digits are there? */
    Count=0;
    Div=1;
    Iter=Abs;
    while(1)
    {
        Iter/=10;
        Count++;
        if(Iter!=0)
            Div*=10;
        else
            break;
    }
    Num+=Count;

    /* Print the integer */
    Iter=Abs;
    while(Count>0)
    {
        Count--;
        PUTCHAR((signed char)(Iter/Div)+'0');
        Iter=Iter%Div;
        Div/=10;
    }
    
    return Num;
}
/* End Function:Int_Print ****************************************************/

/* Function:Str_Print *********************************************************
Description : Print a string on the debugging console.
Input       : const rmp_s8_t* String - The string to print.
Output      : None.
Return      : rmp_cnt_t - The length of the string printed, the '\0' is not included.
******************************************************************************/
int Str_Print(const signed char* String)
{
    uint32_t Count;
    
    for(Count=0U;Count<255U;Count++)
    {
        if(String[Count]==(signed char)'\0')
            break;
        
        PUTCHAR(String[Count]);
    }
    
    return (uint32_t)Count;
}
/* End Function:Str_Print ****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
