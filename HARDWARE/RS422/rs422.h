#ifndef _RS422_H
#define _RS422_H
#include "sys.h"
#include "stdio.h"	

u8 KeyScan_3X7(void);
void send_data();
void uart1_init(void);
void uart2_init(void);
void coding1(float num,char num1);//这是四个字节装一个浮点数的函数，第一个参数是要抓换的数字，第二个参数是首个数组的标号
void coding2(float num,char num1);//两个字节装一个浮点数，第一个参数是要转换的数字，第二个参数是首个数组的标号
float uncoding1(char num1);//正好coding1相反，返回值是浮点数，入口参数是数组标号
float uncoding2(char num1);//正好coding2相反，返回值是浮点数，入口参数是数组标号
float uncoding3(char num1);
void uart1_send();
#endif

