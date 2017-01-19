/*
**********************************************************************************************************
    *@  file:007_elmo.c
    *@  author: 007
    *@  date: 3/20/2015
    *@  version: v2.0
    *@  brief: 增加底盘试用的快速启动和快速刹车函数
    *@         增加了启动预处理函数
    *@         更新了note1中的函数注释
    *@         更新了note2中的注意事项
    *...............................................................................
    *@ Notes1: (1)初始化函数：       
    *@            name      : Elmo_Init(CAN_TypeDef* CANx, uint8_t PPr, uint8_t SPr)
    *@            function  : 调用初始化函数即可使用CAN通信模块，入参为CAN1或CAN2，用来选择
    *@			                  CAN口，确保物理通信层正确，通信速度为1Mbps，同时选择TIM7的中断
    *@	     		              优先级，为底层报文发送提供时序	
    *@            input     : CANx      CAN1 or CAN2
    *@						            PPr       TIM7的抢占优先级
    *@                        SPr       TIM7的从优先级
    *@            output    : 0	          初始化成功
    *@                        0x80000000  主控没有连上CAN总线
    *@ 							          其他         对应的ELMO的ID位没有置1
    *@	
    *@         (2)力矩模式函数：        		
    *@            name      : Elmo_PTM(u8 elmoID, float torque);
    *@            function  : 单轴力矩控制函数，维持电机力矩恒定,支持模式切换  
    *@            input     : elmoID    取elmo节点ID     
    *@			    			        torque     目标转矩(A)
    *@            output    : 0         函数调用成功
    *@                        1         函数调用失败
    *@ 	
    *@         (3)速度模式函数：  
    *@            name      : Elmo_PVM(u8 elmoID, s32 speed);      
    *@            function  : 单轴速度控制函数,支持模式切换 ，支持连续调用，elmo 
    *@                        将以最大加速度或减速度使电 机转速达到设定值
    *@            input     : elmoID    取elmo节点ID      
    *@                        speed     目标速度(cnt/s)
    *@            output    : 0         函数调用成功
    *@                        1         函数调用失败
    *@
    *@         (4)位置模式函数：   
    *@            name      : Elmo_PPM(u8 elmoID, u32 speed, s32 position, u8 PPMmode);
    *@            function  : 单轴速度控制函数，支持模式切换，支持连续调用，对于POS_ABS模式
    *@ 						            终是指最终电机的位置相对于上电后的电机的位置，也就是电机 中，目    
    *@ 						            标位置始的绝对位置。对于POS_REL模式中，最终电机的位置是现在elmo
    *@ 				                绝对位置寄存器中的绝对位置加上目标位置，绝对位置寄存器在CAN_init 
    *@                        后默认为0
    *@						  例子1：
    *@						  Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						  while(position != 500000);
    *@					   	  Elmo_PPM(1, 50000, 250000, POS_REL);		
    *@                             电机位置在750000处		
    *@						            例子2：
    *@						            Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						            while(position != 500000);
    *@						            Elmo_PPM(1, 50000, 250000, POS_ABS);			
    *@						            电机位置在250000处		
    *@						            例子3：
    *@						            Elmo_PPM(1, 50000, 500000, POS_ABS);
    *@						            while(position <= 250000);
    *@						            Elmo_PPM(1, 50000, 250000, POS_REL);			
    *@					              电机位置在750000处		
    *@            input     : elmoID      取elmo节点ID,请勿在底盘控制中调用1--4        
    *@ 						            speed       速度(cnt/s)
    *@ 					              position    目标位置(cnt)
    *@ 						            PPMmode     运行模式
    *@ 						            POS_ABS    // PPM运行方式:绝对位置
    *@ 		   			            POS_REL    // PPM运行方式:相对位置
    *@  			    output    : 0         函数调用成功
    *@                        1         函数调用失败
    *@
    *@         (5)电机释放函数：   
    *@            name      : Elmo_Close(u8 elmoID);
    *@            function  : 驱动输出关闭，电机靠惯性继续行驶,支持连续调用，支持联合调用（能
    *@						            够在各种elmo函数前后调用），不会影响其他函数的功能实现和elmo稳
    *@						            定性														
    *@            input     : elmoID      取elmo节点ID     
    *@  			    output    : 0         函数调用成功
    *@                        1         函数调用失败
    *@         
    *@         (6)电机抱死函数：
    *@            name      : Elmo_Stop(u8 elmoID);
    *@            function  : 电机抱死，维持电机当前位置不变，支持连续调用，支持联合调用（能
    *@ 						            够在各种elmo函数前后调用），不会影响其他函数的功能实现和elmo稳
    *@ 						            定性	
    *@            input     : elmoID      取elmo节点ID     
    *@  			    output    : 0         函数调用成功
    *@                        1         函数调用失败
    *@	
    *@         (7)加速度设置函数：	
    *@  			    name      : Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
    *@            function  : 设置速度模式和位置模式的电机加减速，如果没有在函数中调用，初始化
    *@	                      后加速度默认为1000000000，减速度默认为1000000000
    *@			      input     : elmoID      取elmo的节点ID
    *@                        acc         加速度,加速度最大不能超过1000000000,同时应考虑电机性能
    *@                        dec         减速度,减速度最大不能超过1000000000,同时应考虑电机性能
    *@  			    output    : 0         函数调用成功
    *@         (8)位置读取函数：	
    *@  			    name      : Elmo_Read_POS(u8 elmoID)
    *@            function  : 读取电机编码器的数据，数据在中断中读取，存储在全局变量Encoder_Data中
    *@			      input     : elmoID      取elmo的节点ID
    *@  			    output    : NONE
    *@         (9)位置设置函数：	
    *@  			    name      : Elmo_Set_POS(u8 elmoID,s32 POS)
    *@            function  : 设置电机编码器的数据
    *@			      input     : elmoID      取elmo的节点ID
    *@                        POS         要给elmo设置的地址
    *@  			    output    : NONE
    *@         (10)电流读取函数：	
    *@  			     name      : Elmo_Read_ACT_CUR(u8 elmoID)
    *@             function  : 读取电机有功电流的数据，数据在中断中读取，存储在全局变量IQ中
    *@			       input     : elmoID      取elmo的节点ID
    *@  			     output    : NONE
    *...............................................................................
    *@ Notes2: (1)、CAN口引脚 
    *@			     CAN1 RX -> PD0     CAN1 TX -> PD1
    *@               CAN2 RX -> PB5     CAN2 TX -> PB6
    *@         (2)、修改宏定义CAN_BUF_NUM的值修改缓冲数目
    *@         (3)、修改TIM7_Configuration()中TIM7更新中断优先级
    *@         (4)、注意CAN时钟为42MHz,TIM7时钟为42MHz
    *@ 	       (5)、注意写优先级分组函数
    *@         (6)、注意更改中断中的电机ID
    *@         (7)、在Elmo_Read_POS和Elmo_Read_ACT_CUR函数调用之前要延时1ms，可以防止打断当前电机的运动状态
    *@         (8)、在Four_Elmo_PVM函数调用之前要调用Elmo_Pre_PVM做预处理
    *...............................................................................
    *@ Notes3: (1)、没有写节点ID和组ID的分配函数
    *@	       (2)、没有加反馈报文机制 
    *@		   (3)、没有加平滑因子设置函数	    
    *@		   (4)、Elmo_Stop()对Elmo_PTM()的响应时间太长，需要优化			
    *@         (5)、没有加节点保护函数和总线心跳报文				 
    *@    	   (6)、Elmo_Delay100us()可以改进，提高效率    
    *@         (7)、GroupID暂时不能用了	
**********************************************************************************************************
*/
#include "007_elmo.h"
#include "Rsing_Can.h"
#include "FreeRTOS.h"
#include "task.h"

static Elmo elmo[ELMO_NUM + 1];
static Elmo chassisGroup, slidewayGroup;
const T_CanFrame MainHeartFrame = {(uint32_t)(0x700+9) << 21,0,{0,0,0,0,0,0,0,0}};  //主控发送心跳供Elmo检测

u32 ElmoHeartbeat = 0;   //用来记录ELMO心跳
#define DEF_ElmoHeartbeat_VALUE     (0xfF)  //只有在elmo收到该值表示的各elmo心跳后才会送出心跳信号量

extern xSemaphoreHandle ElmoSem;
extern xSemaphoreHandle PosRacketMainSem;      //收到Elmo编码器的值
extern xSemaphoreHandle PosRacketServeSem;    //收到Elmo编码器的值

#define Elmo_Delay100us(time_100us)     vTaskDelay(time_100us)

/**
  ******************************************************************************
  * @name     : Elmo_Init
  * @function : 
  * @input    : 
  * @output   : 
  ******************************************************************************
  */
u32 Elmo_Init(CAN_TypeDef* CANx, uint8_t PPr, uint8_t SPr)
{
    u8 i = 0;
    int temp = 0;
    /* 硬件及全局变量初始化 */ 
    CAN_Config(CANx);

    /*定义变量初始化*/
    Variate_init();
    
    /* 关闭电机 */
    RSDO(&elmo[0], 0x6040, 0x00, 0x00);
    Elmo_Delay100us(200);
    
    /* 对elmo分配节点ID */
    for(i=0; i <= ELMO_NUM; i++)
    {
        elmo[i].NodeID = i;			
    }

    /* 对elmo分配组ID */
    chassisGroup.NodeID = 64;
    slidewayGroup.NodeID = 65;

    /* 对Elmo5~8 & 1~2设置GroupID */
    for(i = 5; i <= 8; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, chassisGroup.NodeID);
        Elmo_Delay100us(10);
    }
    for(i = 1; i <= 2; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, slidewayGroup.NodeID);
        Elmo_Delay100us(10);
    }

    /* 对全体节点进行通信复位 */
    Elmo_Delay100us(500);
    NMTCmd(&elmo[0], NMT_RESET_COMMUNICATION);
    Elmo_Delay100us(500);

    /* Elmo心跳检测Start------------------------------------------------------*/
    RSDO(&elmo[0], 0x1017, 0, (100&0xffff));//设置心跳为100ms
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x6007, 0, 1);//电机故障，MO=0
    Elmo_Delay100us(100);

    RSDO(&elmo[0], 0x1029, 1, 2);//通信错误行为：停止
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x1016, 1, (0x09<<16) | (150&0xffff));//消费者心跳时间间隔 ID：0x09 T：150ms
    Elmo_Delay100us(2000);

    /* Elmo心跳检测End--------------------------------------------------------*/
    
    //Elmo_software_delay_ms(100);
    //	/* 等待Elmo启动完毕,即接收到Boot up报文 */
    //	Elmo_Delay100us(50);

    /* CANOpen通信参数初始化 */
    /* RPDO1->0x6040,指令字,2字节,异步传输 */
    // 驱动器默认映射,不需要修改 //

    /* RPDO2->0x2012,二进制编译输入,4字节,异步传输 */
    // 驱动器默认映射,不需要修改 //

    /* 禁用TPDO,Debug时开启,电流环时最好关闭 */
    RSDO(&elmo[0], 0x1A00, 0x00, 0);
    Elmo_Delay100us(150);
    RSDO(&elmo[0], 0x1A01, 0x00, 0);
    Elmo_Delay100us(150);
    
    /* 进入NMT操作状态 */
    Elmo_Delay100us(40);
    NMTCmd(&elmo[0], NMT_ENTER_OPERATIONAL);
    Elmo_Delay100us(80);

    /*此后心跳数据会变为0x05，网络状态变为运行态*/

    /* 关闭驱动 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(80);

    /* 初始化加速度 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(40);	
    RPDO2_Cmd_data(&elmo[0], (u8 *)"AC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);
    RPDO2_Cmd_data(&elmo[0], (u8 *)"DC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);

    /* Enter in SCM */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
    Elmo_Delay100us(40);

    for(i=0;i<=ELMO_NUM;i++)
    {
        elmo[i].CurOPMode = UM_SCM;			
    }

    /* 使能驱动 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(250);
    Elmo_Delay100us(250);
    Elmo_Delay100us(50);
    return temp;
}

void Elmo_Reinit(void)
{
    uint32_t i = 0;
    ElmoHeartbeat = 0;
    
    RSDO(&elmo[0], 0x6040, 0x00, 0x80);  //清除错误标志位
    Elmo_Delay100us(40);
    
    /* 关闭电机 */
    RSDO(&elmo[0], 0x6040, 0x00, 0x00);
    Elmo_Delay100us(200);
    
    /* 对Elmo5~8 & 1~2设置GroupID */
    for(i = 5; i <= 8; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, chassisGroup.NodeID);
        Elmo_Delay100us(10);
    }
    for(i = 1; i <= 2; i++)
    {
        RSDO(&elmo[i], 0x2040, 0x00, slidewayGroup.NodeID);
        Elmo_Delay100us(10);
    }
    
    /* 对全体节点进行通信复位 */
    Elmo_Delay100us(500);
    NMTCmd(&elmo[0], NMT_RESET_COMMUNICATION);
    Elmo_Delay100us(1000);

    /* Elmo心跳检测Start------------------------------------------------------*/
    RSDO(&elmo[0], 0x1017, 0, (100&0xffff));//设置心跳为100ms
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x6007, 0, 1);//电机故障，MO=0
    Elmo_Delay100us(100);

    RSDO(&elmo[0], 0x1029, 1, 2);//通信错误行为：停止
    Elmo_Delay100us(100);
    
    RSDO(&elmo[0], 0x1016, 1, (0x09<<16) | (150&0xffff));//消费者心跳时间间隔 ID：0x09 T：150ms
    Elmo_Delay100us(2500);
    /* Elmo心跳检测End--------------------------------------------------------*/
    
    
    /* 禁用TPDO,Debug时开启,电流环时最好关闭 */
    RSDO(&elmo[0], 0x1A00, 0x00, 0);
    Elmo_Delay100us(150);
    RSDO(&elmo[0], 0x1A01, 0x00, 0);
    Elmo_Delay100us(150);
    
    /* 进入NMT操作状态 */
    Elmo_Delay100us(40);
    NMTCmd(&elmo[0], NMT_ENTER_OPERATIONAL);
    Elmo_Delay100us(80);

    /*此后心跳数据会变为0x05，网络状态变为运行态*/

    /* 关闭驱动 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(80);

    /* 初始化加速度 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(40);	
    RPDO2_Cmd_data(&elmo[0], (u8 *)"AC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);
    RPDO2_Cmd_data(&elmo[0], (u8 *)"DC", 0, TYPE_INTEGER, 1000000);
    Elmo_Delay100us(40);

    /* Enter in SCM */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
    Elmo_Delay100us(40);

    for(i=0;i<=ELMO_NUM;i++)
    {
        elmo[i].CurOPMode = UM_SCM;			
    }

    /* 使能驱动 */
    RPDO2_Cmd_data(&elmo[0], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(550);
}

/**
  ******************************************************************************
  * @name     : Elmo_PTM
  * @function : 单轴力矩模式函数
  * @input    : elmoID  取elmo节点ID
  *             torque  目标转矩(A)
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_PTM(u8 elmoID, float torque)
{
    u8 i = 0;
    
    /* 如果使用广播控制, 当前模式不为电流模式,切换为电流模式*/
    if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_TCM)  
    {
        if( elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }		
        
        /* 将所有状态变量赋值为TCM */		
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_TCM;			
        }
        
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_TCM);
        Elmo_Delay100us(10);
        
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }  
    /* 使用单个ELMO控制,当前模式不为电流模式,切换为电流模式 */
    else if( elmo[elmoID].CurOPMode != UM_TCM)         
    {
        if( elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        elmo[elmoID].CurOPMode = UM_TCM;
        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_TCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        
        /* 如果ELMO只挂载了1个，不管怎么改其的模式，elmo[0]的模式和它一样 */		
        if(ELMO_NUM == 1)
        {
            elmo[0].CurOPMode = UM_TCM;
        }	
        /* 用来判断是否所有电机模式相同，从而确定elmo[0]的模式 */
        else
        {
            for(i = 1; i < ELMO_NUM; i++)  
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_TCM;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }			
        }
    }
        
    /* 设置目标转矩并运行电流模式 */
    RPDO2_Cmd_data( &elmo[elmoID], (u8 *)"TC", 0, TYPE_FLOAT, f2h(torque));
    Elmo_Delay100us(10);
    return 0;
}

/**
  ******************************************************************************
  * @name     : Elmo_PVM
  * @function : 单轴速度模式函数
  * @input    : elmoID      取elmo节点ID
  *             speed       速度(cnt/s)
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_PVM(u8 elmoID, s32 speed)
{
    u8 i = 0;    
    if(elmo[elmoID].CurOPMode != UM_SCM)        /* 当前模式不为速度模式,切换为速度模式 */
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        
        
        if(elmoID == 0) //如果使用广播控制
        {
            for(i = 0; i <= ELMO_NUM; i++)
            {
                elmo[i].CurOPMode = UM_SCM;
            }
        }
        else            //非广播控制
        {
            elmo[elmoID].CurOPMode = UM_SCM;
            elmo[0].CurOPMode = UM_SCM;
            for(i = 1; i <= ELMO_NUM; i++) 
            {
                if( elmo[i].CurOPMode != UM_SCM )
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }
        }
    }
    
    /* 设置目标速度 */
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, speed);
    Elmo_Delay100us(10);

    /* 运行速度模式 */
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
    Elmo_Delay100us(20);
    
    return 0;
}


/**
  ******************************************************************************
  * @name     : Elmo_PPM
  * @function : 单轴平滑速度的位置模式
  * @input    : elmoID      取elmo节点ID
  *             speed       速度(cnt/s)
  *             position    目标位置(cnt)
  *             PPMmode     运行模式
  *             POS_ABS     PPM运行方式:绝对位置
  *             POS_REL     PPM运行方式:相对位置
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_PPM(u8 elmoID, u32 speed, s32 position, u8 PPMmode)
{
    u8 i = 0;

    /* 如果使用广播控制,当前模式不为位置模式,切换为位置模式 */
    if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_PCM)
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(90);
        }	
        /* 将所有状态变量赋值为PCM */					
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_PCM;			
        }

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_PCM);
        Elmo_Delay100us(20);
        
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }  
    /* 使用单个ELMO控制,当前模式不为位置模式,切换为位置模式 */
    else if(elmo[elmoID].CurOPMode != UM_PCM)
    {
        if(elmo[elmoID].CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(90);
        }

        elmo[elmoID].CurOPMode = UM_PCM;
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_PCM);
        Elmo_Delay100us(200);

        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);


        /* 如果ELMO只挂载了1个，不管怎么改其的模式，elmo[0]的模式和它一样 */				
        if(ELMO_NUM == 1)      
        {
            elmo[0].CurOPMode = UM_PCM;
        }		
        /* 用来判断是否所有电机模式相同，从而确定elmo[0]的模式 */
        else 
        {
            for(i = 1; i < ELMO_NUM; i++) 
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_PCM;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }			
        }
    }

    /* 设置目标速度 */
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"SP", 0, TYPE_INTEGER, speed);
    Elmo_Delay100us(20);

    /* 根据运行模式设置位置 */
    if(PPMmode)
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PA", 0, TYPE_INTEGER, position);
        Elmo_Delay100us(20);
    }
    else
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PR", 0, TYPE_INTEGER, position);
        Elmo_Delay100us(20);
    }
    
    /* 运行位置模式 */
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
    Elmo_Delay100us(10);
    return 0;
}

/**
  ******************************************************************************
  * @name     : Elmo_Close
  * @function : 驱动输出关闭，电机靠惯性继续行驶
  * @input    : elmoID  取elmo的节点ID
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_Close(u8 elmoID)
{
    u8 i = 0;
    
    /* 如果使用广播控制,将所有ELMO失能 */
    if(elmoID == 0)
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
        Elmo_Delay100us(100);
        
        /* 将所有状态变量赋值为IDLE */
        for(i = 0; i <= ELMO_NUM; i++)
        {
            elmo[i].CurOPMode = UM_IDLE;			
        }
    }
    /* 使用单个ELMO控制 */
    else 
    {
        RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
        Elmo_Delay100us(100);
        elmo[elmoID].CurOPMode = UM_IDLE;		

        /* 如果ELMO只挂载了1个，不管怎么改其的模式，elmo[0]的模式和它一样 */			
        if(ELMO_NUM == 1)       
        {
            elmo[0].CurOPMode = UM_IDLE;
        }		
        /* 用来判断是否所有电机模式相同，从而确定elmo[0]的模式 */
        else 
        {
            for(i = 1; i < ELMO_NUM; i++)  
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_IDLE;
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }
        }
    }
    return 0;
}


/**
  ******************************************************************************
  * @name     : Elmo_Stop
  * @function : 刹车，电机抱死
  * @input    : elmoID  取elmo的节点ID
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_Stop(u8 elmoID)
{
    u8 i = 0;
    
    /* 如果CAN总线上轴与轴的控制模式不相同，且发出广播指令，依次关闭 */
    if(elmoID == 0 && elmo[elmoID].CurOPMode == UM_UNC)  
    {
        /* 循环判断个个电机的状态，并关闭每一个电机 */
        for(i = 1; i <= ELMO_NUM; i++)            
        {		
            /* 当前模式为释放电机,先打开电机，再抱死 */		
            if (elmo[i].CurOPMode == UM_IDLE) 
            {
                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
                Elmo_Delay100us(30);				
            }
            /* 当前模式为力矩模式,先切换为速度模式，再关闭 */
            else if(elmo[i].CurOPMode == UM_TCM)
            {
                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);	
                Elmo_Delay100us(200);			

                elmo[i].CurOPMode = UM_SCM;			

                RPDO2_Cmd_data(&elmo[i], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
                Elmo_Delay100us(10);

                RPDO2_Cmd_data(&elmo[i], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
                Elmo_Delay100us(30);

                /* 设置目标速度 */
                RPDO2_Cmd_data(&elmo[i], (u8 *)"JV", 0, TYPE_INTEGER, 0);
                Elmo_Delay100us(10);

                /* 运行速度模式 */
                RPDO2_Cmd_string(&elmo[i], (u8 *)"BG");
                Elmo_Delay100us(10);
            }	
                
            RPDO2_Cmd_string(&elmo[i], (u8 *)"ST");
            Elmo_Delay100us(10);
        }	
        
        /* 用来判断是否所有电机模式相同，从而确定elmo[0]的模式,能够进入这个if的不可能是在CAN线上只有单轴 */
        for(i = 1; i < ELMO_NUM; i++)  
        {
            if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
            {
                elmo[0].CurOPMode = UM_SCM;         //如果在STOP模式里面电机模式统一了，只有可能是SCM模式
            }
            else
            {
                elmo[0].CurOPMode = UM_UNC;
                break;
            }
        }											
    }
    /* 发出广播指令，但是总线上多轴的控制模式都相同，这时候可以一起关闭 */
    else if(elmoID == 0 && elmo[elmoID].CurOPMode != UM_UNC)  
    {
        /* 当前模式为释放电机,先打开电机，再抱死 */		
        if (elmo[elmoID].CurOPMode == UM_IDLE) 
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);				
        }
        /* 当前模式为力矩模式,先切换为速度模式，再关闭 */
        else if(elmo[elmoID].CurOPMode == UM_TCM)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			

            /* 广播模式下，所有速度模式都改为SCM */
            for(i=0; i <= ELMO_NUM; i++) 
            {
                elmo[i].CurOPMode = UM_SCM;			
            }
        
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
            Elmo_Delay100us(10);

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);

            /* 设置目标速度 */
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, 0);
            Elmo_Delay100us(10);

            /* 运行速度模式 */
            RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
            Elmo_Delay100us(10);
        }	
            
        RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"ST");
        Elmo_Delay100us(10);	
        
    }
    /* 使用单个ELMO控制 */
    else    
    {
        /* 当前模式为释放电机,先打开电机，再抱死 */		
        if (elmo[elmoID].CurOPMode == UM_IDLE) 
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);				
        }
        /* 当前模式为力矩模式,先切换为速度模式，再关闭 */
        else if(elmo[elmoID].CurOPMode == UM_TCM)
        {
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);	
            Elmo_Delay100us(200);			

            elmo[elmoID].CurOPMode = UM_SCM;			

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
            Elmo_Delay100us(10);

            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
            Elmo_Delay100us(30);

            /* 设置目标速度 */
            RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"JV", 0, TYPE_INTEGER, 0);
            Elmo_Delay100us(10);

            /* 运行速度模式 */
            RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"BG");
            Elmo_Delay100us(10);

            /* 如果ELMO只挂载了1个，不管怎么改其的模式，elmo[0]的模式和它一样 */			
            for(i=1;i<ELMO_NUM;i++)  //用来判断是否所有电机模式相同，从而确定elmo[0]的模式
            {
                if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
                {
                    elmo[0].CurOPMode = UM_SCM;    //如果在STOP模式里面电机模式统一了，只有可能是SCM模式
                }
                else
                {
                    elmo[0].CurOPMode = UM_UNC;
                    break;
                }
            }				
        }	
            
        RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"ST");
        Elmo_Delay100us(10);		
    }
    return 0;		
}

/**
  ******************************************************************************
  * @name     : Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
  * @function : 设置速度模式和位置模式的电机加减速
  * @input    : elmoID  取elmo的节点ID
  *             acc     加速度,加速度最大不能超过1000000000,同时应考虑电机性能
  *             dec     减速度,减速度最大不能超过1000000000,同时应考虑电机性能
  * @output   : 0       函数调用成功
  *             1       函数调用失败
  ******************************************************************************
  */
u8 Elmo_SetAcc(u8 elmoID, u32 acc, u32 dec)
{ 
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(30);	
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PM", 0, TYPE_INTEGER, 0x01);
    Elmo_Delay100us(10);	
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"AC", 0, TYPE_INTEGER, acc);
    Elmo_Delay100us(10);
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"DC", 0, TYPE_INTEGER, dec);
    Elmo_Delay100us(10);
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(100);
    return 0;
}
/*============================================================================*/


/** @ElmoCanOpen 底层函数 ----------------------------------------------------*/
/**
  ******************************************************************************
  * @name     : NMTCmd
  * @function : 对于CANOPEN的NMT状态设置
  * @input    : elmo    取elmo的节点ID
  *             MNTCmd  NMT指令,NMT_xxx
  * @output   : None
  ******************************************************************************
  */
static void NMTCmd(Elmo *elmo, u8 MNTCmd)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = COBID_NMT_SERVICE << 21;    //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 2;                          //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = MNTCmd;
    CanFrame.Data.u08[1] = elmo->NodeID;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RSDO
  * @function : 使用下载SDO进行指令发送
  * @input    : elmo      取elmo节点ID
  *             Index     索引
  *             SubIndex  子索引
  *             Data      数据
  * @output   : None
  ******************************************************************************
  */
static void RSDO(Elmo *elmo, u16 Index, u8 SubIndex, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RSDO + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u32[0] = ((uint32_t)SubIndex) << 24 | ((uint32_t)Index<<8) | 0x22;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

static void RSDORead(Elmo *elmo, u16 Index, u8 SubIndex, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RSDO + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u32[0] = ((uint32_t)SubIndex) << 24 | ((uint32_t)Index<<8) | 0x40;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RPDO2_Cmd_data
  * @function : 使用二进制编码对ELMO发送数据指令
  *             CANopen RPDO2 -> 0x2012 二进制输入-设置功能
  * @input    : elmo    取elmo节点ID
  *             Cmd     命令,以字符串形式输入
  *             Index   命令的下标           
  *             Type    数据类型
  *             Data    数据
  * @output   : None
  ******************************************************************************
  */
static void RPDO2_Cmd_data(Elmo *elmo, u8 *Cmd, u8 Index, u8 Type, u32 Data)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RPDO2 + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 8;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = (*Cmd++);
    CanFrame.Data.u08[1] = (*Cmd);
    CanFrame.Data.u08[2] = Index;
    CanFrame.Data.u08[3] = Type<<7;
    CanFrame.Data.u32[1] = Data;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : RPDO2_Cmd_string
  * @function : 使用二进制编码对ELMO发送字符串指令
  *             CANopen RPDO2 -> 0x2012 二进制输入-执行功能
  * @input    : elmo    取elmo节点ID
                cmd     命令,以字符串形式输入
  * @output   : None
  ******************************************************************************
  */
static void RPDO2_Cmd_string(Elmo *elmo, u8 *Cmd)
{
    T_CanFrame CanFrame;
    
    CanFrame.IxR  = (COBID_RPDO2 + elmo->NodeID) << 21;  //CanFrame.IxR  = (pCANDATA->COBID << 21);
    CanFrame.DTxR = 4;                                  //CanFrame.DTxR = (pCANDATA->DLC)&0x0f;
    CanFrame.Data.u08[0] = (*Cmd++);
    CanFrame.Data.u08[1] = (*Cmd);
    CanFrame.Data.u08[2] = 0;
    CanFrame.Data.u08[3] = 0;
    
    CAN_SendFrame(&CanFrame);
}

/**
  ******************************************************************************
  * @name     : SendHeart2Elmo
  * @function : 主控发送心跳帧给elmo
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
void SendHeart2Elmo(void)
{
    CAN_SendFrame((T_CanFrame*)&MainHeartFrame);
}
/*============================================================================*/


/**
  ******************************************************************************
  * @name     : Variate_init
  * @function : 参数初始化
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
static void Variate_init(void)
{
    ElmoHeartbeat = 0;
    memset (elmo, 0, (ELMO_NUM + 1)*sizeof(Elmo));
    memset (&chassisGroup, 0, sizeof(Elmo));
    memset (&slidewayGroup, 0, sizeof(Elmo));
}

/**
  ******************************************************************************
  * @name     : Elmo_software_delay_ms
  * @function : Elmo软件延时
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
void Elmo_software_delay_ms(unsigned int t)
{
    int i;
    for( i=0;i<t;i++)
    {
        int a = 41580; //at 168MHz 41580 is ok
        while(a--);
    }
}

/**
  ******************************************************************************
  * @name     : f2h
  * @function : 将浮点数转化为8字节十六进制数(IEEE754)
  * @input    : x   浮点数 
  * @output   : None
  ******************************************************************************
  */
static u32 f2h(float x)
{
    u32 *p = (u32 *)&x;
    return ((u32)*p);
}

/**
  ******************************************************************************
  * @name     : CAN1_RX0_IRQHandler
  * @function : CAN中断处理函数
  * @input    : None
  * @output   : None
  ******************************************************************************
  */
int32_t Encoder_Data_Main = 0, Encoder_Data_Serve = 0;
void CAN1_RX0_IRQHandler()
{
    static CanRxMsg RxMsg;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMsg);

    if(RxMsg.StdId > COBID_HEARTBEAT && RxMsg.StdId < (COBID_HEARTBEAT+0xf) && RxMsg.Data[0] == 0x05)
    {
        ElmoHeartbeat |= (1<<((RxMsg.StdId&0x0F)-1));
        if(ElmoHeartbeat == DEF_ElmoHeartbeat_VALUE)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            ElmoHeartbeat = 0;
            xSemaphoreGiveFromISR( ElmoSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
    
    else if(RxMsg.StdId == COBID_TSDO + MOTOR_ID_HIT_MAIN)
    {
        if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            memcpy(&Encoder_Data_Main, &RxMsg.Data[4],sizeof(int32_t));
            xSemaphoreGiveFromISR( PosRacketMainSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
    else if(RxMsg.StdId == COBID_TSDO + MOTOR_ID_HIT_SERVE)
    {
        if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
        {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
            memcpy(&Encoder_Data_Serve, &RxMsg.Data[4],sizeof(int32_t));
            xSemaphoreGiveFromISR( PosRacketServeSem, &xHigherPriorityTaskWoken );
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/**
  * @brief  Elmo_Read_POS,读取对应的elmo的编码器的数据
  * @param  elmoID   无符号8位数据
  *         pEncData int32_t 型指针，指向返回数据
  * @retval 0   ：成功
  *         1   ：失败
  * @time   RSDORead
  *         -   1ms间隔连续发送1000次，最大读取(发送完命令至接收到数据)时间未超过 80000/84us = 952.38us
  *         -   此处等待1.5ms
  * @note   中断中读取
  *         RxMsg.StdId == COBID_TSDO + ID;
  *         if ((RxMsg.Data[2]<<8 | RxMsg.Data[1]) == 0x6064)
  *             memcpy(&Encoder_Data, &RxMsg.Data[4],sizeof(int32_t));
  *         目前仅支持 ID：MOTOR_ID_HIT_MAIN，MOTOR_ID_HIT_SERVE ！！！
  */
uint32_t Elmo_Read_POS(u8 elmoID, int32_t *pEncData)
{
    uint32_t err = 1;
    /* 现在始终开启CAN1接收中断，故不必再开启CAN1接收中断 */
    //CAN1->IER |= CAN_IT_FMP0;

    /* 读取elmo编码器的数值 */
    RSDORead(&elmo[elmoID], 0x6064, 0x00, 0);
    
    if(elmoID == MOTOR_ID_HIT_MAIN)
    {
        if(xSemaphoreTake(PosRacketMainSem, 3*portTICK_MS/2) == pdTRUE)   //1.5ms
        {
            *pEncData = Encoder_Data_Main;
            err = 0;
        }
        else
            err = 1;
    }
    else if(elmoID == MOTOR_ID_HIT_SERVE)
    {
        if(xSemaphoreTake(PosRacketServeSem, 3*portTICK_MS/2) == pdTRUE)   //1.5ms
        {
            *pEncData = Encoder_Data_Serve;
            err = 0;
        }
        else
            err = 1;
    }
    
    /* 关闭CAN1接收中断 */
    //CAN1->IER &= ~CAN_IT_FMP0;
    return err;
}

/**
  ******************************************************************************
  * @name     : Elmo_Set_POS
  * @function : 读取对应的elmo的编码器的数据
  * @input    : elmoID   无符号8位数据 
  * @output   : None
  ******************************************************************************
  */
void Elmo_Set_POS(u8 elmoID,s32 POS)
{
    //关闭电机，这是设置位置值的先决条件
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(90);

    //对编码器的值进行设置
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"PX", 0, TYPE_INTEGER, POS);
    Elmo_Delay100us(90);

    //开启电机
    RPDO2_Cmd_data(&elmo[elmoID], (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
    Elmo_Delay100us(90);
    //更新当前电机状态
    elmo[elmoID].CurOPMode = UM_UNC;
}

/**
  ******************************************************************************
  * @name     : Elmo_Read_ACT_CUR
  * @function : 读取对应的elmo的有效电流
  * @input    : elmoID   无符号8位数据 
  * @output   : None
  ******************************************************************************
  */
void Elmo_Read_ACT_CUR(u8 elmoID)
{
    //开启TPDO2
    RSDO(&elmo[0], 0x1A01, 0x00,1);
    Elmo_Delay100us(50);
    //读取elmo编码器的数值
    RPDO2_Cmd_string(&elmo[elmoID], (u8 *)"IQ");
    Elmo_Delay100us(20);

    //关闭TPDO2
    RSDO(&elmo[0], 0x1A01, 0x00,0);
    Elmo_Delay100us(50);
}
/*============================================================================*/


/** @GroupOperation 组操作模式 -----------------------------------------------*/
// PVM ---------------------------------------------------------------
void Chassis_Elmo_PVM(s32 speed1, s32 speed2, s32 speed3, s32 speed4)
{
    u8 i;
    for(i = 5; i < 8; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            chassisGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            chassisGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(chassisGroup.CurOPMode != UM_SCM)        /* 当前模式不为速度模式,切换为速度模式 */
    {
        if(chassisGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&chassisGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }
    
    /* 设置目标速度 */
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_1], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_1*speed1);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_2], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_2*speed2);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_3], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_3*speed3);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_CHASSIS_4], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_CHASSIS_4*speed4);
    Elmo_Delay100us(10);

    /* 运行速度模式 */
    RPDO2_Cmd_string(&chassisGroup, (u8 *)"BG");
    Elmo_Delay100us(20);
    
    for(i = 5; i <= 8; i++)
    {
        elmo[i].CurOPMode = UM_SCM;
    }
}

void Slideway_Elmo_PVM(s32 speed1,s32 speed2)
{
    u8 i;
    
    for(i = 1; i < 2; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            slidewayGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            slidewayGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(slidewayGroup.CurOPMode != UM_SCM)        /* 当前模式不为速度模式,切换为速度模式 */
    {
        if(slidewayGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
    }
    
    /* 设置目标速度 */
    RPDO2_Cmd_data(&elmo[MOTOR_ID_SLIDEWAY_X], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_SLIDEWAY_X*speed1);
    RPDO2_Cmd_data(&elmo[MOTOR_ID_SLIDEWAY_Y], (u8 *)"JV", 0, TYPE_INTEGER, MOTOR_DIR_SLIDEWAY_Y*speed2);
    Elmo_Delay100us(10);

    /* 运行速度模式 */
    RPDO2_Cmd_string(&slidewayGroup, (u8 *)"BG");
    Elmo_Delay100us(20);
    
    for(i = 1; i <= 2; i++)
    {
        elmo[i].CurOPMode = UM_SCM;
    }
}

// Stop --------------------------------------------------------------------------
void Chassis_Elmo_Stop()
{
    u8 i;
    for(i = 5; i < 8; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            chassisGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            chassisGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(chassisGroup.CurOPMode != UM_SCM)        /* 当前模式不为速度模式,切换为速度模式 */
    {
        if(chassisGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&chassisGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);

        for(i = 5; i <= 8; i++)
        {
            elmo[i].CurOPMode = UM_SCM;
        }
    }

    RPDO2_Cmd_string(&chassisGroup, (u8 *)"ST");
    Elmo_Delay100us(10);
}

void Slideway_Elmo_Stop()
{
    u8 i;
    for(i = 1; i < 2; i++)
    {
        if( elmo[i].CurOPMode == elmo[i+1].CurOPMode )
        {
            slidewayGroup.CurOPMode = elmo[i].CurOPMode;
        }
        else
        {
            slidewayGroup.CurOPMode = UM_UNC;
            break;
        }
    }
    
    if(slidewayGroup.CurOPMode != UM_SCM)        /* 当前模式不为速度模式,切换为速度模式 */
    {
        if(slidewayGroup.CurOPMode != UM_IDLE)
        {
            RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
            Elmo_Delay100us(100);
        }
        
        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"UM", 0, TYPE_INTEGER, UM_SCM);
        Elmo_Delay100us(10);

        RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_ON);
        Elmo_Delay100us(30);
        for(i = 1; i <= 2; i++)
        {
            elmo[i].CurOPMode = UM_SCM;
        }
    }
    
    RPDO2_Cmd_string(&slidewayGroup, (u8 *)"ST");
    Elmo_Delay100us(10);
}


// Close ----------------------------------------------------------------------
void Chassis_Elmo_Close()
{
    u8 i;
    
    RPDO2_Cmd_data(&chassisGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(100);

    for(i = 5; i <= 8; i++)
    {
        elmo[i].CurOPMode = UM_IDLE;
    }
}

void Slideway_Elmo_Close()
{
    u8 i;
    
    RPDO2_Cmd_data(&slidewayGroup, (u8 *)"MO", 0, TYPE_INTEGER, MO_OFF);
    Elmo_Delay100us(100);

    for(i = 1; i <= 2; i++)
    {
        elmo[i].CurOPMode = UM_IDLE;
    }
}
