/*****************************************************************************
* @FileName: AuxCom.c
* @Engineer: TenYan
* @version   V1.0
* @Date:     2021-1-12
* @brief     Modemģ���Micro����ͨ��Э��ʵ��
******************************************************************************/
//-----ͷ�ļ�����------------------------------------------------------------
#include "AuxComHW.h"
#include "config.h"

/******************************************************************************
 * Typedef
 ******************************************************************************/


/******************************************************************************
 *   Macros
 ******************************************************************************/
#define AuxCom_Receive     USART6_ReceiveData
#define AuxCom_Transmit    USART6_TransmitData

#define AUX_MAX_TXBUF_SIZE  USART6_TX_BUFFER_SIZE
#define AUX_MAX_RXBUF_SIZE  USART6_RX_BUFFER_SIZE
#define aux_rx_buffer       usart6_rx_buffer
#define aux_rx_size         usart6_rx_size

#define AUX_DELAY_MS(ms)   do { osDelay(ms);} while(0)
#define PART(x)     1


// FSRVЭ�鶨��
#define FCLIT_MSG_HEAD            0x7E    // ֡ͷ
#define FCLIT_MSG_TAIL            0x0D0A  // ֡β
#define FCLIT_SEND_COMMAND_ID     0x84
#define FCLIT_RECV_COMMAND_ID     0x04
#define FCLIT_PACKET_SIZE         RFU_BUFFER_SIZE

#define FCLIT_TIMEOUT_SP          30  // ����100ms
#define FCLIT_RETRY_SP            2   // ��ʱ���Դ���

// ֡ͷ(1B)+���ݳ���(2B)+������(1B)+��ˮ��(1B)+���ݰ�(NB)+У��(1B)+֡β(2B)
#define FCLIT_FRAME_STX_FIELD        0x00
#define FCLIT_FRAME_SIZE_HIGH_FIELD  0x01
#define FCLIT_FRAME_SIZE_LOW_FIELD   0x02
#define FCLIT_FUNCTION_CODE_FIELD    0x03
#define FCLIT_SERIAL_NUMBER_FIELD    0x04
#define FCLIT_DATA_START_FIELD       0x05

/******************************************************************************
 * Data Types and Globals
 ******************************************************************************/
extern volatile uint16_t usart6_rx_size;
extern uint8_t usart6_rx_buffer[USART6_RX_BUFFER_SIZE];
//static uint8_t momi_tx_buffer[MOMI_MAX_TXBUF_SIZE];

static uint8_t aux_com_buffer[USART6_RX_BUFFER_SIZE]; // AuxCom�������ݻ���

rfu_context_t rfu_context;
fclit_context_t fclit_context;

/******************************************************************************
 * RTOS���
 ******************************************************************************/
//=������==============================================================
#define AppThreadPriority_AuxComProcess   osPriorityHigh2
osThreadId_t tid_AuxComProcess;

const osThreadAttr_t AppThreadAttr_AuxComProcess =
{
  .priority = AppThreadPriority_AuxComProcess,
  .attr_bits = osThreadDetached,
  .stack_size = 1024, // �ֽ�
};

//=������==============================================================
#define AppThreadPriority_AuxComProduce   osPriorityHigh1
osThreadId_t tid_AuxComProduce;

const osThreadAttr_t AppThreadAttr_AuxComProduce =
{
  .priority = AppThreadPriority_AuxComProduce,
  .attr_bits = osThreadDetached,
  .stack_size = 1024, // �ֽ�
};

/******************************************************************************
 * ����У���
*******************************************************************************/
uint8_t iFCLIT_CalcSumCheck(uint8_t* pbuf,uint16_t len)
{
  uint16_t i;
  uint8_t sum = 0;

  for (i = 0; i < len; i++)
  {
    sum += pbuf[i];
  }

  return sum;
}

/******************************************************************************
 * FCLIT�����Ե��ú���(����100mS,������)
*******************************************************************************/
void FCLIT_Do100msTasks(void)
{

}

/******************************************************************************
 * ��ʼ��FSRV����
*******************************************************************************/
void FCLIT_Init(void)
{
  fclit_context.state = FSRV_STATE_IDLE;
  fclit_context.dev_id = 0x00;  // ��Ч
  fclit_context.msg_sn = 0x00;
  fclit_context.fw_packet_index = 0x00;
  fclit_context.fw_packet_size = FSRV_PACKET_SIZE;
  fclit_context.start_address = 0x00;
  fclit_context.ending_address = 0x00;
  fclit_context.total_block_count = 0x00;
  fclit_context.percent = 0x00;
  fclit_context.result = 0x00;

  fclit_context.tx_size = 0x00;
  fclit_context.tx_data = aux_tx_buffer;

  fclit_context.rx_size = 0x00;
  fclit_context.rx_data = aux_rx_buffer;

  fclit_context.retry_sp = FSRV_RETRY_SP;
  fclit_context.retry_cnt = 0;
  
  fclit_context.timeout_100ms_sp = FSRV_TIMEOUT_SP;
  fclit_context.timeout_100ms = fclit_context.timeout_100ms_sp;
}



pThis->rx_msg_sn = msg_sn;  // ��ˮ��
//===============================================================

pThis->un_result = 0x00;  // ִ�гɹ�
pThis->state = FCLIT_STATE_NOTIFIED;  // ״̬��ת(��Ӧ����֪ͨ)
pThis->retry_cnt = 0x00;  // ���Լ�����
pThis->timeout_100ms = pThis->timeout_100ms_sp;  // ��λ��ʱ��ʱ��
pThis->msg_sn = 0x00;  // ������ˮ��



#if (PART("����FCLIT����"))
/*************************************************************************
 * ���͹̼����������Ϣ
*************************************************************************/
//==��4Gģ�鷢�͹̼�����֪ͨ��Ӧ��(ST->4G)================================
void iFCLIT_SendUnRspMsg(fclit_context_t* pThis)
{
  uint8_t* pdata = pThis->tx_data;
  uint16_t pos = 0;
  uint16_t msg_len;
  uint8_t check_sum;

#if FCLIT_DEBUG
  PcDebug_Printf("FclitUN:%d!\n", pThis->un_result);
#endif

  pdata[FCLIT_FRAME_STX_FIELD] = FCLIT_MSG_HEAD;  // ֡ͷ(1B)
  pdata[FCLIT_FUNCTION_CODE_FIELD] = FCLIT_SEND_COMMAND_ID;  // �����
  pdata[FCLIT_SERIAL_NUMBER_FIELD] = pThis->rx_msg_sn;  // ��Ӧ��ˮ��
  pos = FCLIT_DATA_START_FIELD;

  //==��������======================================================
  pdata[pos++] = 0x01;  //�����ʶ
  pdata[pos++] = pThis->un_result;  // Ӧ����(0=�ɹ�, 1=��ʽ�쳣,2=�豸��֧��,3=����)
  //===============================================================

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  check_sum = iFCLIT_CalcSumCheck(&pdata[3], msg_len); // ����У��ֵ:������ſ�ʼ����
  pdata[pos++] = check_sum;  // У���

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  pdata[FCLIT_FRAME_SIZE_HIGH_FIELD] = (msg_len>>8) & 0xFF; // ����
  pdata[FCLIT_FRAME_SIZE_LOW_FIELD] = msg_len & 0xFF;

  pdata[pos++] = (FCLIT_MSG_TAIL>>8) & 0xFF;  // ֡β(2B)
  pdata[pos++] = FCLIT_MSG_TAIL & 0xFF;

  AuxCom_Transmit(pdata, pos);

  if(pThis->un_result == 0x00)  // ִ�гɹ�
  {  pThis->state = FCLIT_STATE_START;}  // ��������
  else
  {  pThis->state = FCLIT_STATE_IDLE;}  // ��������
}

//==�̼�������������(ST->4G)==============================================
void iFCLIT_SendUdReqMsg(fclit_context_t* pThis)
{
  uint8_t* pdata = pThis->tx_data;
  uint16_t pos = 0;
  uint16_t msg_len;
  uint8_t check_sum;

#if FCLIT_DEBUG
  PcDebug_Printf("FclitUN!\n");
#endif

  pThis->msg_sn++;
  pdata[FCLIT_FRAME_STX_FIELD] = FCLIT_MSG_HEAD;  // ֡ͷ(1B)
  pdata[FCLIT_FUNCTION_CODE_FIELD] = FCLIT_SEND_COMMAND_ID;  // �����
  pdata[FCLIT_SERIAL_NUMBER_FIELD] = pThis->msg_sn;  // ��ˮ��
  pos = FCLIT_DATA_START_FIELD;

  //==��������======================================================
  pdata[pos++] = 0x01;  //�����ʶ
  pdata[pos++] = 0x00;  // Ӧ����(0=�ɹ�, 1=��ʽ�쳣,2=�豸��֧��,3=����)
  //===============================================================

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  check_sum = iFCLIT_CalcSumCheck(&pdata[3], msg_len); // ����У��ֵ:������ſ�ʼ����
  pdata[pos++] = check_sum;  // У���

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  pdata[FCLIT_FRAME_SIZE_HIGH_FIELD] = (msg_len>>8) & 0xFF; // ����
  pdata[FCLIT_FRAME_SIZE_LOW_FIELD] = msg_len & 0xFF;

  pdata[pos++] = (FCLIT_MSG_TAIL>>8) & 0xFF;  // ֡β(2B)
  pdata[pos++] = FCLIT_MSG_TAIL & 0xFF;

  AuxCom_Transmit(pdata, pos);

}

//==��4G�ϱ��������(ST->4G)==============================================
void iFCLIT_SendUrReqMsg(fclit_context_t* pThis)
{
  uint8_t* pdata = pThis->tx_data;
  uint16_t pos = 0;
  uint16_t msg_len;
  uint8_t check_sum;

#if FCLIT_DEBUG
  PcDebug_Printf("FclitUN!\n");
#endif

  pThis->msg_sn++;
  pdata[FCLIT_FRAME_STX_FIELD] = FCLIT_MSG_HEAD;  // ֡ͷ(1B)
  pdata[FCLIT_FUNCTION_CODE_FIELD] = FCLIT_SEND_COMMAND_ID;  // �����
  pdata[FCLIT_SERIAL_NUMBER_FIELD] = pThis->msg_sn;  // ��ˮ��
  pos = FCLIT_DATA_START_FIELD;

  //==��������======================================================
  pdata[pos++] = 0x01;  //�����ʶ
  pdata[pos++] = 0x00;  // Ӧ����(0=�ɹ�, 1=��ʽ�쳣,2=�豸��֧��,3=����)
  //===============================================================

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  check_sum = iFCLIT_CalcSumCheck(&pdata[3], msg_len); // ����У��ֵ:������ſ�ʼ����
  pdata[pos++] = check_sum;  // У���

  msg_len = pos - 3; // ����֡ͷ�����ݳ���
  pdata[FCLIT_FRAME_SIZE_HIGH_FIELD] = (msg_len>>8) & 0xFF; // ����
  pdata[FCLIT_FRAME_SIZE_LOW_FIELD] = msg_len & 0xFF;

  pdata[pos++] = (FCLIT_MSG_TAIL>>8) & 0xFF;  // ֡β(2B)
  pdata[pos++] = FCLIT_MSG_TAIL & 0xFF;

  AuxCom_Transmit(pdata, pos);
}

/*************************************************************************
 * 100ms����һ��
*************************************************************************/
void FCLIT_ProduceSendMsg(void)
{
  if (pThis->state==FSRV_STATE_IDLE) /// ����
  {
    return;
  }

  switch (pThis->state)
  {
  case FSRV_STATE_START: /// ��������(��ʼ������)
    iFSRV_start(pThis);
    break;

  case FSRV_STATE_SEND_UN:  /// ����֪ͨ����
    iFSRV_SendUnMsg(pThis);
    pThis->state = FSRV_STATE_WAIT_REQ;
    break;

  case FSRV_STATE_WAIT_REQ:  /// �ȴ�ST�������Ӧ
    break;

  case FSRV_STATE_SEND_UD:  /// ������������(��ӦST����)
    iFSRV_SendUdMsg(pThis);
    pThis->state = FSRV_STATE_WAIT_REQ;
    break;

  case FSRV_STATE_REPORT:  /// ��������ϱ��������
    iFSRV_SendUrMsg(pThis);
    pThis->state = FSRV_STATE_IDLE;
    break;

  default:
    break;
  }

  FSRV_Do100msTasks();
}
#endif

/******************************************************************************
 * �����յ�4Gģ����Ϣ
*******************************************************************************/
#if (PART("����FCLIT����"))
//==�ж�MOMI��ϢУ���==========================================================
uint8_t iFCLIT_CheckMsg(fclit_context_t* pThis)
{
  uint8_t *pdata = pThis->rx_data;
  uint16_t size = pThis->rx_size;
  uint8_t received_check_sum;
  uint8_t calculated_check_sum;
  uint16_t msg_len;
  uint16_t msg_tail;

  if (pdata[0] != FCLIT_MSG_HEAD) // ֡ͷ����
  {
    return FCLIT_NOK;
  }

  msg_tail = (uint16_t)(pdata[size-2]<<8) + pdata[size-1]; // ��ȡ֡β
  if (msg_tail != FCLIT_MSG_TAIL) // ֡β����
  {
    return FCLIT_NOK;
  }

  if (pdata[3] != FCLIT_RECV_COMMAND_ID) // �����ֶδ���
  {
    return FCLIT_NOK;
  }

  msg_len = size-6; // ����֡ͷ(1B)+���ݳ���(2B)+�����(1B)+֡β(2B)
  received_check_sum = pdata[size-3]; // ���յ��ļ����
  calculated_check_sum = iMomi_CalcSumCheck(&pdata[FCLIT_FUNCTION_CODE_FIELD], msg_len); // ����У��ֵ:������ſ�ʼ����
  if (received_check_sum==calculated_check_sum) //check if crc's match
  {
    return FCLIT_OK;
  }
  else
  {
    return FCLIT_NOK;
  }
}

//==��������ָ֪ͨ��(4G->ST)===================================================
void iFCLIT_AnalyzeUnReqMsg(fclit_context_t* pThis)
{
  uint8_t *pdata;
  uint16_t pos;
  uint16_t msg_len;
  uint16_t msg_sn;
  uint8_t rfu_type;
  uint8_t rfu_dev;

  if(pThis->state != FCLIT_STATE_IDLE) // ����æ,���Դ˴�����
  {
    return;
  }
  
  msg_sn = pThis->rx_data[FCLIT_SERIAL_NUMBER_FIELD];  // ��Ϣ��ˮ��
  pThis->rx_msg_sn = msg_sn;  // ��ˮ��
  pThis->un_result = 0x00;  // ִ�гɹ�
  pThis->state = FCLIT_STATE_NOTIFIED;  // ״̬��ת(��������֪ͨ��Ӧ)
  //pThis->retry_cnt = 0x00;  // ���Լ�����
  //pThis->timeout_100ms = pThis->timeout_100ms_sp;  // ��λ��ʱ��ʱ��
  //pThis->msg_sn = 0x00;  // ������ˮ��

  msg_len = pThis->rx_data[FCLIT_FRAME_SIZE_HIGH_FIELD];  // ���ݳ���
  msg_len <<= 8;
  msg_len += pThis->rx_data[FCLIT_FRAME_SIZE_LOW_FIELD];
  if (msg_len < 20) // ������
  {
    pThis->un_result = 0x01;  // ��ʽ�쳣
    return;
  }
  msg_len -= 3;  // ���ݰ��ĳ���(������������(1B)+��ˮ��(1B)+У��(1B))

  //==��������======================================================
  pos = 0x00;
  pdata = &pThis->rx_data[FCLIT_DATA_START_FIELD];
  pos++;  // �����ʶ
  rfu_type = pdata[pos++];  // ��������
  rfu_dev = pdata[pos++];  // ����Ŀ���豸
  if(rfu_dev > 3)
  {
    pThis->un_result = 0x02;  // �豸��֧��
    return;
  }
  //==��������======================================================
  rfu_context.type = rfu_type;  // ��������
  rfu_context.dev = rfu_dev;  // ����Ŀ���豸

  rfu_context.file_name_length = pdata[pos++];  // �̼����Ƴ���
  memcpy(rfu_context.file_name, &pdata[pos], rfu_context.file_name_length);  // �̼�����(ASCII)
  pos += rfu_context.file_name_length;

  rfu_context.file_version_length = pdata[pos++];  // �̼��汾����
  memcpy(rfu_context.file_version, &pdata[pos], rfu_context.file_version_length);  // �̼��汾(ASCII)
  pos += rfu_context.file_version_length;

  rfu_context.file_length = pdata[pos++];  // �̼���С(4B)
  rfu_context.file_length <<= 8;
  rfu_context.file_length += pdata[pos++];
  rfu_context.file_length <<= 8;
  rfu_context.file_length += pdata[pos++];
  rfu_context.file_length <<= 8;
  rfu_context.file_length += pdata[pos++];

  rfu_context.total_block_count = pdata[pos++];  // �̼��ܰ���(2B)
  rfu_context.total_block_count <<= 8;
  rfu_context.total_block_count += pdata[pos++];

  rfu_context.plain_crc32val = pdata[pos++];  // �̼�CRCУ��(4B)
  rfu_context.plain_crc32val <<= 8;
  rfu_context.plain_crc32val += pdata[pos++];
  rfu_context.plain_crc32val <<= 8;
  rfu_context.plain_crc32val += pdata[pos++];
  rfu_context.plain_crc32val <<= 8;
  rfu_context.plain_crc32val += pdata[pos++];

  pThis->block_size = pdata[pos++];  // ������С(�̶�ֵ1024)
  pThis->block_size <<= 8;
  pThis->block_size += pdata[pos++];

  if(pThis->block_size >= 1024)  // ������С�ж�
  {
    pThis->un_result = 0x03;  // ����
    return;
  }
  //===============================================================
}

/***********************************************************************
** ��������: ���յ�����������
** ��    ��: ptr,    ͨѶЭ���������ʶ(0x01)��һ���ֽ�ָ���ַ
             uslen�� Э���������峤��-1(ȥ�������ʶ1�ֽڳ���)
************************************************************************/
uint16_t iFCLIT_AnalyzeUdRspMsg(fclit_context_t* pThis)
{
  uint16 usPackSn, usTotalPacks, usPackLen;
  uint32 uiTemp;

  if (uslen<6)
    stu_ExtMCUUpgrade.ucUpgradeStep = 0; //�����˳�

  usPackSn = (*p<<8) + *(p+1);
  p += 2;
  usTotalPacks = (*p<<8) + *(p+1);
  p += 2;
  usPackLen = (*p<<8) + *(p+1);
  p += 2;

  if (usPackSn != FirmwareUpdate.usRequestPacketSn)
    return 0;
  
  if (usTotalPacks!= FirmwareUpdate.usPackets)
    return 0;
  
  if (usPackLen == 0)
    return 0;

  if (FirmwareUpdate.ucdev==3)
    WriteToFlash(FLASH_PAGEADDR_UPGRADE +(usPackSn*(FIRMWARE_PACKET_LEN/256)), 0, usPackLen, p);
  else
    WriteToFlash(FLASH_Firmware_MCU +(usPackSn*(FIRMWARE_PACKET_LEN/256)), 0, usPackLen, p);

  FirmwareUpdate.usRequestPacketSn++;
  if (FirmwareUpdate.usRequestPacketSn==FirmwareUpdate.usPackets)//�������
  {
    FirmwareUpdate.usLastPacketLen = usPackLen;
    uiTemp = GetCrc32();
    if (uiTemp==FirmwareUpdate.uiCrc)
    {
      stu_ExtMCUUpgrade.ucResult = 0;
      if (FirmwareUpdate.ucdev==1)
      {
        stu_McuFirmware.ucRcvPackflag = 1;
        stu_McuFirmware.ucLoadStep = 1;
        stu_McuFirmware.usReadFlashSN = 0;
        //  stu_McuFirmware.usMcuUPdateTimerOut = FirmwareUpdate.usPackets*2+30;
        stu_McuFirmware.usMcuUPdateTimerOut = 600;   //10����
        PC_SendDebugData((uint8 *)("MCULoad OK"), 10, DEBUG_ANYDATA);
      }
      else if (FirmwareUpdate.ucdev==4)
      {
        stu_KCMCUDownload.ucRcvPackflag = 1;
        stu_KCMCUDownload.ucLoadStep = 0xFF;
        //  stu_McuFirmware.usMcuUPdateTimerOut = FirmwareUpdate.usPackets*2+30;
        stu_KCMCUDownload.usMcuUPdateTimerOut = 1200;   //20����
        PC_SendDebugData((uint8 *)("KCMCULoad OK"), 10, DEBUG_ANYDATA);

      }
      else
        PC_SendDebugData((uint8 *)("ST Load OK"), 10, DEBUG_ANYDATA);
    }
    else
    {
      stu_ExtMCUUpgrade.ucResult = 1;
      stu_McuFirmware.ucRcvPackflag = 0;
      stu_McuFirmware.ucLoadStep = 0;
      //stu_ExtMCUUpgrade.ucUpgradeStep = 3;
      PC_SendDebugData((uint8 *)("FM CRC ERR"), 10, DEBUG_ANYDATA);
    }
    stu_ExtMCUUpgrade.ucUpgradeStep = 3;
    FirmwareUpdate.usRequestPacketSn = 0;
    stu_ExtMCUUpgrade.ucTimer = 1;
    stu_ExtMCUUpgrade.ucRepeats = 0;
    return 0;
  }
  stu_ExtMCUUpgrade.ucUpgradeStep = 2;
  stu_ExtMCUUpgrade.ucTimer = 0;
  stu_ExtMCUUpgrade.ucRepeats = 0;
  return 0;
}

/***********************************************************************
** ��������: Ext_Mcu_Upgradepackage_Request
** ��������: ����Э������Ӧ��������������
** ��    ��: ptr,    ͨѶЭ���������ʶ(0x01)��һ���ֽ�ָ���ַ
             uslen�� Э���������峤��-1(ȥ�������ʶ1�ֽڳ���)
************************************************************************/
uint16_t iFCLIT_AnalyzeUrRspMsg(fclit_context_t* pThis)
{
  stu_ExtMCUUpgrade.ucUpgradeStep = 0;   //�����˳�
  if (stu_McuFirmware.ucLoadStep == 7)
  {
    stu_McuFirmware.ucLoadStep = 0;
    stu_McuFirmware.ucRcvPackflag = 0;
  }
  if (FirmwareUpdate.ucdev!=3||(memcmp(FirmwareUpdate.aucSWName,"AUX_MCU",7)!=0))
    return;

  CPU_IntDis();
  IAP_Start();  /*Enable interrupt and reset system    */
  CPU_IntEn();
}

//============================================================================
static void FCLIT_ProcessRecvMsg(fclit_context_t* pThis)
{
  uint8_t msg_status;
  uint8_t command;

  msg_status = iFCLIT_CheckMsg(pThis);
  if (msg_status==FCLIT_OK)
  {
    command = pThis->rx_data[FCLIT_DATA_START_FIELD];  // �����ʶ
    switch (command)
    {
    case FCLIT_COMMAND_TYPE_NOTIFIE:  // ����֪ͨ
      iFCLIT_AnalyzeUnReqMsg(pThis);
      break;

    case FCLIT_COMMAND_TYPE_DOWNLOAD:  // ��������
      iFCLIT_AnalyzeUdRspMsg(pThis);
      break;

    case FCLIT_COMMAND_TYPE_REPORT:  // �ϱ����
      iFCLIT_AnalyzeUrRspMsg(pThis);
      break;

    default:
      break;
    }
  }
}
#endif

/******************************************************************************
 * �����������ݷ��ͽ���
*******************************************************************************/
void AppTask_AuxComProduce(void *argument)
{
  while (1)  // 100mִ��һ��
  {
    AUX_DELAY_MS(OS_TICKS_PER_SEC/10); // 100msִ��һ��
    FCLIT_ProduceSendMsg(&fclit_context); // ����MOMI��Ϣ
  }
}

/******************************************************************************
 * ����ͨ���������ڷ�����M2M͸��ָ��
*******************************************************************************/
void AuxCom_ProcessM2mMsg(uint8_t *pdata, uint16_t size)
{
  if (pdata[0]==0x05 && (size>=10))
  {
    memcpy(aux_com_buffer, pdata, size);
    SbusMsg_AuxCom.data_size = size;  // �������ݳ���
    SbusMsg_AuxCom.data = aux_com_buffer;  // ��ȡ���ݵ�ַ
    SYSBUS_PutMbox(SbusMsg_AuxCom);  // ������Ϣ
  }
}

/******************************************************************************
 * �����������ݽ��ս���
*******************************************************************************/
void AppTask_AuxComProcess(void *argument)
{
  uint8_t *pdata = NULL;
  uint16_t len;

  while (1)
  {
    if (AuxCom_Receive(&pdata, &len)) // �ȴ��ź���(����)
    {
      if (pdata[0]==0x05) // ͸��M2M��������CMD_REQָ��
      {
        if ( (pdata[15]=='P' && pdata[16]=='W') || (pdata[15]=='R' && pdata[16]=='C') ) // PWָ���RCָ��
        {
          AuxCom_ProcessM2mMsg(pdata, len);
        }
      }
      else
      {
        fclit_context.rx_data = pdata;
        fclit_context.rx_size = len;
        FCLIT_ProcessRecvMsg(&fclit_context);
      }
    }
  }
}

/*************************************************************************
 *
*************************************************************************/
void AuxCom_ServiceInit(void)
{
  USART6_Initialize(AUXCOM_UART_BAUDRATE);
  FCLIT_Init();
}

/*************************************************************************
 *
*************************************************************************/
void AuxCom_ServiceStart(void)
{
  tid_AuxComProcess = osThreadNew(AppTask_AuxComProcess, NULL, &AppThreadAttr_AuxComProcess);
  tid_AuxComProduce = osThreadNew(AppTask_AuxComProduce, NULL, &AppThreadAttr_AuxComProduce);
}

//-----�ļ�AuxCom.c����---------------------------------------------
