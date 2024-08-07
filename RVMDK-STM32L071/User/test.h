/******************************************************************************
Filename    : start.h
Author      : hrs 
Date        : 14/07/2024
Licence     : The Unlicense; see LICENSE for details.
Description : FreeRTOS init functions.
******************************************************************************/

/* Include *******************************************************************/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_conf.h"
#include "core_cm0plus.h"
#include "FreeRTOSConfig.h"
#include "string.h"
/* End Include ***************************************************************/

/* Define ********************************************************************/
#define Print_Int(INT)          Int_Print((int)(INT))
#define Print_Str(STR)          Str_Print((const signed char*)(STR))
/* Counter read wrapper */
#define TEST_CNT_READ()    			((uint16_t)(TIM2->CNT))
/* Memory pool test switch */
/* #define TEST_MEM_POOL */

#define PUTCHAR(CHAR) \
do \
{ \
    USART1->TDR=CHAR; \
    while((USART1->ISR&0x40U)==0U); \
} \
while(0)
/* End Define ****************************************************************/

/* Global ********************************************************************/
void Int_Handler(void);
TIM_HandleTypeDef TIM2_Handle={0};
TIM_HandleTypeDef TIM21_Handle={0};
/* End Global ****************************************************************/

/* Function:Low_Lvl_Init ******************************************************
Description : Initialize underlying hardware.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Low_Lvl_Init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    GPIO_InitTypeDef GPIO_Init;
    UART_HandleTypeDef UART1_Handle;
	
    memset(&RCC_OscInitStruct, 0U, sizeof(RCC_OscInitTypeDef));
    memset(&RCC_ClkInitStruct, 0U, sizeof(RCC_ClkInitTypeDef));
    memset(&GPIO_Init, 0U, sizeof(GPIO_InitTypeDef));
    memset(&UART1_Handle, 0U, sizeof(UART_HandleTypeDef));
   
    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    /* The voltage scaling allows optimizing the power consumption when the device is 
     * clocked below the maximum system frequency, to update the voltage scaling value 
     * regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState=RCC_HSE_ON;
    RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL=RCC_PLL_MUL8;
    RCC_OscInitStruct.PLL.PLLDIV=RCC_PLL_DIV2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clock dividers */
    RCC_ClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
   
    /* Enable USART 1 for user-level operations */
    /* Clock enabling */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    /* UART IO initialization */
    GPIO_Init.Pin=GPIO_PIN_9;
    GPIO_Init.Mode=GPIO_MODE_AF_PP;
    GPIO_Init.Pull=GPIO_PULLUP;
    GPIO_Init.Speed=GPIO_SPEED_HIGH;
    GPIO_Init.Alternate=GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA,&GPIO_Init);
    /* UART initialization */
    UART1_Handle.Instance=USART1;
    UART1_Handle.Init.BaudRate=115200U;
    UART1_Handle.Init.WordLength=UART_WORDLENGTH_8B;
    UART1_Handle.Init.StopBits=UART_STOPBITS_1;
    UART1_Handle.Init.Parity=UART_PARITY_NONE;
    UART1_Handle.Init.HwFlowCtl=UART_HWCONTROL_NONE;
    UART1_Handle.Init.Mode=UART_MODE_TX;
    HAL_UART_Init(&UART1_Handle);
    
    /* Cancel followling comment if SysTick is needed. */
    //SysTick_Config(3600U);
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
    /* TIM2 clock = CPU clock */
    TIM2_Handle.Instance=TIM2;
    TIM2_Handle.Init.Prescaler=0;
    TIM2_Handle.Init.CounterMode=TIM_COUNTERMODE_UP;
    TIM2_Handle.Init.Period=(uint16_t)(-1);
    TIM2_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM2_Handle);
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_TIM_ENABLE(&TIM2_Handle);
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
    /* TIM21 clock = CPU clock */
    TIM21_Handle.Instance=TIM21;
    TIM21_Handle.Init.Prescaler=0;
    TIM21_Handle.Init.CounterMode=TIM_COUNTERMODE_DOWN;
    TIM21_Handle.Init.Period=3600;
    TIM21_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM21_Handle);
    __HAL_RCC_TIM21_CLK_ENABLE();
    __HAL_TIM_ENABLE(&TIM21_Handle);
    /* Clear interrupt pending bit, because we used EGR to update the registers */
    __HAL_TIM_CLEAR_IT(&TIM21_Handle, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&TIM21_Handle);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM21) 
    {
        /* Set the interrupt priority */
        NVIC_SetPriority(TIM21_IRQn,0xFF);
        /* Enable timer 21 interrupt */
        NVIC_EnableIRQ(TIM21_IRQn);
        /* Enable timer 21 clock */
        __HAL_RCC_TIM21_CLK_ENABLE();
    }
}

/* The interrupt handler */
void TIM21_IRQHandler(void)
{
    TIM21->SR=~TIM_FLAG_UPDATE;
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
    /* Disable timer 21 interrupt */
    NVIC_DisableIRQ(TIM21_IRQn);
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
