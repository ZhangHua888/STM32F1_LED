#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "tftlcd.h"
#include "pcf8574.h"
#include "timer.h"
#include "sdram.h"
#include "malloc.h"
#include "touch.h"
#include "GUI.h"
#include "WM.h"
#include "ButtonUse.h"
#include "includes.h"
#include "DIALOG.h"
#include "keyboard.h"
#include "rs422.h"
#include "adc.h"
/************************************************
 ALIENTEK STM32开发板STemWin实验
 STemWin BUTTON使用
 
 UCOSIII中以下优先级用户程序不能使用，ALIENTEK
 将这些优先级分配给了UCOSIII的5个系统内部任务
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
 优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

////TOUCH任务
////设置任务优先级
//#define TOUCH_TASK_PRIO				4
////任务堆栈大小
//#define TOUCH_STK_SIZE				128
////任务控制块
//OS_TCB TouchTaskTCB;
////任务堆栈
//CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
////touch任务
//void touch_task(void *p_arg);

//LED0任务
//设置任务优先级
//#define LED0_TASK_PRIO 				4
////任务堆栈大小
//#define LED0_STK_SIZE				128
////任务控制块
//OS_TCB Led0TaskTCB;
////任务堆栈
//CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
////led0任务
//void led0_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			5
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			512
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

//键盘任务
//设置任务优先级
#define KEYBOARD_TASK_PRIO 				6
//任务堆栈大小
#define KEYBOARD_STK_SIZE					128
//任务控制块
OS_TCB	KeyBoardTaskTCB;
//任务堆栈
CPU_STK	KEYBOARD_TASK_STK[KEYBOARD_STK_SIZE];
//键盘任务
void keyboard_task(void *p_arg);


extern GUI_CONST_STORAGE GUI_FONT GUI_Fontsong18;
int main(void)
{
    OS_ERR err;
	CPU_SR_ALLOC();
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
     uart1_init();
		uart2_init();	              //初始化USART
	Beep_Init();
	  Heat_Init();
	   MY_ADC_Init();                  //初始化ADC1通道5
  //  KEY_Init();                     //初始化按键
    PCF8574_Init();                 //初始化PCF8574
		KEY3X7_ON(); 										//打开键盘
    SDRAM_Init();                   //SDRAM初始化
    TFTLCD_Init();  		        //LCD初始化
    TP_Init();				        //触摸屏初始化
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
    TIM3_Init(60000-1,9000-1);//定时3秒
    OSInit(&err);		            //初始化UCOSIII
	OS_CRITICAL_ENTER();            //进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	            //退出临界区	 
	OSStart(&err);                  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	//使能时间片轮转调度功能,设置默认的时间片长度
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	__HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
	GUI_Init();  			//STemWin初始化
	WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏可能会用到
	OS_CRITICAL_ENTER();	//进入临界区
	//STemWin Demo任务	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//触摸屏任务
//	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
//				 (CPU_CHAR*   )"Touch task", 		
//                 (OS_TASK_PTR )touch_task, 			
//                 (void*       )0,					
//                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
//                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
//                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
//                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
//                 (OS_MSG_QTY  )0,					
//                 (OS_TICK	  )0,  					
//                 (void*       )0,					
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR*     )&err);			 //LED0任务
//	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
//				 (CPU_CHAR*   )"Led0 task", 		
//                 (OS_TASK_PTR )led0_task, 			
//                 (void*       )0,					
//                 (OS_PRIO	  )LED0_TASK_PRIO,     
//                 (CPU_STK*    )&LED0_TASK_STK[0],	
//                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
//                 (CPU_STK_SIZE)LED0_STK_SIZE,		
//                 (OS_MSG_QTY  )0,					
//                 (OS_TICK	  )0,  					
//                 (void*       )0,					
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR*     )&err);	 
//键盘任务
		OSTaskCreate((OS_TCB*     )&KeyBoardTaskTCB,		
				 (CPU_CHAR*   )"KeyBoard task", 		
                 (OS_TASK_PTR )keyboard_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )KEYBOARD_TASK_PRIO,     
                 (CPU_STK*    )&KEYBOARD_TASK_STK[0],	
                 (CPU_STK_SIZE)KEYBOARD_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEYBOARD_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);	 							 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
	
	//更换皮肤
//	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
//	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
//	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
//	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
//	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
//	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
//	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
//	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
//	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
//	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
//	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
//	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX); 
			MainTask();
	while(1)
	{
	
	//	GUI_Delay(100); 
	}
}


////TOUCH任务
//void touch_task(void *p_arg)
//{
//	OS_ERR err;
//	while(1)
//	{
//		GUI_TOUCH_Exec();	
//		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
//	}
//}

//LED0任务
//void led0_task(void *p_arg)
//{
//	OS_ERR err;
////	pingmujiare();
//	while(1)
//	{
////	OS_CRITICAL_CRITICAL();
//    uart1_send();
//		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
//	}
//}


void keyboard_task(void *p_arg)
	{	
	//	u8 key;
	//	u8 mode[2];
	//	GUI_UC_SetEncodeUTF8();
		OS_ERR err;
		//GUI_SetFont(&GUI_Fontsong18); 
				while(1)
					{ 
						
						send_data();
						uart1_send();
					}
	}
		

