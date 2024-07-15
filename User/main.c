/******************************************************************************
Filename    : FreeRTOS_benchmark.c
Author      : hrs 
Date        : 12/06/2024
Licence     : The Unlicense; see LICENSE for details.
Description : The performance benchmark for FreeRTOS on RVM.
******************************************************************************/

/* Include *******************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "test.h"
/* End Include ***************************************************************/

/* Define ********************************************************************/
#define TIM2_CR1                *((volatile uint32_t*)(TIM2_BASE+0x00U))
#define TIM2_ARR                *((volatile uint32_t*)(TIM2_BASE+0x2CU))
#define TIM2_PSC                *((volatile uint32_t*)(TIM2_BASE+0x28U))
#define TIM2_CNT                *((volatile uint32_t*)(TIM2_BASE+0x24U))
/* Number of rounds to test - default to 10000 */
#ifndef ROUND_NUM
#define ROUND_NUM               (10000U)
#endif
#define RMP_CNT_READ()          ((TIM2_CNT)<<1)
#define OVERFLOW_NUM            (10000U)

/* Data initialization */
#define TEST_INIT() \
do \
{ \
    Total=0U; \
    Max=0U; \
    Min=((uint16_t)-1U); \
} \
while(0)

/* Data extraction */
#define TEST_DATA() \
do \
{ \
    Diff=(uint16_t)(End-Start); \
    Total+=(Diff); \
    Max=(Diff)>(Max)?(Diff):(Max); \
    Min=(Diff)<(Min)?(Diff):(Min); \
} \
while(0)

/* Data printing */
#define TEST_LIST(X) \
do \
{ \
    Print_Str(X); \
    Print_Str(" : "); \
    Print_Int(Total/ROUND_NUM); \
    Print_Str(" / "); \
    Print_Int(Max); \
    Print_Str(" / "); \
    Print_Int(Min); \
    Print_Str("\r\n"); \
} \
while(0)
/* End Define ****************************************************************/

/* Global ********************************************************************/
volatile uint32_t Flip=0U;
volatile uint16_t Start=0U;
volatile uint16_t End=0U;
volatile uint16_t Diff=0U;
volatile uint16_t Min=0U;
volatile uint16_t Max=0U;
volatile uint32_t Overflow=0U;
volatile uint32_t Total=0U;

TaskHandle_t Thd_1;
TaskHandle_t Thd_2;
/* End Global ****************************************************************/

/* Function:Func_1 ************************************************************
Description : The test function group 1.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Test_Yield_1(void)
{
    uint32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        /* Test flip result */
        if(Flip!=0U)
        {
            Print_Str("Yield error in Thd1 @ round ");
            Print_Int(Count);
            Print_Str(".\r\n");
        }
        Flip=1U;
        /* Read counter here */
        Start=RMP_CNT_READ();
        taskYIELD();
    }
}

void Func_1(void* pvParameters)
{
#ifdef FLOAT_CONTEXT
    Float+=1.0f;
#endif

    Print_Str("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");
    vTaskPrioritySet(Thd_1,1U);
    
    Test_Yield_1();
    /* Change priority of thread 2 */
    vTaskPrioritySet(Thd_2,2U);
//    Test_Mail_1();
//    Test_Sem_1();
//    Test_Msgq_1();
//    Test_Bmq_1();
    while(1);
}
/* End Function:Func_1 *******************************************************/

/* Function:Func_2 ************************************************************
Description : The test function group 2.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Test_Yield_2(void)
{
    uint32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        taskYIELD();
        /* Read counter here */
        End=RMP_CNT_READ();
        TEST_DATA();
        /* Test flip result */
        if(Flip==0U)
        {
            Print_Str("Yield error in Thd2 @ round ");
            Print_Int(Count);
            Print_Str(", ");
            Print_Int(Start);
            Print_Str(", ");
            Print_Int(End);
            Print_Str(".\r\n");
        }
        Flip=0U;
    }
}

void Func_2(void* pvParameters)
{
#ifdef FAULT_INJECT
    RMP_DBG_S("Injecting fault by accessing NULL address.\r\n");
    *((volatile rmp_ptr_t*)0U)=0U;
#endif
    
#ifdef FLOAT_CONTEXT
    Float+=1.0f;
#endif
    
    
    /* Yield tests */
    TEST_INIT();
    Test_Yield_2();
    TEST_LIST("Yield                             ");
    
//    /* Elevate priority of thread 2 for IPC tests */
//    RMP_Thd_Set(&Thd_2,2U,RMP_SLICE_MAX);
//    
//    /* Mailbox tests */
//    RMP_INIT();
//    Test_Mail_2();
//    RMP_LIST("Mailbox                           ");
//    
//    /* Semaphore tests */
//    RMP_INIT();
//    Test_Sem_2();
//    RMP_LIST("Semaphore                         ");
//    
//    /* Fifo tests */
//    RMP_INIT();
//    Test_Fifo();
//    RMP_LIST("FIFO                              ");
//    
//    /* Message queue tests */
//    RMP_INIT();
//    Test_Msgq_2();
//    RMP_LIST("Message queue                     ");
//    
//    /* Blocking message queue tests */
//    RMP_INIT();
//    Test_Bmq_2();
//    RMP_LIST("Blocking message queue            ");
//    
//    /* Memory pool tests */
//#ifdef TEST_MEM_POOL
//    RMP_INIT();
//    Test_Mem_Pool();
//    RMP_LIST("Memory allocation/free pair       ");
//#endif

//    /* Prepare interrupt tests */
//    Int_Init();
//    
//    /* Mailbox from interrupt tests */
//    RMP_INIT();
//    Test_Mail_ISR();
//    Mail_ISR_Total=Total;
//    Mail_ISR_Max=Max;
//    Mail_ISR_Min=Min;
//    
//    /* Semaphore from interrupt tests */
//    RMP_INIT();
//    Test_Sem_ISR();
//    Sem_ISR_Total=Total;
//    Sem_ISR_Max=Max;
//    Sem_ISR_Min=Min;
//    
//    /* Message queue from interrupt tests */
//    RMP_INIT();
//    Test_Msgq_ISR();
//    Msgq_ISR_Total=Total;
//    Msgq_ISR_Max=Max;
//    Msgq_ISR_Min=Min;
//    
//    /* Blocking message queue from interrupt tests */
//    RMP_INIT();
//    Test_Bmq_ISR();
//    Bmq_ISR_Total=Total;
//    Bmq_ISR_Max=Max;
//    Bmq_ISR_Min=Min;
//    
//    /* Print results */
//    Total=Mail_ISR_Total;
//    Max=Mail_ISR_Max;
//    Min=Mail_ISR_Min;
//    RMP_LIST("ISR Mailbox                       ");
//    
//    Total=Sem_ISR_Total;
//    Max=Sem_ISR_Max;
//    Min=Sem_ISR_Min;
//    RMP_LIST("ISR Semaphore                     ");
//    
//    Total=Msgq_ISR_Total;
//    Max=Msgq_ISR_Max;
//    Min=Msgq_ISR_Min;
//    RMP_LIST("ISR Message queue                 ");
//    
//    Total=Bmq_ISR_Total;
//    Max=Bmq_ISR_Max;
//    Min=Bmq_ISR_Min;
//    RMP_LIST("ISR Blocking message queue        ");
    
    /* Test stop - Decide whether to exit, or keep dumping counter values
     * to detect potentially wrong timer clock rate configurations */
#ifdef TEST_EXIT
    Test_Exit();
#else
    while(1)
    {
        Start=End;
        End=(uint16_t)RMP_CNT_READ();
        if(Start>End)
        {
            Overflow++;
            if((Overflow%OVERFLOW_NUM)==0U)
            {
                Print_Int(OVERFLOW_NUM);
                Print_Str(" overflows\r\n");
            }
        }
    }
#endif
}
/* End Function:Func_2 *******************************************************/

/* Function:main **************************************************************
Description : The entry of the FreeRTOS.
Input       : None.
Output      : None.
Return      : int - This function never returns.
******************************************************************************/
int main()
{
    Low_Lvl_Init();
    
    /* Initialize timer 2 */
    Timer_Init();
    
    xTaskCreate(Func_2,
                "Test Function 2",
                256,
                (void*)0x4321U,
                1U,
                &Thd_2);
    
    /* Print table header */
    xTaskCreate(Func_1,
                "Test Function 1",
                256,
                (void*)0x1234U,
                2U,
                &Thd_1);
    
    vTaskStartScheduler();
}
/* End Function:main *********************************************************/
