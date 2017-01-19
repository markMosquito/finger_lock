/*
********************************************************************************
	*@  file: 007_elmo.h
	*@  author: 007
	*@  data: 3/20/2015
	*@  version: v2.0
	*@  brief: 参数修改，不要乱动！！！
	*...............................................................................
	*@
	*@ Notes: (1) 修改ElMO_NUM的值确定Elmo个数,底盘为4个Elmo.
	*@            如需其他Elmo,在加号后面加上.
	*@        (2) 修改电机参数,额定电流,峰值电流,电机最大转速.
	*@            这些是底盘的电机的参数.如需使用其他Elmo,可直接调用函数.
	*@            尽量将除底盘外电机设置为大于5的ID号.
	*@        (3) ***********************************************************
	*@            *  MAXON电机型号       额定转速(RPM)  额定电流(A) 功率(W) *
	*@            *  EC-4pole-30-305013  16200          9.21        200     *
	*@            *  RE-40-148867        6930           5.77        150     *
	*@            *  RE-30-310007        8050           3.44        60      *
	*@            *  RE-25-339152        9620           1.42        20      *
	*@            ***********************************************************
	*@
	               
********************************************************************************
*/

#ifndef __007_ELMO_H
#define __007_ELMO_H
#include "stm32f4xx.h"
#include <math.h>
#include <string.h>
#include "MotorConfig.h"

/*******************************************************************************
* 宏定义 
*******************************************************************************/
/* 0:在速度控制模式下启动速度模式,此时加速度失效 */ 
/* 1:在位置控制模式下启动速度模式,此时加速度生效 */ 
#define  JV_IN_PCM          0


/* 循环队列参数 */
#define ELMO_NUM            (8)                             // Elmo个数,必须严格按照挂载个数的ELMO配置，不得多配置！！！
#define CAN_BUF_NUM         1000                            //缓冲指令条数
#define CAN_ID_DELAY        0x128                           //延时指令

/* ELMO相关参数 */
#define RATE_CURRENT        9.210                           // 额定电流(A)
#define PEAK_CURRENT        (9.210*2)                       // 峰值电流(A)
#define MAX_VOLOCITY        16200                           // 电机最大转速(rpm)

/* 保护机制相关参数 */
#define RATE_CUR            RATE_CURRENT                    // 额定电流(A)          CL[1]
#define MAX_CURRENT         PEAK_CURRENT                    // 峰值电流(A)          PL[1]
#define MAX_PM_SPEED        (MAX_VOLOCITY*2000/60)          // 最大平滑速度(cnt/s)  VH[2]
#define MIN_PM_SPEED        (u32)(-MAX_PM_SPEED)            // 最小平滑速度(cnt/s)  VL[2]
#define MAX_FB_SPEED        ((MAX_VOLOCITY+1000)*2000/60)   // 最大反馈速度(cnt/s)  HL[2]
#define MIN_FB_SPEED        (u32)(-MAX_FB_SPEED)            // 最小反馈速度(cnt/s)  LL[2]

/* 平滑运动相关参数 */ 
#define PM_ACC              3500000                         // 平滑加速度(cnt/s^2)  AC
#define PM_DEC              3500000                         // 平滑减速度(cnt/s^2)  DC
#define QUICKSTOP_DEC       1000000000                      // 急停减速度(cnt/s^2)  SD
#define POSITION_LIMIT_MAX  1000000000                      // 最大位置极限         VH[3] HL[3]
#define POSITION_LIMIT_MIN  (u32)-1000000000                // 最小位置极限         VL[3] LL[3]

/* CANopen COB identifiers */
#define COBID_NMT_SERVICE   0x000
#define COBID_SYNC          0x080
#define COBID_EMERGENCY     0x080
#define COBID_TIME_STAMP    0x100
#define COBID_TPDO1         0x180
#define COBID_RPDO1         0x200
#define COBID_TPDO2         0x280
#define COBID_RPDO2         0x300
#define COBID_TPDO3         0x380
#define COBID_RPDO3         0x400
#define COBID_TPDO4         0x480
#define COBID_RPDO4         0x500
#define COBID_TSDO          0x580
#define COBID_RSDO          0x600
#define COBID_HEARTBEAT     0x700

/* NMT Command Specifier */
#define NMT_ENTER_OPERATIONAL      0x01
#define NMT_ENTER_STOPPED          0x02
#define NMT_ENTER_PRE_OPERATIONAL  0x80
#define NMT_RESET_NODE             0x81
#define NMT_RESET_COMMUNICATION    0x82

/* Binary Interpreter Command */
#define UM_IDLE             0x00
#define UM_TCM              0x01                      // Torque control mode,力矩控制模式
#define UM_PCM              0x05                      // Position control mode，位置控制模式
#define UM_UNC              0x06                      // 不能确定模式，适用于全部电机调用

#if JV_IN_PCM
   #define UM_SCM           0x05                      // Position control mode，
#else
   #define UM_SCM           0x02                      // Speed control mode
#endif

#define TYPE_INTEGER      0
#define TYPE_FLOAT        1

#define MO_OFF            0
#define MO_ON             1

#define POS_REL           0
#define POS_ABS           1



/*******************************************************************************
* 结构体
*******************************************************************************/
/* CAN循环队列元素 */ 
typedef struct __CANDATA 
{
   u16 COBID;               // CANopen COB identifier + NodeID
   u8  DLC;                 // Data Length Code
   u8  DATA[8];             // Data
} CANDATA;

/* CAN循环队列结构体 */
typedef struct __CANQUEUE
{
   u16 Front;        
   u16 Rear;
   CANDATA CANBUF[CAN_BUF_NUM];
} CANQUEUE;

/* Elmo结构体,记录节点ID,状态和控制参数 */
typedef struct __Elmo
{
    u8 NodeID;         // elmo结点号
    u8 CurOPMode;      // 当前运行模式
}Elmo;

typedef struct 
{
    Elmo ElmoBase;
    u8 Start;
    u8 End;
}ElmoGroupTypedef;

/*数据转化共用体*/
typedef union 
{
    uint32_t u32_form;
    int32_t  s32_form;
    uint8_t  u8_form[4];
    int8_t   s8_form[4];
    float    float_form;
}DataConvert;



/*******************************************************************************
* 函数声明
*******************************************************************************/
/* ELMO初始化函数,对外调用 */
extern u32 Elmo_Init( CAN_TypeDef* CANx, uint8_t PPr, uint8_t SPr);
extern void Elmo_Reinit(void);

/* ELMO控制函数，对外调用 */
extern u8 Elmo_PTM(u8 elmoID, float torque);
extern u8 Elmo_PVM(u8 elmoID, s32 speed);  
extern u8 Elmo_PPM(u8 elmoID, u32 speed, s32 position, u8 PPMmode);		//		POS_ABS  POS_REL 
extern u8 Elmo_Close(u8 elmoID);
extern u8 Elmo_Stop(u8 elmoID);
extern u8 Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec);
extern uint32_t Elmo_Read_POS(u8 elmoID, int32_t *pEncData);
extern void Elmo_Set_POS(u8 elmoID,s32 POS);
extern void Elmo_Read_ACT_CUR(u8 elmoID);

/* CANOpen的实现函数,不对外调用 */
static void NMTCmd(Elmo *elmo, u8 MNTCmd);
static void RSDO(Elmo *elmo, u16 Index, u8 SubIndex, u32 Data);
static void RPDO2_Cmd_data(Elmo *elmo, u8 *Cmd, u8 Index, u8 Type, u32 Data);
static void RPDO2_Cmd_string(Elmo *elmo, u8 *Cmd);

/* 硬件初始化函数,不对外调用 */
static void CAN_init(CAN_TypeDef* CANx);
static int Self_test(void);
static void Variate_init(void);

/* 数据发送、转换与延时函数,不对外调用 */
static void Elmo_SendCmd(void);
//static void Elmo_software_delay_ms(unsigned int t);
static u32 f2h(float x);

/* 发送心跳给elmo */
void SendHeart2Elmo(void);

// GroupControl
void Chassis_Elmo_PVM(s32 speed1, s32 speed2, s32 speed3, s32 speed4);
void Slideway_Elmo_PVM(s32 speed1, s32 speed2);
void Chassis_Elmo_Stop(void);
void Slideway_Elmo_Stop(void);
void Chassis_Elmo_Close(void);
void Slideway_Elmo_Close(void);
#endif


