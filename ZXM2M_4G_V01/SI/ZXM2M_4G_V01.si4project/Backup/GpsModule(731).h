/*****************************************************************************
 * Copyright (c) 2020-2040 XGIT Limited. All rights reserved.
 * @FileName: GpsModule.h
 * @Engineer: TenYan
 * @version   V1.0
 * @Date:     2020-10-11
 * @brief:    ���ļ�ΪGPS����ģ�����ж���ӿ��ļ�
******************************************************************************/
#ifndef _GPS_MODULE_H_
#define _GPS_MODULE_H_
#include "types.h"
#include "GpsNMEA.h"

/******************************************************************************
 *   Macros
 ******************************************************************************/
 #define GPS_RECV_TIMEOUT_SP  30   // 15��û���յ�GPSģ��������Ϊģ���쳣
 
#define nmeaMsgQueueENTER_CRITICAL() pthread_mutex_lock(&nmeaMsgQueueMutex)
#define nmeaMsgQueueEXIT_CRITICAL()  pthread_mutex_lock(&nmeaMsgQueueMutex)
extern pthread_mutex_t nmeaMsgQueueMutex;

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
void GPS_ServiceInit(void);
void GPS_ServiceStart(void);
void GPS_Do1sTasks(void);

// GPS��λ��Ϣ�����ӿ�
uint8_t GPS_GetSatelliteNum(void);
uint8_t GPS_GetPositioningStatus(void);
uint8_t GPS_GetEastWest(void);
uint8_t GPS_GetNorthSouth(void);
uint32_t GPS_GetLongitude(void);
uint32_t GPS_GetLatitude(void);
uint16_t GPS_GetSpeed(void);
uint16_t GPS_GetHeading(void);
int16_t GPS_GetHeight(void);
utc_time_t GPS_GetUtcTime(void);
uint8_t GPS_GetModuleStatus(void);
uint8_t GPS_GetAntennaStatus(void);
uint8_t GPS_GetAntShortStatus(void);
uint8_t GPS_GetAntOpenStatus(void);
uint8_t GPS_GetSpeedOverrunStatus(void);

#endif
