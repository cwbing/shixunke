/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* �ļ���  ��Enbddevice
* ����    ��Chenyetao
* �汾    ��V0.0.1
* ʱ��    ��2021/5/11
* ����    ���ļ�˵��
********************************************************************
* ����
*
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* ͷ�ļ� ----------------------------------------------------------------*/
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
/* �궨�� ----------------------------------------------------------------*/
/* �ṹ���ö�� ----------------------------------------------------------------*/
/* �ڲ��������� ----------------------------------------------------------------*/
void  GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);
void  GenericApp_SendTheMessage(void);



/* ���� ----------------------------------------------------------------*/




/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
* �ļ���  �� GENERICAPP_MAX_CLUSTERS
* ����    �� chenyetao
* �汾    �� V1.0.0
* ʱ��    �� 2021/5/11
* ��Ҫ    �� �궨�壬����Э��ջ�����ݵĶ����ʽ����һ���ļ�˵��  
********************************************************************/
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS ] = 
{
    GENERICAPP_CLUSTERID
};

//����һ��ZigBee���豸�ڵ㣬�̶���ʽ
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

endPointDesc_t GenericApp_epDesc;   //�ڵ�������
byte GenericApp_TaskID;     //�������ȼ�
byte GenericApp_TransID;    //���ݷ������к�
devStates_t GenericApp_NwkState;    //���ֽڵ�״̬����



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

* ������  ��GenericApp_ProcessEvent
* ����    ��byte task_id,UNIT16 evens
* ����    ��UINT16
* ����    ��Chenli
* ʱ��    ��2021/5/11
* ����    ����Ϣ������
----------------------------------------------------------------*/
UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  if ( events &SYS_EVENT_MSG )      //�����յ������û���յ�����NULL
  {
     MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
       (GenericApp_TaskID);
     while ( MSGpkt )  //�����Ϊ��ʱ���ж���Ϣ������
     {
        switch ( MSGpkt->hdr.event )
        {
        case ZDO_STATE_CHANGE://��������󴥷�ZDO_STATE_CHANGE�¼�
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.
          status);//��õ�ǰ��״̬
          if (GenericApp_NwkState ==DEV_END_DEVICE) //�����ն˽ڵ��������
          {
            GenericApp_SendTheMessage() ;
          }
          break;
        default:
          break;
        }
        osal_msg_deallocate( (uint8 *)MSGpkt );//�ͷ��ڴ�
        MSGpkt = (afIncomingMSGPacket_t*)osal_msg_receive(GenericApp_TaskID );
}
        return (events ^ SYS_EVENT_MSG);// ����δ������¼� 
  }
         return 0;
     }

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

* ������  ��GenericApp_SendTheMessage
* ����    ��void
* ����    ��void
* ����    ��Chenli
* ʱ��    ��2021/5/11
* ����    ��������Ϣ�ж�
----------------------------------------------------------------*/
void  GenericApp_SendTheMessage(void)
{
  unsigned char theMessageData[4] = "LED";//���Ҫ���͵�����
          afAddrType_t my_DstAddr;
          my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
          my_DstAddr.endPoint = GENERICAPP_ENDPOINT;//��ʼ���˿ں�
          my_DstAddr.addr.shortAddr = 0x0000;//ֱ��ָ��Э�����������ַ
          AF_DataRequest( &my_DstAddr,&GenericApp_epDesc,//�������ݷ��ͺ���AF_DataRequst�����������ݵķ���
                         GENERICAPP_CLUSTERID,
                          3,
                          theMessageData,
                          &GenericApp_TransID,//���� ID
                          AF_DISCV_ROUTE, 
                          AF_DEFAULT_RADIUS);
              HalLedBlink (HAL_LED_2,0,50,500);
  ////ʹ�ն˽ڵ�led2��˸
  
}