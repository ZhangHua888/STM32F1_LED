//#include "key.h"
//#include "delay.h"
////////////////////////////////////////////////////////////////////////////////////	 
////������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
////ALIENTEK STM32F429������
////KEY��������	   
////����ԭ��@ALIENTEK
////������̳:www.openedv.com
////��������:2016/1/5
////�汾��V1.0
////��Ȩ���У�����ؾ���
////Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
////All rights reserved									  
////////////////////////////////////////////////////////////////////////////////////

////������ʼ������
//void KEY_Init(void)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//    
//    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
//    __HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOCʱ��
//    __HAL_RCC_GPIOH_CLK_ENABLE();           //����GPIOHʱ��
//    
//    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
//    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//    
//    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
//    GPIO_Initure.Pull=GPIO_PULLUP;          //����
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
//    
//    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PH2,3
//    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
//}

////������������
////���ذ���ֵ
////mode:0,��֧��������;1,֧��������;
////0��û���κΰ�������
////1��WKUP���� WK_UP
////ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
//u8 KEY_Scan(u8 mode)
//{
//    static u8 key_up=1;     //�����ɿ���־
//    if(mode==1)key_up=1;    //֧������
//    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
//    {
//        delay_ms(10);
//        key_up=0;
//        if(KEY0==0)       return 1;
//        else if(KEY1==0)  return 2;
//        else if(KEY2==0)  return 3;
//        else if(WK_UP==1) return 4;          
//    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1;
//    return 0;   //�ް�������
//}