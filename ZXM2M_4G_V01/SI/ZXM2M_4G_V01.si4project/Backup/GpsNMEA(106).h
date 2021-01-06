/*****************************************************************************
 * Copyright (c) 2020-2040 XGIT Limited. All rights reserved.
 * @FileName: GpsNMEA.h
 * @Engineer: TenYan
 * @version:  V1.0
 * @Date:     2020-10-20
 * @brief:    Basic parser for the NMEA0183 protocol
******************************************************************************/
#ifndef _GPS_NMEA_H_
#define _GPS_NMEA_H_

//-----头文件调用-------------------------------------------------------------
#include "types.h"

/******************************************************************************
 *   Macros
 ******************************************************************************/
#define NMEA_MIN_PARSEBUFF      (128)

#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

#define NMEA_MAXSAT         (12)
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

#define NMEA_DEF_LAT        (5001.2621)
#define NMEA_DEF_LON        (3613.0595)

/******************************************************************************
 *   User Macros
 ******************************************************************************/
#define NMEA_MSG_QUEUE_MAX_SIZE  1024
#define NMEA_MSG_HEAD  '$'         // 帧头
#define NMEA_MSG_TAIL   0x0D0A  // 帧尾
#define NMEA_TUD_KNOTS      (1.852)  /// Knots, kilometer / NMEA_TUD_KNOTS = knot

/******************************************************************************
 * Data Types
 ******************************************************************************/
// 循环缓冲队列
typedef struct
{
  uint16_t head;  // 队列头
  uint16_t tail;  // 队列尾
  uint8_t data[NMEA_MSG_QUEUE_MAX_SIZE];  // 数据缓冲区
}nmea_msg_queue_t;

// NMEA packets type which parsed and generated by library
enum
{
  NMEA_PACK_TYPE_NONE = 0x00, /// Unknown packet type.
  NMEA_PACK_TYPE_GGA = 0x01, /// GGA - Essential fix data which provide 3D location and accuracy data.
  NMEA_PACK_TYPE_GSA = 0x02, /// GSA - GPS receiver operating mode, SVs used for navigation, and DOP values.
  NMEA_PACK_TYPE_GSV = 0x04, /// GSV - Number of SVs in view, PRN numbers, elevation, azimuth & SNR values.
  NMEA_PACK_TYPE_RMC = 0x08, /// RMC - Recommended Minimum Specific GPS/TRANSIT Data.
  NMEA_PACK_TYPE_VTG = 0x10, /// VTG - Actual track made good and speed over ground.
};

/**
 * Information about satellite
 * @see nmeaSATINFO
 * @see nmeaGPGSV
 */
typedef struct
{
  int id;      /// Satellite PRN number
  int in_use;  /// Used in position fix
  int elv;     /// Elevation in degrees, 90 maximum
  int azimuth; /// Azimuth, degrees from true north, 000 to 359
  int sig;     /// Signal, 00-99 dB
} nmea_satellite_t;

/// Information about all satellites in view @see nav_info_t  @see nmea_gsv_t
typedef struct
{
  uint8_t inuse;      /// Number of satellites in use (not those in view)
  uint8_t inview;     /// Total number of satellites in view
  nmea_satellite_t sat[NMEA_MAXSAT]; /// Satellites information
} nmea_satinfo_t;

// Summary GPS information from all parsed packets, used also for generating NMEA stream
typedef struct
{
  utc_time_t utc;   /// UTC 日期和时间
  uint8_t fixValid;  /// 定位状态
  int32_t  lat;      /// 纬度 单位:百万分之一度
  uint8_t  ns;       /// [N]orth or [S]outh
  int32_t  lon;      /// 经度 单位:百万分之一度
  uint8_t  ew;       /// [E]ast or [W]est
  int16_t  alt;      /// 海拔高度(米),海平面以下为负数
  uint16_t speed;    /// 对地速度(0.1km/h) 0-2200
  uint16_t heading;  /// 对地真航向(单位为度) 0-360
  uint8_t numSvFix;  /// 用于定位的卫星数目
  uint8_t antStatus; /// NMEA语句天线的状态
} nav_info_t;
extern nav_info_t nav_data;

// GPS结构体
typedef struct
{
  uint8_t sleepStatus;    /// 休眠状态:0=正常,1=休眠
  uint8_t no_msg_recv_flag;  // 收到GPS消息标志:0=收到, 1=未收到
  uint8_t no_msg_recv_timer; // 未收到消息计时器
  uint8_t antStatus;      /// 天线状态:0=正常,1=异常
  uint8_t antShortStatus; /// 天线短路状态:0=正常,1=短路
  uint8_t antOpenStatus;  /// 天线开路状态:0=正常,1=开路
  uint8_t moduleStatus;   /// 模块状态:0=正常,1=异常
  uint8_t speedStatus;    /// 速度状态:0=正常,1=超速
}gps_context_t;
extern gps_context_t gps_context;

enum
{
  NMEA_NOK = 0x00,
  NMEA_OK = 0x01
};

enum
{
  NMEA_FALSE = 0x00,
  NMEA_TRUE = 0x01
};

enum
{
  GPS_OK = 0x00,
  GPS_NOK = 0x01
};

/******************************************************************************
 * Function prototypes
 ******************************************************************************/


#endif  /* _GPS_NMEA_H_ */

