#include "key.h"
#include "delay.h"
u8 KEY3X7_ON_OFF;
u8 KeyScan_3X7(void)
{
	if(KEY3X7_ON_OFF){
	
	u8 keyValue=0;
    static u8 key_up=1;//按键松开标志
	KEY3X7_RAW_SET();                                                      // 行线置位 1
	KEY3X7_COL_RESET();                                                    // 列线复位 0
	KEY3X7_RAW_READ_Init(); 
  delay_ms(5);		// 行线设为输入模式
    if(key_up&&(RAW1_STATE==0||RAW2_STATE==0||RAW3_STATE==0))
	{
      
        key_up=0;                                        //屏蔽此行则表示支持 连续按
		if	   (RAW1_STATE==0)	keyValue=1;		                          // raw1
		else if(RAW2_STATE==0)	keyValue=2;			                       // raw2
		else if(RAW3_STATE==0)	keyValue=3;			                       // raw3
		else return 0;	
		
		KEY3X7_RAW_RESET();                                                // 行线复位 0
		KEY3X7_COL_SET();                                                  // 列线置位 1
		KEY3X7_COL_READ_Init();                                            // 列线设为输入模式
		if(COL1_STATE==0||COL2_STATE==0||COL3_STATE==0||COL4_STATE==0||COL5_STATE==0||COL6_STATE==0||COL7_STATE==0)
		{
			if	   (COL1_STATE==0)	{keyValue=(keyValue-1)*7+1;       return keyValue;}   // col1
			else if(COL2_STATE==0)	{keyValue=(keyValue-1)*7+2;       return keyValue;}   // col2
			else if(COL3_STATE==0)	{keyValue=(keyValue-1)*7+3;       return keyValue;}   // col3
			else if(COL4_STATE==0)	{keyValue=(keyValue-1)*7+4;      return keyValue;}	  // col4
			else if(COL5_STATE==0)	{keyValue=(keyValue-1)*7+5;      return keyValue;}	  // col4
			else if(COL6_STATE==0)	{keyValue=(keyValue-1)*7+6;      return keyValue;}	  // col4
			else if(COL7_STATE==0)	{keyValue=(keyValue-1)*7+7;      return keyValue;}	  // col4
			else return 0;
		}		
		return 0;
	}
    else if(RAW1_STATE==1&&RAW2_STATE==1&&RAW3_STATE==1) key_up=1;
	return 0;	//无按键按下	
	}
	
}
// 打开矩阵键盘


u8 Judge(int key_value)//小数点和-号判断
{ int i;
	if(key_value==10)
	{
		while(1){
			delay_ms(150);
		if(RAW2_STATE==0&&COL3_STATE==0){
					return -1;
			break;
		}
		i++;
		if(i>5){				
			return 10;
			break;}
	}
		}
	}
void KEY3X7_ON(void)
{
	KEY3X7_ON_OFF=KEY3X7_OPEN;
}
// 关闭矩阵键盘
void KEY3X7_OFF(void)
{
	KEY3X7_ON_OFF=KEY3X7_CLOSE;
}



/******************************  行线置位 1 *************************************/
void KEY3X7_RAW_SET(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
		// PB6,PB7,PB8,PB9设为输出模式
	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
		GPIO_InitStruct.Pin =GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14,GPIO_PIN_SET);	
}


/******************************  行线复位 0 *************************************/
void KEY3X7_RAW_RESET(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;    
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
		// PB6,PB7,PB8,PB9设为输出模式
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	  GPIO_InitStruct.Pin =GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
		// 4根行线置0
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14,GPIO_PIN_RESET);	
}
/******************************  行线设为输入模式  ******************************/
void KEY3X7_RAW_READ_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;   
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
		// PB6,PB7,PB8,PB9设为输入模式
	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}
/******************************  列线置位  **************************************/
void KEY3X7_COL_SET(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOC时钟
		// PC6,PC7,PC8,PC9设为输出模式
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
		// 4根列线置1
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11,GPIO_PIN_SET);	
}
/******************************  列线复位  **************************************/
void KEY3X7_COL_RESET(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;    
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOC时钟
		// PC6,PC7,PC8,PC9设为输出模式
	GPIO_InitStruct.Pin =GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
		// 4根列线置0
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11,GPIO_PIN_RESET);		
}
/******************************  列线设为输入模式  ******************************/
void KEY3X7_COL_READ_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;   
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOC时钟
		// PC6,PC7,PC8,PC9设为输入模式
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
}
