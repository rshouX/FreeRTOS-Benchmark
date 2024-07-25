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
#include "semphr.h"
#include "queue.h"
#include "test.h"
/* End Include ***************************************************************/

/* Define ********************************************************************/
/* Number of rounds to test - default to 10000 */
#ifndef ROUND_NUM
#define ROUND_NUM               (10000U)
#endif
#define TEST_CNT_READ()         (TIM1->CNT)
#define OVERFLOW_NUM            (10000U)

/* Memory pool test switch */
#define TEST_MEM_POOL           (4096U)

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
    if(Diff>((uint16_t)(Start-End))) \
        Diff=(uint16_t)(Start-End); \
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

#define SEM_CNT_MAX             (100U)
#define QUEUE_LENGTH            (100U)
#define QUEUE_ITEM_SIZE         sizeof(uint32_t)
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

volatile uint32_t Notify_ISR_Total=0;
volatile uint32_t Notify_ISR_Max=0;
volatile uint32_t Notify_ISR_Min=0;
volatile uint32_t Sem_ISR_Total=0;
volatile uint32_t Sem_ISR_Max=0;
volatile uint32_t Sem_ISR_Min=0;
volatile uint32_t Bmq_ISR_Total=0;
volatile uint32_t Bmq_ISR_Max=0;
volatile uint32_t Bmq_ISR_Min=0;

TaskHandle_t Thd_1;
TaskHandle_t Thd_2;

QueueHandle_t Queue_1;
uint32_t Val_Snt=1U;
SemaphoreHandle_t Sem_1;
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
        Start=TEST_CNT_READ();
        taskYIELD();
    }
}

void Test_Notify_1(void)
{
    int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        /* Read counter here */
        Start=TEST_CNT_READ();
        xTaskNotify(Thd_2,1U,eSetValueWithOverwrite);
    }
}

void Test_Sem_1(void)
{
    int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        /* Read counter here */
        Start=TEST_CNT_READ();
        xSemaphoreGive(Sem_1);
    }
}

void Test_Bmq_1(void)
{
    int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        /* Read counter here */
        Start=TEST_CNT_READ();
        xQueueSend(Queue_1,&Val_Snt,portMAX_DELAY);
    }
}

void Func_1(void* pvParameters)
{
    Print_Str("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");
    vTaskPrioritySet(Thd_1,1U);
    
    Test_Yield_1();
    /* Change priority of thread 2 */
    vTaskPrioritySet(Thd_2,2U);
    Test_Notify_1();
    Test_Sem_1();
    Test_Bmq_1();
    
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
        End=TEST_CNT_READ();
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

void Test_Notify_2(void)
{
    int32_t Count;
    uint32_t Data;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xTaskNotifyWait(0x00,0xFFFFFFFF,&Data,portMAX_DELAY);
        /* Read counter here */
        End=TEST_CNT_READ();
        TEST_DATA();
    }
}

void Test_Sem_2(void)
{
    int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xSemaphoreTake(Sem_1,portMAX_DELAY);
        /* Read counter here */
        End=TEST_CNT_READ();
        TEST_DATA();
    }
}

void Test_Bmq_2(void)
{
    int32_t Count;
    uint32_t Data;
    
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xQueueReceive(Queue_1,&Data,portMAX_DELAY);
        End=TEST_CNT_READ();
        TEST_DATA();
    }
}

void Test_Notify_ISR(void)
{
    uint32_t Data;
    static int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xTaskNotifyWait(0x00,0xFFFFFFFF,&Data,portMAX_DELAY);
        /* Read counter here */
        End=TEST_CNT_READ();
        TEST_DATA();
        Flip=0U;
    }
}


void Test_Sem_ISR(void)
{
    static int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xSemaphoreTake(Sem_1,portMAX_DELAY);
        /* Read counter here */
        End=TEST_CNT_READ();
        TEST_DATA();
        Flip=0U;
    }
}

void Test_Bmq_ISR(void)
{
    uint32_t Data;
    static int32_t Count;
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        xQueueReceive(Queue_1,&Data,portMAX_DELAY);
        /* Read counter here */
        End=TEST_CNT_READ();
        TEST_DATA();
        Flip=0U;
    }
}

#ifdef TEST_MEM_POOL
int32_t Rand(void)
{
    static uint32_t LFSR=0xACE1U;
    
    if((LFSR&0x01U)!=0U)
    {
        LFSR>>=1;
        LFSR^=0xB400U;
    }
    else
        LFSR>>=1;
    
    return LFSR;
}

void Swap(uint32_t* Arg1, uint32_t* Arg2)
{
    uint32_t Temp;
    
    Temp=*Arg1;
    *Arg1=*Arg2;
    *Arg2=Temp;
}

void Test_Mem_Pool(void)
{
    static void* Mem[8];
    static uint32_t Alloc[8];
    static uint32_t Free[8];
    static uint32_t Size[8];
    static uint32_t Amount[8];
    int32_t Case_Cnt;
    int32_t Test_Cnt;
    
    Amount[0]=(TEST_MEM_POOL/32U)*sizeof(uint32_t);
    Amount[1]=(TEST_MEM_POOL/64U+16U)*sizeof(uint32_t);
    Amount[2]=(TEST_MEM_POOL/4U)*sizeof(uint32_t);
    Amount[3]=(TEST_MEM_POOL/128U+32U)*sizeof(uint32_t);
    Amount[4]=(TEST_MEM_POOL/16U)*sizeof(uint32_t);
    Amount[5]=(TEST_MEM_POOL/8U+16U)*sizeof(uint32_t);
    Amount[6]=(TEST_MEM_POOL/128U+64U)*sizeof(uint32_t);
    Amount[7]=(TEST_MEM_POOL/2U-256U)*sizeof(uint32_t);


    /* Initialize the pool */
    for(Test_Cnt=0;Test_Cnt<ROUND_NUM;Test_Cnt++)
    {
        /* Random sequence and number generation */
        for(Case_Cnt=0;Case_Cnt<8;Case_Cnt++)
        {
            Alloc[Case_Cnt]=(uint8_t)Case_Cnt;
            Free[Case_Cnt]=(uint8_t)Case_Cnt;
            Size[Case_Cnt]=(uint8_t)Case_Cnt;
        }
        
        for(Case_Cnt=7;Case_Cnt>0;Case_Cnt--)
        {
            Swap(&Alloc[Case_Cnt], &Alloc[Rand()%((uint32_t)Case_Cnt+1U)]);
            Swap(&Free[Case_Cnt], &Free[Rand()%((uint32_t)Case_Cnt+1U)]);
            Swap(&Size[Case_Cnt], &Size[Rand()%((uint32_t)Case_Cnt+1U)]);
        }
        
        Start=TEST_CNT_READ();
        /* Allocation tests - one of the mallocs may fail if because the management data
         * structure takes up some space. However, the first four must be successful. */
        Mem[Alloc[0]]=pvPortMalloc(Amount[Size[0]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[1]]=pvPortMalloc(Amount[Size[1]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[2]]=pvPortMalloc(Amount[Size[2]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[3]]=pvPortMalloc(Amount[Size[3]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[4]]=pvPortMalloc(Amount[Size[4]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[5]]=pvPortMalloc(Amount[Size[5]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[6]]=pvPortMalloc(Amount[Size[6]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");
        Mem[Alloc[7]]=pvPortMalloc(Amount[Size[7]]);
        if(Mem[Alloc[0]]==NULL) Print_Str("Failure.\r\n");

        /* Deallocation tests */
        vPortFree(Mem[Free[0]]);
        vPortFree(Mem[Free[1]]);
        vPortFree(Mem[Free[2]]);
        vPortFree(Mem[Free[3]]);
        vPortFree(Mem[Free[4]]);
        vPortFree(Mem[Free[5]]);
        vPortFree(Mem[Free[6]]);
        vPortFree(Mem[Free[7]]);
        End=TEST_CNT_READ();
        TEST_DATA();

        /* This should always be successful because we deallocated everything else, and
         * management data structure should never take up more than 1/8 of the pool. */
        Mem[0]=pvPortMalloc(TEST_MEM_POOL*sizeof(uint32_t)*7U/8U);
        if(Mem[0]==NULL)
        {
            Print_Str("Memory test failure: ");
            Print_Int(Test_Cnt);
            Print_Str(" runs.\r\n");
            while(1);
        }
        vPortFree(Mem[0]);
    }
    
    Total/=8U;
    Max/=8U;
    Min/=8U;
}
#endif

void Func_2(void* pvParameters)
{
    /* Yield tests */
    TEST_INIT();
    Test_Yield_2();
    TEST_LIST("Yield                             ");
    
    vTaskPrioritySet(Thd_2,2U);

    /* Mailbox tests */
    TEST_INIT();
    Test_Notify_2();
    TEST_LIST("Notification                      ");

    /* Semaphore tests */
    TEST_INIT();
    Test_Sem_2();
    TEST_LIST("Semaphore                         ");

    /* Mailbox tests */
    TEST_INIT();
    Test_Bmq_2();
    TEST_LIST("Message queue                     ");

#ifdef TEST_MEM_POOL
    /* Memory pool tests */
    TEST_INIT();
    Test_Mem_Pool();
    TEST_LIST("Memory allocation/free pair       ");
#endif

    /* Prepare interrupt tests */
    Int_Init();

    /* Task notification from interrupt tests */
    TEST_INIT();
    Test_Notify_ISR();
    Notify_ISR_Total=Total;
    Notify_ISR_Max=Max;
    Notify_ISR_Min=Min;

    /* Semaphore from interrupt tests */
    TEST_INIT();
    Test_Sem_ISR();
    Sem_ISR_Total=Total;
    Sem_ISR_Max=Max;
    Sem_ISR_Min=Min;

    /* Blocking message queue from interrupt tests */
    TEST_INIT();
    Test_Bmq_ISR();
    Bmq_ISR_Total=Total;
    Bmq_ISR_Max=Max;
    Bmq_ISR_Min=Min;


    Total=Notify_ISR_Total;
    Max=Notify_ISR_Max;
    Min=Notify_ISR_Min;
    TEST_LIST("ISR Notification                  ");

    Total=Sem_ISR_Total;
    Max=Sem_ISR_Max;
    Min=Sem_ISR_Min;
    TEST_LIST("ISR Semaphore                     ");

    Total=Bmq_ISR_Total;
    Max=Bmq_ISR_Max;
    Min=Bmq_ISR_Min;
    TEST_LIST("ISR Message queue                 ");
    
    /* Test stop - Decide whether to exit, or keep dumping counter values
     * to detect potentially wrong timer clock rate configurations */
#ifdef TEST_EXIT
    Test_Exit();
#else
    while(1)
    {
        Start=End;
        End=(uint16_t)TEST_CNT_READ();
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

/* Function:Int_Handler *******************************************************
Description : The interrupt handler. Call this in your periodic ISR.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Int_Handler(void)
{
    BaseType_t Retval;
    BaseType_t Task_Woke=pdFALSE;
    static uint32_t Count=0U;
    uint32_t Val_Snt=1U;
    
    if(Flip!=0U)
        Print_Str("Interrupt reentered.\r\n");
    
    Flip=1U;
    
    if(Count<ROUND_NUM)
    {
        Count++;
        Start=TEST_CNT_READ();
        xTaskNotifyFromISR(Thd_2,1U,eSetValueWithOverwrite,&Task_Woke);
        portYIELD_FROM_ISR(Task_Woke);
    }
    else if(Count<ROUND_NUM*2U)
    {
        Count++;
        Start=TEST_CNT_READ();
        Retval=xSemaphoreGiveFromISR(Sem_1,&Task_Woke);
        portYIELD_FROM_ISR(Task_Woke);
        if(Retval!=pdTRUE)
            Print_Str("ISR semaphore post failed.\r\n");
    }
    else if(Count<ROUND_NUM*3U)
    {
        Count++;
        Start=TEST_CNT_READ();
        Retval=xQueueSendFromISR(Queue_1,&Val_Snt,&Task_Woke);
        portYIELD_FROM_ISR(Task_Woke);
        if(Retval!=pdTRUE)
            Print_Str("ISR bmq message send failed.\r\n");
    }
    else
    {
        Retval=0;
        Int_Disable();
    }
}
/* End Function:Int_Handler **************************************************/

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

    /* Create kernel objects */
    Sem_1=xSemaphoreCreateCounting(SEM_CNT_MAX,0U);
    Queue_1=xQueueCreate(QUEUE_LENGTH,QUEUE_ITEM_SIZE);
    
    Print_Str("====================================================\r\n");

    xTaskCreate(Func_2,
                "Func2",
                256,
                (void*)0x4321U,
                1U,
                &Thd_2);

    xTaskCreate(Func_1,
                "Func1",
                256,
                (void*)0x1234U,
                2U,
                &Thd_1);

    vTaskStartScheduler();
}
/* End Function:main *********************************************************/
