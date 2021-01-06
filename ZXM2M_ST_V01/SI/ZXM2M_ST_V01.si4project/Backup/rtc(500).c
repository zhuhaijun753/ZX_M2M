/*******************************************************************************
 * Copyright (c) 2020-2040 XGIT Limited. All rights reserved.
 * @FileName: rtc.c
 * @Engineer: TenYan
 * @version:  V1.0
 * @Date:     2020-9-17
 * @brief:
 *******************************************************************************/
#include "rtc.h"
#include "PcDebug.h"
#include "stm32f2xx.h"
#include "main.h"

/******************************************************************************
 * Macros
 ******************************************************************************/
#define ONE_DAY_SECONDS      (86400U)
#define ONE_HOUR_SECONDS     (3600U)
#define ONE_MINUTE_SECONDS   (60U)

rtc_soft_t st_time;

/******************************************************************************
 * Return the number of seconds since January, 1 1970
 * Seconds is stored as an unsigned 32 bit number so it won't roll over until
 * 6:28:15 AM 7-Feb-2106
 * ���룺nʱ���ľ�������ʱ��
 * �����0ʱ���ı�׼����
******************************************************************************/
void RTC_ConvertDataTimeToSeconds(rtc_date_t* time, uint32_t* seconds)
{
  uint32_t temp;

  // Days per month: 31,28,31,30,31,30,31,31,30,31,30,31
  uint16_t days[] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

  temp = time->year + 2000; // Get the current year

  // Start with the number of days since 1970 assuming 365 days per year */
  (*seconds) = (temp - 1970) * 365;

  // Add leap year days
  (*seconds) += ((temp / 4) - (1970 / 4));

  // Add number of days till given month
  (*seconds) += days[time->month];

  /* Add days in given month*/
  (*seconds) += time->day-1;

  /* For leap year if month less than or equal to Febraury, decrement day counter*/
  if ((!(temp & 0x03)) && (time->month <= 0x02))
  {
    (*seconds)--;
  }

  (*seconds) = (*seconds) * 86400;   // ���������
  (*seconds) += (uint32_t)time->hour * 3600; // ����Сʱ����
  (*seconds) += (uint32_t)time->minute * 60;    // ������ӵ���
  (*seconds) += time->second;         // ������
  (*seconds)++;
}

/************************************************************************
 * ��ת��Ϊ�꣬�£��գ�ʱ���֣���(Start from 1970-01-01)
 * ���룺nʱ���ı�׼����
 * �����nʱ���ľ�������ʱ��
 ************************************************************************/
void RTC_ConvertSecondsToDateTime(uint32_t seconds, date_time_t* p_date_time)
{
  uint32_t days_left;
  uint16_t days_of_year;
  uint8_t days_of_month;

  // ��ת��Ϊʱ����
  days_left = seconds / ONE_DAY_SECONDS; // ��ȡ������
  seconds -= days_left*ONE_DAY_SECONDS;  // ����������

  p_date_time->hour = seconds / ONE_HOUR_SECONDS; // Сʱ(ע��ʱ��)
  seconds -= p_date_time->hour * ONE_HOUR_SECONDS;

  p_date_time->minute = seconds / ONE_MINUTE_SECONDS; // ����
  p_date_time->second = seconds - p_date_time->minute*ONE_MINUTE_SECONDS; // ��

  // ת����Ϊ������
  p_date_time->year = 1970; // ��1970-01-01��ʼ
  p_date_time->month = 1;
  p_date_time->day = 1;

  while (days_left > 0)
  {
    if (p_date_time->year & 0x03)
      days_of_year = 365;  // ������
    else
      days_of_year = 366; // ����(leap year)

    if (days_left < days_of_year)
    {
      while (days_left > 0) // ��ʼ������
      {
        switch (p_date_time->month)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
          days_of_month = 31;
          break;

        case 4:
        case 6:
        case 9:
        case 11:
          days_of_month = 30;
          break;

        case 2:
          days_of_month = (days_of_year == 365) ? 28 : 29;
          break;

        default:
          break;
        }

        if (days_left < days_of_month)
        {
          p_date_time->day = days_left + 1; // ��������
          days_left = 0;
        }
        else
        {
          days_left -= days_of_month;
          p_date_time->month++; // ������
        }
      }
    }
    else
    {
      days_left -= days_of_year;
      p_date_time->year++;  // ������
    }
  }
}

/******************************************************************************
 * ��ȡRTC����ʱ��
 ******************************************************************************/
rtc_date_t RTC_GetBjTime(void)
{
  rtc_date_t time;

  am18x5_GetDateTime(&time);
  return time;
}

/******************************************************************************
 * У׼RTCʱ��, ÿ��ϵͳ��λ��ֻУ׼һ��,ֻ�ڶ�λ���յ�ƽ̨��Уʱ����ʱִ��
 ******************************************************************************/
void RTC_CorrectTime(rtc_date_t date)
{
  rtc_date_t time;

  time = RTC_GetBjTime();
  if (date.year!=time.year || date.month!=time.month || date.day!=time.day ||
      date.hour!=time.hour || date.minute!=time.minute || date.second!=time.second)
  {
    am18x5_SetDateTime(&date);
    PcDebug_Printf("Correct RTC\r\n");
  }
}

/******************************************************************************
 * ��UTCʱ��ת��Ϊ����ʱ��
 * 1.����ǿ��Ա� 400����
 * 2.��ݿ��Ա�4�����������ǲ��Ҳ��ܱ�100������
 ******************************************************************************/
uint8_t RTC_CovertUtcToBjt(rtc_date_t *utc, rtc_date_t *bj)
{
  const uint8_t day_of_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

  if ((0==utc->year) && (0==utc->month) && (0==utc->day))
  {
    return FALSE;
  }

  *bj = *utc;

  if (utc->hour < 16)
  {
    bj->hour = utc->hour + 8;
    bj->day = utc->day;
  }
  else
  {
    bj->hour = utc->hour + 8 - 24;
    bj->day = utc->day + 1;
    if (((bj->year%400==0)||((bj->year%4==0)&&(bj->year%100!=0)))&&(bj->month==2)) // ����
    {
      if (bj->day > 29)
      {
        bj->day = 1;
        bj->month = 3;
      }
    }
    else if (bj->day > day_of_month[bj->month - 1])
    {
      bj->day = 1;
      if ((++bj->month) > 12)
      {
        bj->month = 1;
        bj->year++;
      }
    }
  }

  return TRUE;
}

/******************************************************************************
 * ������ʱ��ת��ΪUTCʱ��
 ******************************************************************************/
uint8_t RTC_ConvertBjToUtc(rtc_date_t *bj, rtc_date_t *utc)
{
  if ((bj->year==0) && (bj->month==0) && (bj->day==0))
  {
    return FALSE;
  }

  *utc = *bj;
  if (bj->hour < 8)
  {
    utc->hour = bj->hour + 16;
    utc->day  = bj->day - 1;
    if (((utc->year%400==0)||((utc->year%4==0)&&(utc->year%100!=0)))&&(utc->month==3))   // ����
    {
      if (utc->day==0)
      {
        utc->day = 29;
        utc->month = 2;
      }
    }
    else if (utc->day==0)
    {
      utc->day = 1;
      if ((--utc->month)==0)
      {
        utc->month = 12;
        utc->year--;
      }
    }
  }
  else
  {
    utc->hour = bj->hour - 8;
    utc->day  = bj->day;
  }

  return TRUE;
}

/******************************************************************************
 * ��ȡRTC��UTCʱ��
 ******************************************************************************/
rtc_date_t RTC_GetUtcTime(void)
{
  rtc_date_t time, utc;

  am18x5_GetDateTime(&time); // ��ȡRTC��ǰʱ��
  if (RTC_ConvertBjToUtc(&time, &utc))
  {
    return utc;
  }
  else
  {
    utc.year = 0;
    utc.month = 0;
    utc.weekday = 0;
    utc.day = 0;
    utc.hour = 0;
    utc.minute = 0;
    utc.second = 0;

    return utc;
  }
}

/******************************************************************************
 *
 ******************************************************************************/
void rtcSoft_Init(void)
{
  rtc_date_t rtc_time;
  
  rtc_time = RTC_GetBjTime(); // ��ȡRTCʱ��
  PcDebug_Printf("Tst:%d-%d-%d %d:%d:%d\n\r", rtc_time.year,rtc_time.month,rtc_time.day,rtc_time.hour,rtc_time.minute,rtc_time.second);
  st_time.year = rtc_time.year;
  st_time.month = rtc_time.month;
  st_time.day = rtc_time.day;
  st_time.hour = rtc_time.hour;
  st_time.minute = rtc_time.minute;
  st_time.second = rtc_time.second;
  st_time.ms = 0;
  st_time.ms_temp = 0;
}

//==ÿ10ms����һ��============================================================
void rtcSoft_Run(void)
{
  
  
}

/******************************************************************************
 *
 ******************************************************************************/
void rtc_Initialize(void)
{
  //iRTC_Initialize();
  am18x5_Initialize();
}

//-----�ļ�rtc.c����---------------------------------------------