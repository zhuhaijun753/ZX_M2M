/*****************************************************************************
* @FileName: MomiProtocol.h
* @Engineer: TenYan
* @version   V1.0
* @Date:     2020-12-10
* @brief     Modem模块和Micro通信协议头文件
******************************************************************************/
#ifndef _MOMI_PROTOCOL_H_
#define _MOMI_PROTOCOL_H_

/******************************************************************************
 *   Macros
 ******************************************************************************/
#define MODEM_UART_BAUDRATE (115200U)

/******************************************************************************
 *   Data Types
 ******************************************************************************/
// BOOL定义
enum
{
  MOMI_NOK = 0x00,
  MOMI_OK = 0x01
};

/******************************************************************************
 *   Function prototypes
 ******************************************************************************/
void Modem_SetState(modem_state_t state);
modem_state_t Modem_GetState(void);

void Momi_ServiceInit(void);
void Momi_ServiceStart(void);

#endif  /* _MOMI_PROTOCOL_H_ */

