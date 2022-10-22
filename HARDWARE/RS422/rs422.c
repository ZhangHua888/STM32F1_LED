#include "rs422.h"
#include "key.h"
#include "math.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "sys.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 



UART_HandleTypeDef UART1_Handler; //UART1句柄
UART_HandleTypeDef UART2_Handler; //UART2句柄
		float FW_angle,FW_ans;
		float FY_angle,FY_ans;
		float LW_angle1,LW_ans1;
		float LW_angle2,LW_ans2;
		int FW_change=0;
		int FY_change=0;
	  float ZT1;
		float ZT2;
		float LW1;
		float LW2;
		float HY,ZY,SY,Height;
		int NA_JZ=0,NA_LW=0;
		float ans=0;
	  int mode[2]={0};//稳定模式下的菜单
		int nums=0;//用来切换菜单的
		u8 BZ=0;
		int SD_BZ=0;//锁定标志
		int BEEP_BZ=0;
		int FH_BZ1=1,FH_BZ2=1,FH_BZ3=1,FH_BZ4=1;
			int Reuse[2]={0,0};//复用功能按键数组
			int FW_blink , FY_blink , LW1_blink , LW2_blink ;//闪烁标志
			extern int aotuexit;
u8 rdata1[17]={0},rdata11[34]={0},rdata12[17]={0};//rdata12表示一个中间数组，当检验到rdata12数组正确时才会将rdata12数组赋给tdata1
u8 rdata2[11]={0},rdata21[22]={0},rdata22[11]={0};
u8 tdata[8]={0X5A,0X10,0X00,0X00,0X00,0X00,0X00,0X00};//第一个字节，第二个字节是固定的



extern int state;


void uart1_init()//串口一初始化
{
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=115200;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式

	 
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1		
		HAL_UART_Receive_IT(&UART1_Handler,rdata11,sizeof(rdata11));  //使能串口接收中断

}
void uart2_init()//串口二初始化
{
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=115200;				    //波特率
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	
	
   HAL_UART_Init(&UART2_Handler);	
	 HAL_UART_Receive_IT(&UART2_Handler,rdata21,sizeof(rdata21));  //使能串口接收中断

}
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
   	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10;			//PA9 10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化
		
		
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);			//抢占优先级3，子优先级3
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
	
		
	}
	if(huart->Instance==USART2)//如果是串口2，进行串口2 MSP初始化
		{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
		GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_2;			//PA0,1
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化

		
		
		HAL_NVIC_SetPriority(USART2_IRQn,3,3);			//抢占优先级3，子优先级3
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART2中断通道
	
		
	}

}

 void USART1_IRQHandler()
{
	
					#if SYSTEM_SUPPORT_OS	 	//使用OS
					OSIntEnter();    
				#endif
	
		HAL_UART_IRQHandler(&UART1_Handler);
		HAL_UART_Receive_IT(&UART1_Handler,rdata11,sizeof(rdata11));   //使能串口接收中断
	
	#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
    #endif
}

void USART2_IRQHandler()
{
	
		#if SYSTEM_SUPPORT_OS	 	//使用OS
					OSIntEnter();    
				#endif
	
	
		HAL_UART_IRQHandler(&UART2_Handler);
		HAL_UART_Receive_IT(&UART2_Handler,rdata21,sizeof(rdata21));   //使能串口接收中断
	
	#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口接收中断处理函数
{
	
	if(huart->Instance==USART1)
{
	u8 sum=0x00;
	
			for(int i=0;i<34;i++){
					if(rdata11[i]==0XA5){
								 for(int j=0;j<17;j++){
										rdata12[j]=rdata11[i];
										i++;
								 }
						     break;
						}
				}
			if(rdata12[0]==0XA5&&rdata12[1]==0X81)//转台发送数据定义
		{	
			NA_LW=1;
			for(int d=0;d<16;d++)  sum=sum^rdata12[d];
			if(rdata12[16]==sum){
			for(int i=0;i<17;i++) rdata1[i]=rdata12[i];
			                      ZT1=uncoding1(4);		
														ZT2=uncoding1(8);											
														LW_ans1=uncoding2(12);
														LW_ans2=uncoding2(14);			
			}
			
		}
	
}//串口一
//HAL_UART_Transmit(&UART1_Handler,rdata1,sizeof(rdata1),1000);//串口1发送
	
	if(huart->Instance==USART2)//串口2
	{	
		int JY=0X00;

			for(int i=0;i<22;i++){
					if(rdata21[i]==0XA5){
								 for(int j=0;j<11;j++){
										rdata22[j]=rdata21[i];
										i++;
								 }
						     break;
						}
				}
												if(rdata22[0]==0XA5&&rdata22[1]==0X82)
													{		
															NA_JZ=1;
															for(int d=0;d<10;d++) JY=JY^rdata22[d];
														if(rdata22[10]==JY)
														{
															for(int i=0;i<11;i++) rdata2[i]=rdata22[i];
																	HY=uncoding3(2);
																	ZY=uncoding3(4);
																	SY=uncoding3(6);
																	Height=uncoding3(8);									
														
		  										}
													}									
	}

}

void coding1(float num,char num1)//
{
int a;
a=num*1000;
tdata[num1+3]=a;
tdata[num1+2]=a>>8;
tdata[num1+1]=a>>16;
tdata[num1]=a>>24;
}
void coding2(float num,char num1)
{
    short a;
   a=num*100;
   tdata[num1+1]=a;
   tdata[num1]=a>>8;
}
float uncoding1(char num1)
{
    int a;
    a=rdata1[num1];
    a=a<<8;
    a|=rdata1[num1+1];
    a=a<<8;
    a|=rdata1[num1+2];
    a=a<<8;
    a|=rdata1[num1+3];
    return a/1000.0;
}
float uncoding2(char num1)
{
    short a;
    a=rdata1[num1];
    a=a<<8;
    a|=rdata1[num1+1];
    return a/100.0;
}

float uncoding3(char num1)
{
	
    short a;
    a=rdata2[num1];
    a=a<<8;
    a|=rdata2[num1+1];
    return a/100.0;
}


float Read_Number()//从键盘中读取数字赋给方位角
{
	
	FW_blink=1;
	int number[17]={0};//{2,3,10,2,3,12}
	int piont;
	int i=0,key,a=0;
	   FH_BZ1=1;
	for(int t=0;t<17;t++)number[t]=-1;
	while(i<17){
			key = KeyScan_3X7();
		if((Reuse[1]>0&&Reuse[1]<10)||Reuse[1]==13){//复用数组的第二位为数字或者负号，直接赋值给读数数组
					//number[0]=Reuse[1];
			    key=Reuse[1];
			for(int i=0;i<2;i++)//每发送一次后清除复用功能数组
						Reuse[i]=0; 
				}
			if(key>0&&key<=13){
					Open_Time();
					 if(key==11) number[i]=0;
				else number[i]=key;//输入数组赋值
			}
		
		if(number[i]==12||aotuexit==1) {
			//FW_blink=0;
//			Close_Time();
			if(aotuexit==1){
									Close_Time();
								aotuexit=0;
							}
			break;
		}
		if(key==14){     //删除清零输入数组
				for(int t=0;t<17;t++)number[t]=-1;
          FW_ans=0;//方位显示
					ans=0;
				  i=0;
			    a=0;
					 FH_BZ1=1;
			}
		if(FH_BZ1==-1){
					if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
								ans=ans*10;
								ans=ans+number[i];
							if(ans>=-30&&ans<=30)  
								FW_ans=-ans;
					}		  	
					if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
					{
						piont=i;
						a=1;
					}
					if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
						ans+=number[i]*pow(10,piont-i);
							if(ans>=-30&&ans<=30)  
							FW_ans=-ans;
					}
					 
					if((key>0&&key<=11)||key==12){
						 i++;   
					}
		
		}else{
						if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
									ans=ans*10;
									ans=ans+number[i];
								if(ans>=-30&&ans<=30)  
									FW_ans=ans;
						}		  	
						if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
						{
							piont=i;
							a=1;
						}
						if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
							ans+=number[i]*pow(10,piont-i);
								if(ans>=-30&&ans<=30)  
								FW_ans=ans;
						}
						 
						if((key>0&&key<=11)||key==12){
							
							   i++;
					    
						}
		}
//		  if(number[0]==10&&number[1]==10){
//    number[0]=-1; number[1]=-1;
//     FW_ans=0;//
//     ans=0;
//      i=0;
//	   a=0;
//    FH_BZ1=-1;
//  }
			 if(number[0]==13){//检测到负号按下
						  number[0]=-1; 
						  FW_ans=0;//
						  ans=0;
							i=0;
						  a=0;
						 FH_BZ1=-1;
			}
		}
	FW_blink=0;
		return 	FW_ans;
}

float Read_Number1()//从键盘中读取数字赋给俯仰角
{  Open_Time();
		FY_blink=1;
		int number[17];//{2,3,10,2,3,12}
	int piont;
	int i=0,key,a=0;
	  FH_BZ2=1;
		for(int t=0;t<17;t++)number[t]=-1;
	while(i<17){
		  if((Reuse[1]>0&&Reuse[1]<10)||Reuse[1]==13){//复用数组的第二位为数字或者负号，直接赋值给读数数组
					number[0]=Reuse[1];
					i++;
				}
				key = KeyScan_3X7();
			if(key>0&&key<=13){
					Open_Time();
				 if(key==11) number[i]=0;
				else number[i]=key;//输入数组赋值
			}
	if(number[i]==12||aotuexit==1) {

			if(aotuexit==1){
									Close_Time();
								aotuexit=0;
							}
			break;
		}
		if(key==14){     //删除清零输入数组
				for(int t=0;t<17;t++)number[t]=-1;
          FY_ans=0;//方位显示
					ans=0;
				  i=0;
			    a=0;
					 FH_BZ2=1;
			}
		if(FH_BZ2==-1){
					if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
								ans=ans*10;
								ans=ans+number[i];
							if(ans>=-30&&ans<=30) 
								FY_ans=-ans;
					}		  	
					if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
					{
						piont=i;
						a=1;
					}
					if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
						ans+=number[i]*pow(10,piont-i);
							if(ans>=-30&&ans<=30)  
							FY_ans=-ans;
					}
					 
					if((key>0&&key<=11)||key==12){
						 i++;   
					}
		
		}else{
						if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
									ans=ans*10;
									ans=ans+number[i];
									if(ans>=-30&&ans<=30)  FY_ans=ans;
						}		  	
						if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
						{
							piont=i;
							a=1;
						}
						if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
							ans+=number[i]*pow(10,piont-i);
								if(ans>=-30&&ans<=30)  FY_ans=ans;
						}
						 
						if((key>0&&key<=11)||key==12){
							 i++;   
						}
		}
		 if(number[0]==13){//检测到负号按下
						  number[0]=-1; 
						  FY_ans=0;//
						  ans=0;
							i=0;
						  a=0;
						 FH_BZ2=-1;
			}
		}
	FY_blink=0;
		return 	FY_ans;
}

float Read_Number2()//从键盘中读取数字赋给零位角1
{ 
	Open_Time();
LW1_blink=1;
	int number[17]={0};//{2,3,10,2,3,12}
	int piont;
	int i=0,key,a=0;
	  FH_BZ3=1;
		for(int t=0;t<17;t++)number[t]=-1;
	while(i<17){
				key = KeyScan_3X7();
			if(key>0&&key<=13){
					Open_Time();
			 if(key==11) number[i]=0;
				else number[i]=key;//输入数组赋值
			}
			if(number[i]==12||aotuexit==1) {
			if(aotuexit==1){
									Close_Time();
								aotuexit=0;
							}
			break;
		}
       
		if(key==14){     //删除清零输入数组
					for(int t=0;t<17;t++)number[t]=-1;
          LW_ans1=0;//方位显示
					ans=0;
				  i=0;
			    a=0;
					FH_BZ3=1;
			}
		if(FH_BZ3==-1){
					if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
								ans=ans*10;
								ans=ans+number[i];
							if(ans>=-10&&ans<=10) 
								LW_ans1=-ans;
					}		  	
					if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
					{
						piont=i;
						a=1;
					}
					if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
						ans+=number[i]*pow(10,piont-i);
							if(ans>=-10&&ans<=10) 
							LW_ans1=-ans;
					}
					 
					if((key>0&&key<=11)||key==12){
						 i++;   
					}
		
		}else{
						if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
									ans=ans*10;
									ans=ans+number[i];
								if(ans>=-10&&ans<=10) 
									LW_ans1=ans;
						}		  	
						if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
						{
							piont=i;
							a=1;
						}
						if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
							ans+=number[i]*pow(10,piont-i);
								if(ans>=-10&&ans<=10) 
								LW_ans1=ans;
						}
						 
						if((key>0&&key<=11)||key==12){
							 i++;   
						}
		}
//		  if(number[0]==10&&number[1]==10){
//    number[0]=-1; number[1]=-1;
//     LW_ans1=0;//
//     ans=0;
//      i=0;
//	   a=0;
//    FH_BZ3=-1;
//  }
		 if(number[0]==13){//检测到负号按下
						  number[0]=-1; 
						  LW_ans1=0;//
						  ans=0;
							i=0;
						  a=0;
						 FH_BZ3=-1;
			}
			
		}
	LW1_blink=0;
		return 	LW_ans1;
}

float Read_Number3()//从键盘中读取数字赋给零位角2
{
	 Open_Time();
		LW2_blink=1;
		int number[17]={0};//{2,3,10,2,3,12}
	int piont;
	int i=0,key,a=0;
		  FH_BZ4=1;
		for(int t=0;t<17;t++)number[t]=-1;
	while(i<17){
				key = KeyScan_3X7();
			if(key>0&&key<=13){
					Open_Time();
			 if(key==11) number[i]=0;
				else number[i]=key;//输入数组赋值
			}
				if(number[i]==12||aotuexit==1) {
			if(aotuexit==1){
									Close_Time();
								aotuexit=0;
							}
			break;
		}
       
       
       
		if(key==14){     //删除清零输入数组
					for(int t=0;t<17;t++)number[t]=-1;
          LW_ans2=0;//方位显示
					ans=0;
				  i=0;
			    a=0;
					FH_BZ4=1;
			}
		if(FH_BZ4==-1){
					if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
								ans=ans*10;
								ans=ans+number[i];
							if(ans>=-10&&ans<=10) 
								LW_ans2=-ans;
					}		  	
					if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
					{
						piont=i;
						a=1;
					}
					if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
						ans+=number[i]*pow(10,piont-i);
							if(ans>=-10&&ans<=10) 
							LW_ans2=-ans;
					}
					 
					if((key>0&&key<=11)||key==12){
						 i++;   
					}
		
		}else{
						if((number[i]>=0&&number[i]<10&&a==0)||number[i]==11){//标志位a=0，将输入的数字传递后显示在屏幕上
									ans=ans*10;
									ans=ans+number[i];
								if(ans>=-10&&ans<=10) 
									LW_ans2=ans;
						}		  	
						if(number[i]==10)//标志位=1，以下为计算ans值后经通信发送至转台
						{
							piont=i;
							a=1;
						}
						if((piont<i)&&a==1&&number[i]>=0&&number[i]<10){
							ans+=number[i]*pow(10,piont-i);
								if(ans>=-10&&ans<=10) 
								LW_ans2=ans;
						}
						 
						if((key>0&&key<=11)||key==12){
							 i++;   
						}
		}
//		  if(number[0]==10&&number[1]==10){
//    number[0]=-1; number[1]=-1;
//     LW_ans2=0;//
//     ans=0;
//      i=0;
//	   a=0;
//    FH_BZ4=-1;
//  }
//			
				 if(number[0]==13){//检测到负号按下
						  number[0]=-1; 
						  LW_ans2=0;//
						  ans=0;
							i=0;
						  a=0;
						 FH_BZ4=-1;
			}
		}
	LW2_blink=0;
		return 	LW_ans2;
}



void send_data()//主要进行模式的判断和通过进行按键的按下来该表模式
{
    	u8 check=0x00;//校验位
	    int key_table1;
	    
			key_table1 = KeyScan_3X7();
				if(key_table1>0&&key_table1<22&&Beep==0){
						BEEP_BZ=1;
				}
				if(key_table1==21){
						mode[0]=key_table1;
						BZ=1;
				}
				if(key_table1>15&&key_table1<21&&BZ==1){
						mode[1]=0;
					mode[0]=0;
						BZ=2;
				}
				if(key_table1>0&&key_table1<5&&BZ==1){
						mode[1]=key_table1;
						BZ=2;
				}
//////////////////////////////////////////////
//以下部分是给通过判断功能键的按下，用来发送表1的数据
//////////////////////////////////////////				
//	if(rdata1[3]!=0XB2){
					switch(state)
						{                                                                              
            case 22: 	tdata[3]=0XA6;		tdata[2]=0X01;	 	break;							                                                                                                       			                                                                                                        
            case 23: 	tdata[3]=0XA7;		tdata[2]=0X01;		break;								                                                   						
						case 24: 	tdata[3]=0XA8;		tdata[2]=0X01; 		break;
						case 25: 	tdata[3]=0XA9;		tdata[2]=0X01; 		break;								
						}
						state=0;
						switch(key_table1)
						{
						case 16: 	tdata[3]=0XA3;	  tdata[2]=0X01;  		break;						//零位修正			
						case 18: 	tdata[3]=0XA5;		tdata[2]=0X01;			break;						//	维修
						case 19: 	tdata[3]=0XA2;  	tdata[2]=0X01;			break;					//	锁定
						case 17: 	tdata[3]=0XA1;	  tdata[2]=0X01;				break;					//	归零
						case 20: 	tdata[3]=0XA4;		tdata[2]=0X01; 		break;							//	S20预留 
						}
//					}else{
//									while(1){
//									u8 key_SD=KeyScan_3X7();
//									if(key_SD==19){rdata1[3]=0XB9;break;}
//									}
//					}
	
////////////////////////////////////////////////////////////////////////////以下为表2，在俯仰稳定或调转模式下进行方位角的设置

if(rdata1[3]==0XB7){
	
			if(key_table1==15){
					tdata[2]=0X02;
					FW_angle=Read_Number();
							ans=0;	
					coding1(FW_angle,3);
			}
}	
///////////////////////////////////////////////////////////////////以下为表3的报文判断，判断是否是方位稳定或调转模式，进行俯仰角的设置
			

if(rdata1[3]==0XB6){
	
	if(key_table1==15){		
					tdata[2]=0X03;
					FY_angle=Read_Number1();
					ans=0;
					coding1(FY_angle,3);
	}		
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
if(rdata1[3]==0XB4){
	
	if(key_table1==15){
	    	FW_blink=1;//按下一次（方位/俯仰）键方位框就开始闪烁
		       Open_Time();
					Reuse[0]=key_table1;
				while(1){
					Reuse[1]=KeyScan_3X7();
					if(aotuexit==1){ aotuexit=0; break; }
					if(Reuse[1]!=0) {FW_blink=0;break;}
				}
					
				if(Reuse[0]==15&&Reuse[1]!=15){//复用数组第一第二位赋值
					tdata[2]=0X02;
							FW_angle=Read_Number();
									ans=0;	
							coding1(FW_angle,3);
					}
					if(Reuse[0]==15&&Reuse[1]==15){
						tdata[2]=0X03;
								FY_angle=Read_Number1();
								ans=0;
								coding1(FY_angle,3);
					}
	}		
}





/////////////////////////////////////////////////////////////	表4，在零位修正模式下，设置调零

if(rdata1[3]==0XB3)
{
	if(key_table1==16){
	 
		tdata[2]=0X04;
		LW_angle1=Read_Number2();
	    	ans=0;
		LW_angle2=Read_Number3();
	     	ans=0;
		coding2(LW_angle1,3);
		coding2(LW_angle2,5);	
	}
	 
}
		for(int i=0;i<7;i++)
			{
					check=check^tdata[i];			
			
			}
			tdata[7]=check;

}

void uart1_send()
	{
	if(tdata[2]!=0)  HAL_UART_Transmit(&UART1_Handler,tdata,sizeof(tdata),100);//串口1发送报文
						for(int a=2;a<7;a++)
						tdata[a]=0;	
		
	}





