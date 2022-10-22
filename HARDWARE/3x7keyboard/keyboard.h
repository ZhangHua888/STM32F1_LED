#include "sys.h"

/******** 行线状态宏定义 ************/
#define		RAW1_STATE		PBin(12)
#define		RAW2_STATE		PBin(13)
#define		RAW3_STATE		PBin(14)
/******** 列线状态宏定义 ************/
#define		COL1_STATE		PBin(5)
#define		COL2_STATE  	PBin(6)
#define		COL3_STATE		PBin(7)
#define		COL4_STATE		PBin(8)
#define		COL5_STATE		PBin(9)
#define		COL6_STATE		PBin(10)
#define		COL7_STATE		PBin(11)
/********************* 键值分布************************* 
                 S1 	  S2	   S3		S4
                 S5		  S6	   S7	    S8
                 S9		  S10      S11		S12
                 S13	  S14	   S15	    S16   
*******************************************************
********************矩阵按键连线***********************
                 PC6      PC7      PC8      PC9
        PB6-------|--------|--------|--------|----
        PB7 ------|--------|--------|--------|----
        PB8-------|--------|--------|--------|----
        PB9 ------|--------|--------|--------|----
*******************************************************/
/* -- 开关状态宏定义 -- */
#define 	KEY3X7_OPEN			1
#define 	KEY3X7_CLOSE		0
/* -----------------所有相关函数 --------------------- */
void KEY3X7_ON(void);   // 开启矩阵键盘
void KEY3X7_OFF(void);	// 关闭矩阵键盘
void KEY3X7_RAW_SET(void);  				// 行线置位
void KEY3X7_RAW_RESET(void); 				// 行线复位
void KEY3X7_RAW_READ_Init(void);  	        // 行线设为输入模式
void KEY3X7_COL_SET(void);					// 列线置位
void KEY3X7_COL_RESET(void);				// 列线复位
void KEY3X7_COL_READ_Init(void);		    // 列线设为输入模式
u8 KeyScan_3X7(void); 	// 键盘扫描函数



