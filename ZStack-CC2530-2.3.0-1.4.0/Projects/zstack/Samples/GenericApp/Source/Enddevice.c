/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 文件名  ：Enbddevice
* 作者    ：Chenyetao
* 版本    ：V0.0.1
* 时间    ：2021/5/11
* 描述    ：文件说明
********************************************************************
* 副本
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* 头文件 ----------------------------------------------------------------*/
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDOBject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"
#if !defined( WIN32)
#include "OnBoard.h"
#endif
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
/* 宏定义 ----------------------------------------------------------------*/
/* 结构体或枚举 ----------------------------------------------------------------*/
/* 内部函数声明 ----------------------------------------------------------------*/
void  GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);
void  GenericApp_SendTheMessage(void);



/* 函数 ----------------------------------------------------------------*/




/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* 文件名  ： GENERICAPP_MAX_CLUSTERS
* 作者    ： chenyetao
* 版本    ： V1.0.0
* 时间    ： 2021/5/11
* 简要    ： 宏定义，保持协议栈里数据的定义格式保持一致文件说明  
********************************************************************/
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS ] = 
{
    GENERICAPP_CLUSTERID
};

//描述一个ZigBee的设备节点，固定格式
const SimpleDescriptionFormat_t GenericApp_SimpleDesc = 
{
    GENERICAPP_ENDPOINT,
    GENERICAPP_PROFID,
    GENERICAPP_DEVICEID,
    GENERICAPP_DEVICE_VERSION,
    GENERICAPP_FLAGS,
    0,
    (cId_t *)NULL,
    GENERICAPP_MAX_CLUSTERS,
    (cId_t *)GenericApp_ClusterList
};

endPointDesc_t GenericApp_epDesc;   //节点描述符
byte GenericApp_TaskID;     //任务优先级
byte GenericApp_TransID;    //数据发送数列号
devStates_t GenericApp_NwkState;    //保持节点状态变量



void GenericApp_Init( byte task_id )
{
    GenericApp_TaskID   =   task_id;
    GenericApp_NwkState   =   DEV_INIT;
    GenericApp_TransID    =   0;
    GenericApp_epDesc.endPoint    =   GENERICAPP_ENDPOINT;
    GenericApp_epDesc.task_id   =   &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc    =   
      (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq    =   noLatencyReqs;
    afRegister( &GenericApp_epDesc ); //
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 函数名  ：GenericApp_ProcessEvent
* 参数    ：byte task_id,UNIT16 evens
* 返回    ：UINT16
* 作者    ：Chenli
* 时间    ：2021/5/11
* 描述    ：消息处理函数
----------------------------------------------------------------*/
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  if ( events &SYS_EVENT_MSG )      //检索收到的命令，没有收到返回NULL
  {
     MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
       (GenericApp_TaskID);
     while ( MSGpkt )  //如果不为空时，判断消息的类型
     {
        switch ( MSGpkt->hdr.event )
        {
        case ZDO_STATE_CHANGE://加入网络后触发ZDO_STATE_CHANGE事件
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.
          status);//获得当前的状态
          if (GenericApp_NwkState ==DEV_END_DEVICE) //若是终端节点加入网络
          {
            GenericApp_SendTheMessage() ;
          }
          break;
        default:
          break;
        }
        osal_msg_deallocate( (uint8 *)MSGpkt );//释放内存
        MSGpkt = (afIncomingMSGPacket_t*)osal_msg_receive(GenericApp_TaskID );
}
        return (events ^ SYS_EVENT_MSG);// 返回未处理的事件 
  }
         return 0;
     }

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* 函数名  ：GenericApp_SendTheMessage
* 参数    ：void
* 返回    ：void
* 作者    ：Chenli
* 时间    ：2021/5/11
* 描述    ：接收消息判断
----------------------------------------------------------------*/
void  GenericApp_SendTheMessage(void)
{
  unsigned char theMessageData[4] = "LED";//存放要发送的数据
          afAddrType_t my_DstAddr;
          my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
          my_DstAddr.endPoint = GENERICAPP_ENDPOINT;//初始化端口号
          my_DstAddr.addr.shortAddr = 0x0000;//直接指定协调器的网络地址
          AF_DataRequest( &my_DstAddr,&GenericApp_epDesc,//调用数据发送函数AF_DataRequst进行无线数据的发送
                         GENERICAPP_CLUSTERID,
                          3,
                          theMessageData,
                          &GenericApp_TransID,//发送 ID
                          AF_DISCV_ROUTE, 
                          AF_DEFAULT_RADIUS);
              HalLedBlink (HAL_LED_2,0,50,500);
  ////使终端节点led2闪烁
  
}