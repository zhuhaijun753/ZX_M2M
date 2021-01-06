/*****************************************************************************
* @FileName: tcw.c
* @Engineer: TenYan
* @Company:  徐工信息智能硬件部
* @version   V1.0
* @Date:     2019-10-29
* @brief     徐工重型起重机CAN通信协议(上车(CAN2)、下车(CAN1))
* @Device ID: 2181193799(全地面)  2181193800(轮式KXCT)
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include "config.h"
#include "tcw.h"


/******************************************************************************
* Define
******************************************************************************/



/******************************************************************************
 *   Function prototypes
 ******************************************************************************/



/******************************************************************************
* Data Types and Globals
******************************************************************************/
// TLV状态有效标志
bittype2 zxup_tlv_flag1;
bittype2 zxup_tlv_flag2;
bittype2 zxup_tlv_flag3;
bittype2 zxdown_tlv_flag1;
bittype2 zxdown_tlv_flag2;
bittype2 zxengine_tlv_flag;
bittype2 zxversion_tlv_flag;
bittype2 zxstatistics_tlv_flag;

// 数据缓存
uint8_t zxup_buffer[SIZE_OF_ZXUP_BUFFER]; /// 上车数据缓存
uint8_t zxdown_buffer[SIZE_OF_ZXDOWN_BUFFER]; /// 下车底盘数据缓存
uint8_t zxengine_buffer[SIZE_OF_ZXENGINE_BUFFER]; /// 下车发动机数据缓存
uint8_t zxstatistics_buffer[SIZE_OF_ZXSTATISTICS_BUFFER]; ///统计数据缓存
uint8_t zxversion_buffer[SIZE_OF_ZXVERSION_BUFFER]; /// 版本信息缓存

/*************************************************************************
 * 处理接收到的uCAN报文(上车)
*************************************************************************/
uint8_t CAN_ProcessRecvUpMsg(uint32_t canId, uint8_t *pdata, uint8_t size)
{
  uint8_t retval = 0x00;
  //uint32_t tempVal;

  switch (canId) // 上车通信
  {
  //==A5A0====================================================================================
  case 0x565:
    zxup_buffer[ZXUP_A5A0_POS1_ADDR] = pdata[0]; // 力限器配置
    zxup_buffer[ZXUP_A5A0_POS1_ADDR+1] = pdata[1];
    break;

  case 0x285:
    zxup_buffer[ZXUP_A5A0_POS2_ADDR] = pdata[1]; // 工况代码(ABCDEFGH)
    zxup_buffer[ZXUP_A5A0_POS2_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A0_POS2_ADDR+2] = pdata[2];
    zxup_buffer[ZXUP_A5A0_POS3_ADDR] = pdata[0]; // 倍率
    zxup_buffer[ZXUP_A5A0_POS6_ADDR] = pdata[4]; // 主臂长度
    zxup_buffer[ZXUP_A5A0_POS6_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A0_POS10_ADDR] = pdata[6]; // 副臂长度
    zxup_buffer[ZXUP_A5A0_POS10_ADDR+1] = pdata[7];
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x195:
    zxup_buffer[ZXUP_A5A0_POS4_ADDR] = pdata[4]; // 主臂头部角度
    zxup_buffer[ZXUP_A5A0_POS4_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A0_POS5_ADDR] = pdata[2]; // 主臂根部角度
    zxup_buffer[ZXUP_A5A0_POS5_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A0_POS15_ADDR] = pdata[6]; // 臂头高度
    zxup_buffer[ZXUP_A5A0_POS15_ADDR+1] = pdata[7];
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x183:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS8_ADDR], pdata, 8); // 节臂百分比
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x283:
    zxup_buffer[ZXUP_A5A0_POS9_ADDR] = pdata[0]; // 缸臂销锁死
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x295:
    zxup_buffer[ZXUP_A5A0_POS11_ADDR] = pdata[0]; // 副臂角度
    zxup_buffer[ZXUP_A5A0_POS11_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A0_POS12_ADDR] = pdata[2]; // 副臂/塔臂根部角度
    zxup_buffer[ZXUP_A5A0_POS12_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A0_POS13_ADDR] = pdata[4]; // 副臂/塔臂头部角度
    zxup_buffer[ZXUP_A5A0_POS13_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A2_POS1_ADDR] = pdata[6]; // 塔臂拉力左
    zxup_buffer[ZXUP_A5A2_POS1_ADDR+1] = pdata[7];
    tlv_a5a0_valid_flag = 1;
    tlv_a5a2_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x1D5:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS14_ADDR], pdata, 8); // 腔压力
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x185:
    zxup_buffer[ZXUP_A5A0_POS16_ADDR] = pdata[0]; // 额定重量
    zxup_buffer[ZXUP_A5A0_POS16_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A0_POS17_ADDR] = pdata[2]; // 实际重量
    zxup_buffer[ZXUP_A5A0_POS17_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A0_POS18_ADDR] = pdata[4]; // 力矩百分比
    zxup_buffer[ZXUP_A5A0_POS18_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A0_POS19_ADDR] = pdata[6]; // 工作幅度
    zxup_buffer[ZXUP_A5A0_POS19_ADDR+1] = pdata[7];
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x385:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS20_ADDR], pdata, 8); // LMI故障代码1-4
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x395:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS21_ADDR], pdata, 8); // LMI故障代码5-8
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x3F5:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS22_ADDR], pdata, 4); // 非对称故障代码
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x3E5:
    memcpy(&zxup_buffer[ZXUP_A5A0_POS23_ADDR], pdata, 4); // LMI运行时间
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x1C5:
    if (pdata[4]&BIT(0)) // 高度限位
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(0); // 清0

    zxup_buffer[ZXUP_A5A2_POS2_ADDR] = pdata[0]; // 塔臂拉力右
    zxup_buffer[ZXUP_A5A2_POS2_ADDR+1] = pdata[1];
    tlv_a5a0_valid_flag = 1;
    tlv_a5a2_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x463:
    if (pdata[2]&BIT(4)) // LMI强制
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(1); // 清0

    if (pdata[2]&BIT(0)) // 拆装工况
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(2); // 清0

    if (pdata[2]&BIT(2)) // 变幅起强制
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(3); // 清0

    if (pdata[2]&BIT(3)) // 过放强制
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(4); // 清0

    if (pdata[2]&BIT(1)) // 高度限位强制
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A0_POS24_ADDR] &= ~BIT(5); // 清0

    if (pdata[7]&BIT(7)) // 回油阻塞报警
      zxup_buffer[ZXUP_A5B3_POS3_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B3_POS3_ADDR] &= ~BIT(0); // 清0

    if (pdata[7]&BIT(6)) // 进油堵塞报警
      zxup_buffer[ZXUP_A5B3_POS3_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B3_POS3_ADDR] &= ~BIT(1); // 清0

    tlv_a5b3_valid_flag = 1;
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x263:
    zxup_buffer[ZXUP_A5A0_POS25_ADDR] = pdata[0]; // 水平仪X
    zxup_buffer[ZXUP_A5A0_POS25_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A0_POS26_ADDR] = pdata[2]; // 水平仪Y
    zxup_buffer[ZXUP_A5A0_POS26_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A0_POS27_ADDR] = pdata[4]; // 风速
    zxup_buffer[ZXUP_A5A0_POS27_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A0_POS28_ADDR] = pdata[6]; // 回转角度
    zxup_buffer[ZXUP_A5A0_POS28_ADDR+1] = pdata[7];
    tlv_a5a0_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A1====================================================================================
  case 0x1E3:
    zxup_buffer[ZXUP_A5A1_POS1_ADDR] = pdata[0]; // 左超起卷扬角度
    zxup_buffer[ZXUP_A5A1_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A1_POS5_ADDR] = pdata[2]; // 右超起卷扬角度
    zxup_buffer[ZXUP_A5A1_POS5_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A1_POS3_ADDR] = pdata[4]; // 左超起展开角度
    zxup_buffer[ZXUP_A5A1_POS3_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A1_POS7_ADDR] = pdata[6]; // 右超起展开角度
    zxup_buffer[ZXUP_A5A1_POS7_ADDR+1] = pdata[7];
    tlv_a5a1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x2E3:
    zxup_buffer[ZXUP_A5A1_POS2_ADDR] = pdata[0]; // 左超起拉力
    zxup_buffer[ZXUP_A5A1_POS2_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A1_POS6_ADDR] = pdata[2]; // 右超起拉力
    zxup_buffer[ZXUP_A5A1_POS6_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A1_POS13_ADDR] = pdata[4]; // 左超起张紧缸长度
    zxup_buffer[ZXUP_A5A1_POS13_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A1_POS14_ADDR] = pdata[6]; // 右超起张紧缸长度
    zxup_buffer[ZXUP_A5A1_POS14_ADDR+1] = pdata[7];
    tlv_a5a1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x4E3:
    zxup_buffer[ZXUP_A5A1_POS4_ADDR] = pdata[2]; // 左超起变幅角度
    zxup_buffer[ZXUP_A5A1_POS4_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A1_POS8_ADDR] = pdata[4]; // 右超起变幅角度
    zxup_buffer[ZXUP_A5A1_POS8_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5B1_POS3_ADDR] = pdata[6]; // 左超起张紧缸长度 *********
    zxup_buffer[ZXUP_A5B1_POS3_ADDR+1] = pdata[7];
    tlv_a5a1_valid_flag = 1;
    tlv_a5b1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x3E3:
    if (pdata[5]&BIT(0)) // 左超起卷扬锁止
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // 左超起卷扬解锁
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(4)) // 左超起起竖到位
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(2); // 清0

    if (pdata[6]&BIT(1)) // 左超起张紧缸解锁
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(5); // 清0

    if (pdata[6]&BIT(0)) // 左超起张紧缸锁止
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(6); // 清0

    if (pdata[6]&BIT(4)) // 左超起张紧缸全缩到底
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS9_ADDR] &= ~BIT(7); // 清0

    if (pdata[5]&BIT(2)) // 右超起卷扬锁止
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(3)) // 右超起卷扬解锁
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(5)) // 右超起起竖到位
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(2); // 清0

    if (pdata[6]&BIT(3)) // 右超起张紧缸解锁
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(5); // 清0

    if (pdata[6]&BIT(2)) // 右超起张紧缸锁止
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(6); // 清0

    if (pdata[6]&BIT(5)) // 右超起张紧缸全缩到底
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5A1_POS10_ADDR] &= ~BIT(7); // 清0

    zxup_buffer[ZXUP_A5A1_POS11_ADDR] = pdata[0]; // 左超起齿数
    zxup_buffer[ZXUP_A5A1_POS12_ADDR] = pdata[1]; // 右超起齿数

    if (pdata[7]&BIT(2)) // 左张紧拉力过小*********
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] &= ~BIT(0); // 清0

    if (pdata[7]&BIT(3)) // 左张紧拉力过大*********
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] &= ~BIT(1); // 清0

    if (pdata[7]&BIT(4)) // 右张紧拉力过小*********
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] &= ~BIT(2); // 清0

    if (pdata[7]&BIT(5)) // 右张紧拉力过大*********
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5B1_POS4_ADDR] &= ~BIT(3); // 清0

    zxup_buffer[ZXUP_A5B1_POS2_ADDR] = pdata[2]; // 超起目标张紧角度*********
    zxup_buffer[ZXUP_A5B1_POS2_ADDR+1] = pdata[3];
    tlv_a5a1_valid_flag = 1;
    tlv_a5b1_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A2====================================================================================
  case 0x3F3:
    zxup_buffer[ZXUP_A5A2_POS3_ADDR] = pdata[4]; // 防后倾压力
    zxup_buffer[ZXUP_A5A2_POS3_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A2_POS4_ADDR] = pdata[4]; // 前支架角度
    zxup_buffer[ZXUP_A5A2_POS4_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5BE_POS2_ADDR] = pdata[6]; // 卷筒转速*****
    zxup_buffer[ZXUP_A5BE_POS2_ADDR+1] = pdata[7];
    tlv_a5a2_valid_flag = 1;
    tlv_a5be_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x1F3:
    if (pdata[0]&BIT(3)) // 右超起卷扬锁止
      zxup_buffer[ZXUP_A5A2_POS5_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A2_POS5_ADDR] &= ~BIT(0); // 清0

    if (pdata[2]&BIT(0)) // 制动电磁阀
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] &= ~BIT(1); // 清0

    if (pdata[2]&BIT(1)) // 压力继电器
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] &= ~BIT(2); // 清0

    zxup_buffer[ZXUP_A5BE_POS1_ADDR] = pdata[6]; // 马达电流
    zxup_buffer[ZXUP_A5BE_POS1_ADDR+1] = pdata[7];
    tlv_a5a2_valid_flag = 1;
    tlv_a5be_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A5====================================================================================
  case 0x35A:
    zxup_buffer[ZXUP_A5A5_POS1_ADDR] = pdata[3]; // 发动机转速
    zxup_buffer[ZXUP_A5A5_POS1_ADDR+1] = pdata[4];
    zxup_buffer[ZXUP_A5A5_POS2_ADDR] = pdata[2]; // 实际扭矩百分比

    if (pdata[0]&BIT(0)) // 发动机扭矩模式(调速器类型)1
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] &= ~BIT(0); // 清0
      
    if (pdata[0]&BIT(1)) // 发动机扭矩模式(调速器类型)2
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(2)) // 发动机扭矩模式(调速器类型)3
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(3)) // 发动机扭矩模式(调速器类型)4
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS16_ADDR] &= ~BIT(3); // 清0

    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x35B:
    zxup_buffer[ZXUP_A5A5_POS3_ADDR] = pdata[0]; // 摩擦扭矩百分比
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x35C:
    zxup_buffer[ZXUP_A5A5_POS4_ADDR] = pdata[1]; // 进气歧管压力
    zxup_buffer[ZXUP_A5A5_POS5_ADDR] = pdata[2]; // 进气歧管温度
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x35D:
    zxup_buffer[ZXUP_A5A5_POS6_ADDR] = pdata[0]; // 冷却液温度
    zxup_buffer[ZXUP_A5A5_POS7_ADDR] = pdata[2]; // 机油温度
    zxup_buffer[ZXUP_A5A5_POS7_ADDR+1] = pdata[3];
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x35E:
    if (pdata[0]&BIT(0)) // 油中有水指示灯1
      zxup_buffer[ZXUP_A5A5_POS11_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS11_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 油中有水指示灯2
      zxup_buffer[ZXUP_A5A5_POS11_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A5_POS11_ADDR] &= ~BIT(1); // 清0

    zxup_buffer[ZXUP_A5A5_POS8_ADDR] = pdata[2]; // 机油液位
    zxup_buffer[ZXUP_A5A5_POS9_ADDR] = pdata[3]; // 机油压力
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x35F:
    zxup_buffer[ZXUP_A5A5_POS10_ADDR] = pdata[0]; // 发动机总运行时间
    zxup_buffer[ZXUP_A5A5_POS10_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A5_POS10_ADDR+2] = pdata[2];
    zxup_buffer[ZXUP_A5A5_POS10_ADDR+3] = pdata[3];
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x36A:
    zxup_buffer[ZXUP_A5A5_POS12_ADDR] = pdata[1]; // 油门踏板百分比
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x36B:
    zxup_buffer[ZXUP_A5A5_POS13_ADDR] = pdata[0]; // 发动机燃油消耗率
    zxup_buffer[ZXUP_A5A5_POS13_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A5_POS14_ADDR] = pdata[4]; // 发动机平均燃油消耗率
    zxup_buffer[ZXUP_A5A5_POS14_ADDR+1] = pdata[5];
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x36C:
    zxup_buffer[ZXUP_A5A5_POS15_ADDR] = pdata[1]; // 燃油液位
    tlv_a5a5_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A6====================================================================================
  case 0x464:
    if (pdata[1]&BIT(0)) // 左手柄中位
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(1)) // 左手柄左开关
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(5); // 清0

    if (pdata[1]&BIT(2)) // 左手柄右开关
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(6); // 清0

    if (pdata[1]&BIT(3)) // 左手柄先导开关
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(7); // 清0

    if (pdata[1]&BIT(4)) // 右手柄中位
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(5)) // 右手柄左开关
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(5); // 清0

    if (pdata[1]&BIT(6)) // 右手柄右开关
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(6); // 清0

    if (pdata[1]&BIT(7)) // 右手柄先导开关
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(7); // 清0

    zxup_buffer[ZXUP_A5B3_POS2_ADDR] = pdata[2];
    tlv_a5a6_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x4E4:
    if (pdata[0]&BIT(4)) // 左手柄X正向
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(5)) // 左手柄X负向
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(6)) // 左手柄Y正向
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(3); // 清0

    if (pdata[0]&BIT(7)) // 左手柄Y负向
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS1_ADDR] &= ~BIT(4); // 清0

    if (pdata[0]&BIT(0)) // 右手柄X正向
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(1)) // 右手柄X负向
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(2)) // 右手柄Y正向
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(3); // 清0

    if (pdata[0]&BIT(3)) // 右手柄Y负向
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5A6_POS4_ADDR] &= ~BIT(4); // 清0

    tlv_a5a6_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x363:
    zxup_buffer[ZXUP_A5A6_POS2_ADDR] = pdata[4]; // 左手柄X输出
    zxup_buffer[ZXUP_A5A6_POS2_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5A6_POS3_ADDR] = pdata[6]; // 左手柄Y输出
    zxup_buffer[ZXUP_A5A6_POS3_ADDR+1] = pdata[7];
    zxup_buffer[ZXUP_A5A6_POS5_ADDR] = pdata[0]; // 右手柄X输出
    zxup_buffer[ZXUP_A5A6_POS5_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A6_POS6_ADDR] = pdata[2]; // 右手柄Y输出
    zxup_buffer[ZXUP_A5A6_POS6_ADDR+1] = pdata[3];
    tlv_a5a6_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A7====================================================================================
  case 0x561:
    zxup_buffer[ZXUP_A5A7_POS1_ADDR] = pdata[0]; // 产品配置(1-4)
    zxup_buffer[ZXUP_A5A7_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5A7_POS1_ADDR+2] = pdata[2];
    zxup_buffer[ZXUP_A5A7_POS1_ADDR+2] = pdata[3];
    zxversion_buffer[ZXVERSION_A505_POS3_ADDR] = pdata[4];  // 显示器底层版本
    zxversion_buffer[ZXVERSION_A505_POS3_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A505_POS3_ADDR+2] = pdata[6];
    tlv_a505_valid_flag = 1;
    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;
  
  case 0x181:
    zxup_buffer[ZXUP_A5A7_POS2_ADDR] = pdata[1]; // 工况代码(ABCDEF)
    zxup_buffer[ZXUP_A5A7_POS2_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5A7_POS2_ADDR+2] = pdata[2];
    zxup_buffer[ZXUP_A5A7_POS11_ADDR] = pdata[4]; // 目标组合
    zxup_buffer[ZXUP_A5A7_POS14_ADDR] = pdata[5]; // 伸缩模式
    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x3A1:
    zxup_buffer[ZXUP_A5A7_POS3_ADDR] = pdata[0]; // 主卷起速度
    zxup_buffer[ZXUP_A5A7_POS4_ADDR] = pdata[6]; // 主卷落速度
    zxup_buffer[ZXUP_A5A7_POS5_ADDR] = pdata[4]; // 副卷起速度
    zxup_buffer[ZXUP_A5A7_POS6_ADDR] = pdata[5]; // 副卷落速度
    zxup_buffer[ZXUP_A5A7_POS7_ADDR] = pdata[2]; // 变幅起速度
    zxup_buffer[ZXUP_A5A7_POS8_ADDR] = pdata[3]; // 变幅落速度
    zxup_buffer[ZXUP_A5A7_POS9_ADDR] = pdata[1]; // 左回转速度
    zxup_buffer[ZXUP_A5A7_POS10_ADDR] = pdata[7]; // 右回转速度
    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x191:
    if (pdata[0]&BIT(0)) // 卷扬随动1
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(1)) // 卷扬随动2
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(4)) // 主/副卷随动
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(3); // 清0

    if (pdata[0]&BIT(3)) // 变幅补偿
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(4); // 清0

    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x482:
    if (pdata[2]&BIT(0)) // 取力使能
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(0); // 清0

    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x291:
    if (pdata[0]&BIT(0)) // 回转防摆
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(5); // 清0

    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x381:
    if (pdata[3]&BIT(0)) // 塔臂拆装工况1
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(6); // 清0

    if (pdata[3]&BIT(1)) // 塔臂拆装工况2
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS12_ADDR] &= ~BIT(7); // 清0

    if (pdata[3]&BIT(2)) // 塔臂工况变换1
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] &= ~BIT(0); // 清0

    if (pdata[3]&BIT(3)) // 塔臂工况变换2
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] &= ~BIT(1); // 清0

    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x391:
    if (pdata[0]&BIT(0)) // 塔臂自动
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5A7_POS13_ADDR] &= ~BIT(2); // 清0

    tlv_a5a7_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A8====================================================================================
  case 0x382:
    memcpy(&zxup_buffer[ZXUP_A5A8_POS1_ADDR], pdata, 8); // 超起操控
    zxup_buffer[ZXUP_A5B1_POS1_ADDR] = pdata[3]; // 超起目标展开角度
    tlv_a5a8_valid_flag = 1;
    tlv_a5b1_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x4C1:
    zxup_buffer[ZXUP_A5A8_POS2_ADDR] = pdata[0]; // 超起维修1+2
    zxup_buffer[ZXUP_A5A8_POS2_ADDR+1] = pdata[1];
    tlv_a5a8_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A9====================================================================================
  case 0x1B1:
    zxup_buffer[ZXUP_A5A9_POS1_ADDR] = pdata[0]; // 面板1
    zxup_buffer[ZXUP_A5A9_POS1_ADDR+1] = pdata[1];
    tlv_a5a9_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x1B2:
    zxup_buffer[ZXUP_A5A9_POS2_ADDR] = pdata[0]; // 面板2
    zxup_buffer[ZXUP_A5A9_POS2_ADDR+1] = pdata[1];
    tlv_a5a9_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x1B3:
    zxup_buffer[ZXUP_A5A9_POS2_ADDR] = pdata[0]; // 面板3
    zxup_buffer[ZXUP_A5A9_POS2_ADDR+1] = pdata[1];
    tlv_a5a9_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AA====================================================================================
  case 0x188:
    memcpy(&zxup_buffer[ZXUP_A5AA_POS1_ADDR], pdata, 8); // Msg1
    tlv_a5aa_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x288:
    memcpy(&zxup_buffer[ZXUP_A5AA_POS2_ADDR], pdata, 8); // Msg2
    tlv_a5aa_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x388:
    memcpy(&zxup_buffer[ZXUP_A5AA_POS3_ADDR], pdata, 8); // Msg3
    tlv_a5aa_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x488:
    memcpy(&zxup_buffer[ZXUP_A5AA_POS4_ADDR], pdata, 8); // Msg4
    tlv_a5aa_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AB====================================================================================
  case 0x343:
    memcpy(&zxup_buffer[ZXUP_A5AB_POS1_ADDR], pdata, 8); // 节点状态
    tlv_a5ab_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AC====================================================================================
  case 0x493:
    memcpy(&zxup_buffer[ZXUP_A5AC_POS1_ADDR], pdata, 6); // 伸缩限制和解除
    zxup_buffer[ZXUP_A5B2_POS3_ADDR] = pdata[6]; // 塔臂限制伸臂***********
    zxup_buffer[ZXUP_A5B2_POS4_ADDR] = pdata[7]; // 塔臂限制缩臂***********
    tlv_a5ac_valid_flag = 1;
    tlv_a5b2_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AD====================================================================================
  case 0x4C3:
    memcpy(&zxup_buffer[ZXUP_A5AD_POS1_ADDR], pdata, 8); // 起落限制和解除
    zxup_buffer[ZXUP_A5B2_POS5_ADDR] = pdata[6]; // 塔臂限制变幅起***********
    zxup_buffer[ZXUP_A5B2_POS6_ADDR] = pdata[7]; // 塔臂限制变幅落***********
    tlv_a5ad_valid_flag = 1;
    tlv_a5b2_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AE====================================================================================
  case 0x4B3:
    zxup_buffer[ZXUP_A5AE_POS1_ADDR] = pdata[0]; // 左回限制1
    zxup_buffer[ZXUP_A5AE_POS2_ADDR] = pdata[4]; // 左回限制2
    zxup_buffer[ZXUP_A5AE_POS3_ADDR] = pdata[1]; // 左回解除
    zxup_buffer[ZXUP_A5AE_POS4_ADDR] = pdata[2]; // 右回限制1
    zxup_buffer[ZXUP_A5AE_POS5_ADDR] = pdata[5]; // 右回限制2
    zxup_buffer[ZXUP_A5AE_POS6_ADDR] = pdata[3]; // 右回解除
    tlv_a5ae_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5AF====================================================================================
  case 0x4A3:
    memcpy(&zxup_buffer[ZXUP_A5AF_POS1_ADDR], pdata, 5); // 主卷扬起落限制和解除
    zxup_buffer[ZXUP_A5B2_POS7_ADDR] = pdata[5]; // 塔臂限制主卷起***********
    zxup_buffer[ZXUP_A5B2_POS8_ADDR] = pdata[6]; // 塔臂限制主卷落***********
    tlv_a5af_valid_flag = 1;
    tlv_a5b2_valid_flag = 1;
    retval = 0x01;
    break;

    //==A5B0====================================================================================
  case 0x4D3:
    memcpy(&zxup_buffer[ZXUP_A5B0_POS1_ADDR], &pdata[3], 4); // 副卷扬起落限制和解除
    tlv_a5b0_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B1====================================================================================
  // zxup_buffer

  //==A5B2====================================================================================
  case 0x4F3:
    zxup_buffer[ZXUP_A5B2_POS1_ADDR] = pdata[0]; // 塔卷起限制
    zxup_buffer[ZXUP_A5B2_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B2_POS2_ADDR] = pdata[2]; // 塔卷落限制
    zxup_buffer[ZXUP_A5B2_POS2_ADDR+1] = pdata[3];
    tlv_a5b2_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B3====================================================================================
  case 0x564:
    zxup_buffer[ZXUP_A5B3_POS1_ADDR] = pdata[0]; // 液压油温度
    zxup_buffer[ZXUP_A5B3_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B5_POS10_ADDR] = pdata[4]; // LS1压力*****
    zxup_buffer[ZXUP_A5B5_POS10_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5B5_POS12_ADDR] = pdata[6]; // LS2压力*****
    zxup_buffer[ZXUP_A5B5_POS12_ADDR+1] = pdata[7];
    tlv_a5b3_valid_flag = 1;
    tlv_a5b5_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B4====================================================================================
  case 0x1A3:
    zxup_buffer[ZXUP_A5B4_POS1_ADDR] = pdata[0]; // 1#主泵电磁阀
    zxup_buffer[ZXUP_A5B4_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B4_POS2_ADDR] = pdata[2]; // 2#主泵电磁阀
    zxup_buffer[ZXUP_A5B4_POS2_ADDR+1] = pdata[3];

    zxup_buffer[ZXUP_A5B5_POS5_ADDR] = pdata[0]; // 主卷起电磁阀*****
    zxup_buffer[ZXUP_A5B5_POS5_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B5_POS6_ADDR] = pdata[2]; // 主卷落电磁阀*****
    zxup_buffer[ZXUP_A5B5_POS6_ADDR+1] = pdata[3];

    zxup_buffer[ZXUP_A5BB_POS1_ADDR] = pdata[0]; // 起升电磁阀*****
    zxup_buffer[ZXUP_A5BB_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5BB_POS2_ADDR] = pdata[2]; // 下落电磁阀*****
    zxup_buffer[ZXUP_A5BB_POS2_ADDR+1] = pdata[3];

    zxup_buffer[ZXUP_A5BC_POS1_ADDR] = pdata[4]; // 马达电流
    zxup_buffer[ZXUP_A5BC_POS1_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5BC_POS2_ADDR] = pdata[6]; // 卷筒转速
    zxup_buffer[ZXUP_A5BC_POS2_ADDR+1] = pdata[7];
    tlv_a5b4_valid_flag = 1;
    tlv_a5b5_valid_flag = 1;
    tlv_a5bb_valid_flag = 1;
    tlv_a5bc_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x2A3:
    zxup_buffer[ZXUP_A5B4_POS3_ADDR] = pdata[0]; // 主泵压力
    zxup_buffer[ZXUP_A5B4_POS3_ADDR+1] = pdata[1];

    zxup_buffer[ZXUP_A5B5_POS9_ADDR] = pdata[0]; // MP1压力*****
    zxup_buffer[ZXUP_A5B5_POS9_ADDR+1] = pdata[1];

    zxup_buffer[ZXUP_A5BB_POS3_ADDR] = pdata[0]; // 油泵压力*****
    zxup_buffer[ZXUP_A5BB_POS3_ADDR+1] = pdata[1];

    if (pdata[6]&BIT(0)) // 制动电磁阀*****
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] &= ~BIT(1); // 清0

    if (pdata[6]&BIT(1)) // 压力继电器*****
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] &= ~BIT(2); // 清0

    tlv_a5b4_valid_flag = 1;
    tlv_a5b5_valid_flag = 1;
    tlv_a5bb_valid_flag = 1;
    tlv_a5bc_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B5====================================================================================
  case 0x393:
    zxup_buffer[ZXUP_A5B5_POS1_ADDR] = pdata[0]; // 伸电磁阀
    zxup_buffer[ZXUP_A5B5_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B5_POS2_ADDR] = pdata[2]; // 缩电磁阀
    zxup_buffer[ZXUP_A5B5_POS2_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5B5_POS11_ADDR] = pdata[4]; // MP2压力
    zxup_buffer[ZXUP_A5B5_POS11_ADDR+1] = pdata[5];
    zxup_buffer[ZXUP_A5B7_POS1_ADDR] = pdata[6]; // 伸缩缸压力*******
    zxup_buffer[ZXUP_A5B7_POS1_ADDR+1] = pdata[7];
    tlv_a5b5_valid_flag = 1;
    tlv_a5b7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x1C3:
    zxup_buffer[ZXUP_A5B5_POS3_ADDR] = pdata[0]; // 变幅起电磁阀
    zxup_buffer[ZXUP_A5B5_POS3_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B5_POS4_ADDR] = pdata[2]; // 变幅落电磁阀
    zxup_buffer[ZXUP_A5B5_POS4_ADDR+1] = pdata[3];
    tlv_a5b5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x3D3:
    zxup_buffer[ZXUP_A5B5_POS7_ADDR] = pdata[0]; // 副卷落电磁阀
    zxup_buffer[ZXUP_A5B5_POS7_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5B5_POS8_ADDR] = pdata[2]; // 副卷落电磁阀
    zxup_buffer[ZXUP_A5B5_POS8_ADDR+1] = pdata[3];
    tlv_a5b5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x443:
    if (pdata[3]&BIT(3)) // 合流电磁阀
      zxup_buffer[ZXUP_A5B5_POS13_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B5_POS13_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(0)) // 马达失速检测*****
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5BC_POS3_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 马达失速检测*****
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5BE_POS3_ADDR] &= ~BIT(0); // 清0

    tlv_a5b5_valid_flag = 1;
    tlv_a5bc_valid_flag = 1;
    tlv_a5be_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B6====================================================================================
  case 0x264:
    if (pdata[0]&BIT(0)) // 32MPa压力
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 24MPa压力1
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(1); // 清0
      
    if (pdata[0]&BIT(2)) // 24MPa压力2
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(3); // 清0
      
    if (pdata[0]&BIT(3)) // 1Mpa压力
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(4); // 清0

    tlv_a5b6_valid_flag = 1;
    retval = 0x01;

    break;

  case 0x193:
    if (pdata[5]&BIT(5)) // 伸电磁阀
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(2); // 清0

    if (pdata[5]&BIT(6)) // 缩电磁阀
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(5); // 清0

    ///////////////////////////////////////////////////////////////////////
    if (pdata[0]&BIT(0)) // 后缸销锁死
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 后缸销解锁
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(5)) // 后臂销锁死
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(4)) // 后臂销解锁
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(3); // 清0

    if (pdata[4]&BIT(0)) // 前缸销锁死
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(4); // 清0

    if (pdata[4]&BIT(1)) // 前缸销解锁
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(5); // 清0

    if (pdata[4]&BIT(5)) // 前臂销锁死
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(6); // 清0

    if (pdata[4]&BIT(4)) // 前臂销解锁
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS1_ADDR] &= ~BIT(7); // 清0

    if (pdata[0]&BIT(6)) // 前缸头体标志
      zxup_buffer[ZXUP_A5B8_POS2_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS2_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(7)) // 后缸头体标志
      zxup_buffer[ZXUP_A5B8_POS2_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B8_POS2_ADDR] &= ~BIT(1); // 清0

    zxup_buffer[ZXUP_A5B8_POS5_ADDR] = pdata[1];

    if (pdata[5]&BIT(0)) // 缸臂销供油电磁阀
      zxup_buffer[ZXUP_A5B9_POS1_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5B9_POS1_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // 缸臂销切换电磁阀
      zxup_buffer[ZXUP_A5B9_POS1_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5B9_POS1_ADDR] &= ~BIT(1); // 清0

    tlv_a5b6_valid_flag = 1;
    tlv_a5b8_valid_flag = 1;
    tlv_a5b9_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x2C3:
    if (pdata[2]&BIT(0)) // 变幅起电磁阀
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(6); // 清0

    if (pdata[2]&BIT(3)) // 变幅动力下落阀
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5B6_POS1_ADDR] &= ~BIT(7); // 清0

    zxup_buffer[ZXUP_A5BA_POS1_ADDR] = pdata[6]; // 左变幅平衡阀电流
    zxup_buffer[ZXUP_A5BA_POS1_ADDR+1] = pdata[7];
    tlv_a5b6_valid_flag = 1;
    tlv_a5ba_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B7====================================================================================
  case 0x293:
    zxup_buffer[ZXUP_A5B7_POS2_ADDR] = pdata[2]; // 伸缩缸长度
    zxup_buffer[ZXUP_A5B7_POS2_ADDR+1] = pdata[3];
    tlv_a5b7_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x394:
    zxup_buffer[ZXUP_A5B7_POS3_ADDR] = pdata[6]; // 电控平衡阀
    zxup_buffer[ZXUP_A5B7_POS3_ADDR+1] = pdata[7];
    tlv_a5b7_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B8====================================================================================
  case 0x1E9:
    zxup_buffer[ZXUP_A5B8_POS3_ADDR] = pdata[0]; // 前缸头体检测开关8
    zxup_buffer[ZXUP_A5B8_POS4_ADDR] = pdata[1]; // 后缸头体检测开关8
    zxup_buffer[ZXUP_A5B9_POS1_ADDR] = pdata[6]; // 蓄能器压力
    zxup_buffer[ZXUP_A5B9_POS1_ADDR+1] = pdata[7];
    tlv_a5b8_valid_flag = 1;
    tlv_a5b9_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5B9====================================================================================

  //==A5BA====================================================================================
  case 0x3C3:
    zxup_buffer[ZXUP_A5BA_POS2_ADDR] = pdata[0]; // 右变幅平衡阀电流
    zxup_buffer[ZXUP_A5BA_POS2_ADDR+1] = pdata[1];
    tlv_a5ba_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5BB====================================================================================
  //==A5BC====================================================================================

  //==A5BD====================================================================================
  case 0x2F3:
    zxup_buffer[ZXUP_A5BD_POS1_ADDR] = pdata[0]; // 起升电磁阀
    zxup_buffer[ZXUP_A5BD_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5BD_POS2_ADDR] = pdata[2]; // 下落电磁阀
    zxup_buffer[ZXUP_A5BD_POS2_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5BD_POS3_ADDR] = pdata[4]; // 油泵压力
    zxup_buffer[ZXUP_A5BD_POS3_ADDR+1] = pdata[5];
    tlv_a5bd_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5BE====================================================================================
  // zxup_buffer[ZXUP_A5BE_POS1_ADDR]

  //==A5BF====================================================================================
  case 0x2B3:
    zxup_buffer[ZXUP_A5BF_POS1_ADDR] = pdata[0]; // 左回转电磁阀
    zxup_buffer[ZXUP_A5BF_POS1_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5BF_POS2_ADDR] = pdata[2]; // 右回转电磁阀
    zxup_buffer[ZXUP_A5BF_POS2_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5BF_POS4_ADDR] = pdata[6]; // 油泵压力（回转压力检测）
    zxup_buffer[ZXUP_A5BF_POS4_ADDR+1] = pdata[7];
    tlv_a5bf_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5C0====================================================================================
  case 0x3B3:
    if (pdata[0]&BIT(0)) // 制动控制阀
      zxup_buffer[ZXUP_A5C0_POS1_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C0_POS1_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 压力检测
      zxup_buffer[ZXUP_A5C0_POS1_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C0_POS1_ADDR] &= ~BIT(1); // 清0

    zxup_buffer[ZXUP_A5C0_POS2_ADDR] = pdata[6]; // 回转制动阀-工作压力
    zxup_buffer[ZXUP_A5C0_POS2_ADDR+1] = pdata[7];
    tlv_a5c0_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5C1====================================================================================
  case 0x364:
    zxup_buffer[ZXUP_A5C1_POS1_ADDR] = pdata[4];
    zxup_buffer[ZXUP_A5C1_POS2_ADDR] = pdata[5];
    tlv_a5c1_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5C2====================================================================================
  case 0x3E4:
    zxup_buffer[ZXUP_A5C2_POS1_ADDR] = pdata[6];  // 压力选择
    zxup_buffer[ZXUP_A5C2_POS1_ADDR+1] = pdata[7];

    if (pdata[5]&BIT(0)) // 切换阀（Y35）
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // 切换阀Ａ(Y33A)
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(2)) // 切换阀Ｂ(Y33B)
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5C2_POS2_ADDR] &= ~BIT(2); // 清0
    
    tlv_a5c2_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5C3====================================================================================
  //==A5C4====================================================================================
  case 0x273:
    if (pdata[0]&BIT(0)) // 左-超起变幅起
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 左-超起变幅落
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[0]&BIT(2)) // 左-超起展开
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[0]&BIT(3)) // 左-超起收回
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(3); // 清0
    
    zxup_buffer[ZXUP_A5C3_POS1_ADDR] = pdata[2]; // 左-卷扬起
    zxup_buffer[ZXUP_A5C3_POS1_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5C3_POS2_ADDR] = pdata[6]; // 左-卷扬落
    zxup_buffer[ZXUP_A5C3_POS2_ADDR+1] = pdata[7];
    zxup_buffer[ZXUP_A5C3_POS7_ADDR] = pdata[4]; // 左-张紧缸缩
    zxup_buffer[ZXUP_A5C3_POS7_ADDR+1] = pdata[5];
    tlv_a5c3_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x373:
    if (pdata[0]&BIT(7)) // 左-卷扬压力选择
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(4); // 清0

    if (pdata[1]&BIT(3)) // 左-卷扬制动
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[1]&BIT(1)) // 左-棘轮解锁
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(6); // 清0
    
    if (pdata[1]&BIT(0)) // 左-棘轮上锁
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS4_ADDR] &= ~BIT(7); // 清0
    
    if (pdata[1]&BIT(2)) // 左-张紧缸解锁
      zxup_buffer[ZXUP_A5C3_POS5_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS5_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[1]&BIT(4)) // 左-超起卷扬浮动
      zxup_buffer[ZXUP_A5C3_POS5_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C3_POS5_ADDR] &= ~BIT(1); // 清0
    
    zxup_buffer[ZXUP_A5C4_POS1_ADDR] = pdata[2]; // 右-卷扬起
    zxup_buffer[ZXUP_A5C4_POS1_ADDR+1] = pdata[3];
    zxup_buffer[ZXUP_A5C4_POS2_ADDR] = pdata[6]; // 右-卷扬落
    zxup_buffer[ZXUP_A5C4_POS2_ADDR+1] = pdata[7];
    zxup_buffer[ZXUP_A5C4_POS7_ADDR] = pdata[4]; // 右-张紧缸缩
    zxup_buffer[ZXUP_A5C4_POS7_ADDR+1] = pdata[5];
    
    if (pdata[0]&BIT(0)) // 右-超起变幅起
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // 右-超起变幅落
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[0]&BIT(2)) // 右-超起展开
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(2); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[0]&BIT(3)) // 右-超起收回
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(3); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(3); // 清0

    if (pdata[0]&BIT(7)) // 右-卷扬压力选择
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(4); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(4); // 清0

    if (pdata[1]&BIT(3)) // 右-卷扬制动
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(5); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[1]&BIT(1)) // 右-棘轮解锁
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(6); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(6); // 清0
    
    if (pdata[1]&BIT(0)) // 右-棘轮上锁
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] |= BIT(7); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS4_ADDR] &= ~BIT(7); // 清0
    
    if (pdata[1]&BIT(2)) // 右-张紧缸解锁
      zxup_buffer[ZXUP_A5C4_POS5_ADDR] |= BIT(0); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS5_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[1]&BIT(4)) // 右-超起卷扬浮动
      zxup_buffer[ZXUP_A5C4_POS5_ADDR] |= BIT(1); // 置1
    else
      zxup_buffer[ZXUP_A5C4_POS5_ADDR] &= ~BIT(1); // 清0
    
    tlv_a5c3_valid_flag = 1;
    tlv_a5c4_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x504:
    zxup_buffer[ZXUP_A5C3_POS3_ADDR] = pdata[0]; // 左-超起马达变量
    zxup_buffer[ZXUP_A5C3_POS3_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5C4_POS3_ADDR] = pdata[2]; // 右-超起马达变量
    zxup_buffer[ZXUP_A5C4_POS3_ADDR+1] = pdata[3];
    tlv_a5c3_valid_flag = 1;
    tlv_a5c4_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x473:
    zxup_buffer[ZXUP_A5C3_POS6_ADDR] = pdata[0]; // 左-张紧缸伸
    zxup_buffer[ZXUP_A5C3_POS6_ADDR+1] = pdata[1];
    zxup_buffer[ZXUP_A5C4_POS6_ADDR] = pdata[2]; // 右-张紧缸伸
    zxup_buffer[ZXUP_A5C4_POS6_ADDR+1] = pdata[3];
    tlv_a5c3_valid_flag = 1;
    tlv_a5c4_valid_flag = 1;
    retval = 0x01;
    break;

  //==上车系统版本====================================================================================
  case 0x562:
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR] = pdata[0];  // 显示器2底层
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR+1] = pdata[1];
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR+2] = pdata[2];
    tlv_a505_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x571:
    zxversion_buffer[ZXVERSION_A505_POS2_ADDR] = pdata[4];  // 显示器1
    zxversion_buffer[ZXVERSION_A505_POS2_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A505_POS2_ADDR+2] = pdata[6];
    tlv_a505_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x572:
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR] = pdata[4];  // 显示器2
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A505_POS6_ADDR+2] = pdata[6];
    tlv_a505_valid_flag = 1;
    retval = 0x00;
    break;

  case 0x573:
    zxversion_buffer[ZXVERSION_A505_POS4_ADDR] = pdata[4];  // 控制器
    zxversion_buffer[ZXVERSION_A505_POS4_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A505_POS4_ADDR+2] = pdata[6];
    tlv_a505_valid_flag = 1;
    retval = 0x00;
    break;
    
  case 0x575:
    zxversion_buffer[ZXVERSION_A505_POS1_ADDR] = pdata[4];  // 力矩限制器
    zxversion_buffer[ZXVERSION_A505_POS1_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A505_POS1_ADDR+2] = pdata[6];
    tlv_a505_valid_flag = 1;
    retval = 0x01;
    break;

  default:
    break;
  }

  return retval;
}

/*************************************************************************
 * 处理接收到的dCAN报文(下车)
*************************************************************************/
uint8_t CAN_ProcessRecvDownMsg(uint32_t canId, uint8_t *pdata, uint8_t size)
{
  uint8_t retval = 0x00;
  //uint32_t tempVal;

  switch (canId) // 下车通信
  {
  //==A5A3====================================================================================
  case 0x24D:
    memcpy(&zxdown_buffer[ZXDOWN_A5A3_POS1_ADDR], pdata, 8); // 水平支腿长度
    tlv_a5a3_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x22A:
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR] = pdata[0];   // 左前支腿压力
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+1] = pdata[1];
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+2] = pdata[2]; // 右前支腿压力
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+3] = pdata[3];
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+4] = pdata[4]; // 左后支腿压力
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+5] = pdata[5];
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+6] = pdata[6]; // 右后支腿压力
    zxdown_buffer[ZXDOWN_A5A3_POS2_ADDR+7] = pdata[7];
    tlv_a5a3_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x24E:
    memcpy(&zxdown_buffer[ZXDOWN_A5A3_POS3_ADDR], pdata, 8); // 摆缸长度
    tlv_a5a3_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5A4====================================================================================
  case 0x24B:
    if (pdata[1]&BIT(4)) // 液压离合器状态
      zxdown_buffer[ZXDOWN_A5EA_POS2_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5EA_POS2_ADDR] &= ~BIT(1); // 清0
    
    zxdown_buffer[ZXDOWN_A5A4_POS1_ADDR] = pdata[0]; // 辅助支腿状态
    zxdown_buffer[ZXDOWN_A5E9_POS1_ADDR] = pdata[6]; // 液压油温度
    zxdown_buffer[ZXDOWN_A5EA_POS8_ADDR] = pdata[2]; // 散热器液压驱动泵控制电流
    zxdown_buffer[ZXDOWN_A5EA_POS8_ADDR+1] = pdata[3];
    tlv_a5a4_valid_flag = 1;
    tlv_a5e9_valid_flag = 1;
    tlv_a5ea_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5E0===================================================================================
  case 0x23B:
    zxdown_buffer[ZXDOWN_A5E0_POS1_ADDR] = pdata[0]; // 节点状态1
    
    if (pdata[1]&BIT(0)) // 变矩器总线信号
      zxdown_buffer[ZXDOWN_A5E0_POS2_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E0_POS2_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(1)) // ABS总线信号
      zxdown_buffer[ZXDOWN_A5E0_POS2_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E0_POS2_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[1]&BIT(4)) // 电涡流温度高检测
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[1]&BIT(3)) // 制动器磨损检测
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[1]&BIT(3)) // 制动蹄片磨损检测开关 ?????????????????????
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[1]&BIT(2)) // ABS激活状态
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(6); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(6); // 清0
    
    tlv_a5e0_valid_flag = 1;
    tlv_a5e6_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5E1====================================================================================
  case 0x21D:
    zxdown_buffer[ZXDOWN_A5E1_POS1_ADDR] = pdata[7];
    if (pdata[0]&BIT(0)) // 分动箱空挡
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[0]&BIT(1)) // 取力standby
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[3]&BIT(0)) // bit0:进入ECO模式
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[0]&BIT(2)) // 取力正常
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E8_POS2_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[3]&BIT(4)) // 变速箱空档检测 校对
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[3]&BIT(5)) // 变速箱倒档检测 校对
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[1]&BIT(0)) // 二轴驱动检测 校对
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[1]&BIT(1)) // 分动箱高档检测
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[1]&BIT(2)) // 分动箱低档检测
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(4); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(4); // 清0

    if (pdata[1]&BIT(3)) // 轮间差速检测
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(5); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[1]&BIT(4)) // 轴间差速检测
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] |= BIT(6); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E1_POS2_ADDR] &= ~BIT(6); // 清0
    
    if (pdata[6]&BIT(2)) // 分动箱低档检测
      zxdown_buffer[ZXDOWN_A5EA_POS2_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5EA_POS2_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[6]&BIT(6)) // 手刹检测(手制动检测开关)
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[3]&BIT(6)) // 行车制动检测开关
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(4); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(4); // 清0
    
    if (pdata[2]&BIT(5)) // 低气压报警
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(5); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[6]&BIT(7)) // PTO检测
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] &= ~BIT(0); // 清0

    zxdown_buffer[ZXDOWN_A5E8_POS1_ADDR] = pdata[2]; // 取力挂接(字节状态位)
    tlv_a5e1_valid_flag = 1;
    tlv_a5e6_valid_flag = 1;
    tlv_a5e7_valid_flag = 1;
    tlv_a5e8_valid_flag = 1;
    tlv_a5ea_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x25E:
    zxdown_buffer[ZXDOWN_A5E1_POS3_ADDR] = pdata[0];  // 变速箱油温
    zxdown_buffer[ZXDOWN_A5E1_POS3_ADDR+1] = pdata[1];
    zxdown_buffer[ZXDOWN_A5E1_POS4_ADDR] = pdata[3];  // 变速箱进气气压
    zxdown_buffer[ZXDOWN_A5E1_POS5_ADDR] = pdata[4];  // 离合器位置 
    
    if (pdata[5]&BIT(0)) // 发动机制动
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] |= BIT(7); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS8_ADDR] &= ~BIT(7); // 清0
    
    if (pdata[5]&BIT(1)) // 电涡流制动1挡
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[5]&BIT(2)) // 电涡流制动2挡
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[5]&BIT(3)) // 电涡流制动3挡
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[5]&BIT(4)) // 电涡流制动4挡
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E6_POS9_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[5]&BIT(5)) // 发动机熄火输入
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[5]&BIT(6)) // 发动机熄火输出
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E7_POS1_ADDR] &= ~BIT(2); // 清0
    
    zxdown_buffer[ZXDOWN_A5E5_POS5_ADDR] = pdata[0];  // 缓速器油温
    zxdown_buffer[ZXDOWN_A5E5_POS5_ADDR+1] = pdata[1];
    zxdown_buffer[ZXDOWN_A5E5_POS6_ADDR] = pdata[2];  // 缓速力矩百分比
    zxdown_buffer[ZXDOWN_A5EA_POS7_ADDR] = pdata[6]; // 马达控制电流
    zxdown_buffer[ZXDOWN_A5EA_POS7_ADDR+1] = pdata[7];
    tlv_a5e1_valid_flag = 1;
    tlv_a5e5_valid_flag = 1;
    tlv_a5e6_valid_flag = 1;
    tlv_a5e7_valid_flag = 1;
    tlv_a5ea_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5E2====================================================================================
  case 0x24F:
    zxdown_buffer[ZXDOWN_A5E2_POS1_ADDR] = pdata[7]; // 支腿垂直状态位
    zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] = pdata[5]; // 支腿水平状态位
    
    if (pdata[6]&BIT(2)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[6]&BIT(3)) // 右前摆臂缩
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[6]&BIT(0)) // 左前摆臂伸
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS3_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[6]&BIT(1)) // 左前摆臂缩
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[6]&BIT(6)) // 右后摆臂伸
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] |= BIT(4); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] &= ~BIT(4); // 清0
    
    if (pdata[6]&BIT(7)) // 右后摆臂缩
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] |= BIT(5); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[6]&BIT(4)) // 左后摆臂伸
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] |= BIT(6); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] &= ~BIT(6); // 清0
    
    if (pdata[6]&BIT(5)) // 左后摆臂缩
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] |= BIT(7); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E2_POS2_ADDR] &= ~BIT(7); // 清0
    
    tlv_a5e2_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5E3====================================================================================
  case 0x22B:
    memcpy(&zxdown_buffer[ZXDOWN_A5E3_POS1_ADDR], pdata, 8); // 悬挂压力
    tlv_a5e3_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x22C:
    memcpy(&zxdown_buffer[ZXDOWN_A5E3_POS2_ADDR], pdata, 8); // 悬挂行程
    tlv_a5e3_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x21E:
    memcpy(&zxdown_buffer[ZXDOWN_A5E3_POS3_ADDR], pdata, 4); // 整车重量
    tlv_a5e3_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x25F:
    if (pdata[1]&BIT(0)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(1)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[0]&BIT(0)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(2); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(1)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(3); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[0]&BIT(2)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(4); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(4); // 清0
    
    if (pdata[0]&BIT(3)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(5); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[0]&BIT(4)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(6); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(6); // 清0
    
    if (pdata[0]&BIT(5)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] |= BIT(7); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS4_ADDR] &= ~BIT(7); // 清0
    
    if (pdata[0]&BIT(6)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS5_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS5_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[0]&BIT(7)) // 右前摆臂伸
      zxdown_buffer[ZXDOWN_A5E3_POS5_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E3_POS5_ADDR] &= ~BIT(1); // 清0
    
    tlv_a5e3_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5E4====================================================================================
  case 0x22D:
    memcpy(&zxdown_buffer[ZXDOWN_A5E4_POS1_ADDR], pdata, 8); // 一二轴转向角度
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x22E:
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR] = pdata[0];  // 电控三轴转向角度
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+1] = pdata[1];
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+2] = pdata[2]; // 电控四轴转向角度
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+3] = pdata[3];
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x25A:
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+4] = pdata[0]; // 电控五轴转向角度
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+5] = pdata[1];
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+6] = pdata[2]; // 电控六轴转向角度
    zxdown_buffer[ZXDOWN_A5E4_POS2_ADDR+7] = pdata[3];
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x2BB:
    memcpy(&zxdown_buffer[ZXDOWN_A5E4_POS3_ADDR], pdata, 8); // 一二轴传感器电流
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;    

  case 0x2BC:
    memcpy(&zxdown_buffer[ZXDOWN_A5E4_POS4_ADDR], pdata, 8); // 三四五六轴传感器电流
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x25C:
    zxdown_buffer[ZXDOWN_A5E4_POS6_ADDR] = pdata[0];  // 目标转向模式
    zxdown_buffer[ZXDOWN_A5E4_POS7_ADDR] = pdata[2];  // 转向系统压力
    zxdown_buffer[ZXDOWN_A5E4_POS7_ADDR+1] = pdata[3];
    zxdown_buffer[ZXDOWN_A5E4_POS8_ADDR] = pdata[4];  // 比例调压阀电流
    zxdown_buffer[ZXDOWN_A5E4_POS8_ADDR+1] = pdata[5];
    zxdown_buffer[ZXDOWN_A5E9_POS2_ADDR] = pdata[6]; // 液压系统压力
    zxdown_buffer[ZXDOWN_A5E9_POS2_ADDR+1] = pdata[7];
    tlv_a5e4_valid_flag = 1;
    tlv_a5e9_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x2BD:
    zxdown_buffer[ZXDOWN_A5E4_POS11_ADDR] = pdata[0];  // 桥锁止阀
    memcpy(&zxdown_buffer[ZXDOWN_A5E4_POS9_ADDR], &pdata[2], 6); // 123桥左右转阀占空比
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x2BE:
    memcpy(&zxdown_buffer[ZXDOWN_A5E4_POS10_ADDR], pdata, 6); // 456桥左右转阀占空比
    tlv_a5e4_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5E5====================================================================================
  case 0x1CF22D21:
    zxdown_buffer[ZXDOWN_A5E5_POS1_ADDR] = pdata[0];  // 当前一轴转角
    zxdown_buffer[ZXDOWN_A5E5_POS1_ADDR+1] = pdata[1];
    tlv_a5e5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x1CF28B21:
    if (pdata[1]&BIT(0)) // 转向系统报警指示灯E101
      zxdown_buffer[ZXDOWN_A5E5_POS6_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E5_POS6_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(1)) // 中位指示灯
      zxdown_buffer[ZXDOWN_A5E5_POS6_ADDR] |= BIT(1); // 置1
    else
      zxdown_buffer[ZXDOWN_A5E5_POS6_ADDR] &= ~BIT(1); // 清0
    
    zxdown_buffer[ZXDOWN_A5E5_POS2_ADDR] = pdata[4];  // 转向锁死压力(bar)
    zxdown_buffer[ZXDOWN_A5E5_POS2_ADDR+1] = pdata[5];
    zxdown_buffer[ZXDOWN_A5E5_POS4_ADDR] = pdata[2];  // 程序标记位
    zxdown_buffer[ZXDOWN_A5E5_POS5_ADDR] = pdata[0];  // 阀字节状态位1
    tlv_a5e5_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x1CF25C21:
    zxdown_buffer[ZXDOWN_A5E5_POS3_ADDR] = pdata[0];  // 目标转向模式
    zxdown_buffer[ZXDOWN_A5E5_POS3_ADDR+1] = pdata[1];// 当前转向模式
    tlv_a5e5_valid_flag = 1;
    retval = 0x01;
    break;
  
  //==A5E6====================================================================================
  case 0x25D:
    zxdown_buffer[ZXDOWN_A5E6_POS1_ADDR] = pdata[0];  // 回路一气压
    zxdown_buffer[ZXDOWN_A5E6_POS1_ADDR+1] = pdata[1];
    zxdown_buffer[ZXDOWN_A5E6_POS2_ADDR] = pdata[2];  // 回路一气压
    zxdown_buffer[ZXDOWN_A5E6_POS2_ADDR+1] = pdata[3];
    zxdown_buffer[ZXDOWN_A5E6_POS3_ADDR] = pdata[4];  // 回路一气压
    zxdown_buffer[ZXDOWN_A5E6_POS3_ADDR+1] = pdata[5];
    zxdown_buffer[ZXDOWN_A5E6_POS4_ADDR] = pdata[6];  // 回路一气压
    zxdown_buffer[ZXDOWN_A5E6_POS4_ADDR+1] = pdata[7];
    tlv_a5e6_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x23A:
    zxdown_buffer[ZXDOWN_A5E6_POS7_ADDR] = pdata[4];  // 变矩器油温
    zxdown_buffer[ZXDOWN_A5E6_POS7_ADDR+1] = pdata[5];
    tlv_a5e6_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5E7====================================================================================
  // 分散
  //==A5E8====================================================================================
  // 分散
  //==A5E9====================================================================================
  // 分散
    
  //==A5EA====================================================================================
  case 0x26A:
    if (pdata[0]&BIT(0)) // 上车液压驱动确认信息
      zxdown_buffer[ZXDOWN_A5EA_POS1_ADDR] |= BIT(0); // 置1
    else
      zxdown_buffer[ZXDOWN_A5EA_POS1_ADDR] &= ~BIT(0); // 清0
    
    zxdown_buffer[ZXDOWN_A5EA_POS3_ADDR] = pdata[1]; // 上车发动机水温
    zxdown_buffer[ZXDOWN_A5EA_POS4_ADDR] = pdata[2]; // 上车发动机机油压力
    zxdown_buffer[ZXDOWN_A5EA_POS4_ADDR+1] = pdata[3];
    zxdown_buffer[ZXDOWN_A5EA_POS5_ADDR] = pdata[4]; // 上车发动机转速
    zxdown_buffer[ZXDOWN_A5EA_POS5_ADDR+1] = pdata[5];
    zxdown_buffer[ZXDOWN_A5EA_POS6_ADDR] = pdata[6]; // 上车泵出口压力
    zxdown_buffer[ZXDOWN_A5EA_POS6_ADDR+1] = pdata[7];
    tlv_a5ea_valid_flag = 1;
    retval = 0x01;
    break;

  //==A5EB====================================================================================
  case 0x28C:
    if(pdata[0]>0 && pdata[0]<19) // 1号轮胎到18号轮胎
    {
      zxdown_buffer[ZXDOWN_A5EB_POS1_ADDR+pdata[0]-1] = pdata[1];
      tlv_a5eb_valid_flag = 1;
    }
    retval = 0x01;
    break;

  //==A5EC====================================================================================
  case 0x2AB:
    zxdown_buffer[ZXDOWN_A5EC_POS1_ADDR] = pdata[0]; // 左支腿面板 - 字节状态位1
    zxdown_buffer[ZXDOWN_A5EC_POS2_ADDR] = pdata[1]; // 左支腿面板 - 字节状态位2
    zxdown_buffer[ZXDOWN_A5EC_POS3_ADDR] = pdata[2]; // 左支腿面板 - 字节状态位3
    zxdown_buffer[ZXDOWN_A5EC_POS4_ADDR] = pdata[3]; // 左支腿面板 - 字节状态位4
    zxdown_buffer[ZXDOWN_A5EC_POS5_ADDR] = pdata[4]; // 左支腿面板 - 字节状态位5
    zxdown_buffer[ZXDOWN_A5EC_POS6_ADDR] = pdata[5]; // 左支腿面板 - 预留
    zxdown_buffer[ZXDOWN_A5EC_POS7_ADDR] = pdata[6]; // 左支腿面板 - DIR
    zxdown_buffer[ZXDOWN_A5EC_POS8_ADDR] = pdata[7]; // 左支腿面板 - 调平盒心跳
    tlv_a5ec_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5ED====================================================================================
  case 0x2AC:
    zxdown_buffer[ZXDOWN_A5ED_POS1_ADDR] = pdata[0]; // 右支腿面板 - 字节状态位1
    zxdown_buffer[ZXDOWN_A5ED_POS2_ADDR] = pdata[1]; // 右支腿面板 - 字节状态位2
    zxdown_buffer[ZXDOWN_A5ED_POS3_ADDR] = pdata[2]; // 右支腿面板 - 字节状态位3
    zxdown_buffer[ZXDOWN_A5ED_POS4_ADDR] = pdata[3]; // 右支腿面板 - 字节状态位4
    zxdown_buffer[ZXDOWN_A5ED_POS5_ADDR] = pdata[4]; // 右支腿面板 - 字节状态位5
    zxdown_buffer[ZXDOWN_A5ED_POS6_ADDR] = pdata[5]; // 右支腿面板 - 预留
    zxdown_buffer[ZXDOWN_A5ED_POS7_ADDR] = pdata[6]; // 右支腿面板 - DIR
    zxdown_buffer[ZXDOWN_A5ED_POS8_ADDR] = pdata[7]; // 右支腿面板 - 调平盒心跳
    tlv_a5ed_valid_flag = 1;
    retval = 0x01;
    break;
    
  //==A5EE====================================================================================
  case 0x2AD:
    zxdown_buffer[ZXDOWN_A5EE_POS1_ADDR] = pdata[0]; // 中控台输入信息 - 字节状态位1
    zxdown_buffer[ZXDOWN_A5EE_POS2_ADDR] = pdata[1]; // 中控台输入信息 - 字节状态位2
    tlv_a5ee_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x2AE:
    zxdown_buffer[ZXDOWN_A5EE_POS3_ADDR] = pdata[0]; // 中控台输入信息 - 字节状态位3
    zxdown_buffer[ZXDOWN_A5EE_POS4_ADDR] = pdata[1]; // 中控台输入信息 - 字节状态位4
    tlv_a5ee_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x2AF:
    zxdown_buffer[ZXDOWN_A5EE_POS5_ADDR] = pdata[0]; // 中控台输入信息 - 字节状态位5
    zxdown_buffer[ZXDOWN_A5EE_POS6_ADDR] = pdata[1]; // 中控台输入信息 - 字节状态位6
    zxdown_buffer[ZXDOWN_A5EE_POS7_ADDR] = pdata[2]; // 中控台输入信息 - 字节状态位7
    zxdown_buffer[ZXDOWN_A5EE_POS8_ADDR] = pdata[3]; // 后桥转向角度
    zxdown_buffer[ZXDOWN_A5EE_POS9_ADDR] = pdata[4]; // 字节状态位8
    tlv_a5ee_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x40D:
    // 大气压力
    // 环境温度
    break;

  //==下车系统版本====================================================================================
  case 0x20F:
    zxversion_buffer[ZXVERSION_A506_POS3_ADDR] = pdata[0];  // P1应用层
    zxversion_buffer[ZXVERSION_A506_POS3_ADDR+1] = pdata[1];
    zxversion_buffer[ZXVERSION_A506_POS3_ADDR+2] = pdata[2];
    zxversion_buffer[ZXVERSION_A506_POS5_ADDR] = pdata[3];  // P2应用层
    zxversion_buffer[ZXVERSION_A506_POS5_ADDR+1] = pdata[4];
    zxversion_buffer[ZXVERSION_A506_POS5_ADDR+2] = pdata[5];
    zxversion_buffer[ZXVERSION_A506_POS7_ADDR] = pdata[6];  // P3应用层
    zxversion_buffer[ZXVERSION_A506_POS7_ADDR+1] = pdata[7];
    tlv_a506_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x21F:
    zxversion_buffer[ZXVERSION_A506_POS7_ADDR+2] = pdata[0];  // P3应用层
    zxversion_buffer[ZXVERSION_A506_POS9_ADDR] = pdata[1];    // P4应用层
    zxversion_buffer[ZXVERSION_A506_POS9_ADDR+1] = pdata[2];
    zxversion_buffer[ZXVERSION_A506_POS9_ADDR+2] = pdata[3];
    zxversion_buffer[ZXVERSION_A506_POS11_ADDR] = pdata[4];   // P5应用层
    zxversion_buffer[ZXVERSION_A506_POS11_ADDR+1] = pdata[5];
    zxversion_buffer[ZXVERSION_A506_POS11_ADDR+2] = pdata[6];
    zxversion_buffer[ZXVERSION_A506_POS17_ADDR] = pdata[7];   // P8应用层
    tlv_a506_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x22F:
    zxversion_buffer[ZXVERSION_A506_POS17_ADDR+1] = pdata[0];   // P8应用层
    zxversion_buffer[ZXVERSION_A506_POS17_ADDR+2] = pdata[1];
    tlv_a506_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x29F:
    if(pdata[0]==1)
    {
      zxversion_buffer[ZXVERSION_A506_POS4_ADDR] = pdata[1];  // P1底层
      zxversion_buffer[ZXVERSION_A506_POS4_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS4_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==2)
    {
      zxversion_buffer[ZXVERSION_A506_POS6_ADDR] = pdata[1];  // P2底层
      zxversion_buffer[ZXVERSION_A506_POS6_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS6_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==3)
    {
      zxversion_buffer[ZXVERSION_A506_POS8_ADDR] = pdata[1];  // P3底层
      zxversion_buffer[ZXVERSION_A506_POS8_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS8_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==4)
    {
      zxversion_buffer[ZXVERSION_A506_POS10_ADDR] = pdata[1];  // P4底层
      zxversion_buffer[ZXVERSION_A506_POS10_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS10_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==5)
    {
      zxversion_buffer[ZXVERSION_A506_POS12_ADDR] = pdata[1];  // P5底层
      zxversion_buffer[ZXVERSION_A506_POS12_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS12_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==6)
    {
      zxversion_buffer[ZXVERSION_A506_POS14_ADDR] = pdata[1];  // P6底层
      zxversion_buffer[ZXVERSION_A506_POS14_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS14_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==7)
    {
      zxversion_buffer[ZXVERSION_A506_POS16_ADDR] = pdata[1];  // P7底层
      zxversion_buffer[ZXVERSION_A506_POS16_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS16_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    else if(pdata[0]==8)
    {
      zxversion_buffer[ZXVERSION_A506_POS18_ADDR] = pdata[1];  // P8底层
      zxversion_buffer[ZXVERSION_A506_POS18_ADDR+1] = pdata[2];
      zxversion_buffer[ZXVERSION_A506_POS18_ADDR+2] = pdata[3];
      tlv_a506_valid_flag = 1;
    }
    retval = 0x01;
    break;
  
  default:
    retval = 0x00;
    break;
  }
  
  return retval;
}

/*************************************************************************
 * 处理接收到的CAN报文(底盘发动机)
*************************************************************************/
uint8_t CAN_ProcessRecvEngineMsg(uint32_t canId, uint8_t *pdata, uint8_t size)
{
  uint8_t retval = 0x00;
  //uint32_t tempVal;

  switch (canId) // 上车通信
  {
  //==A5EF===================================================================================
  case 0x0CF00300:
    zxengine_buffer[ZXENGINE_A5EF_POS11_ADDR] = pdata[1]; // 油门踏板百分比
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x0CF00400:
    if (pdata[0]&BIT(0)) // 发动机扭矩模式1
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[0]&BIT(1)) // 发动机扭矩模式2
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[0]&BIT(2)) // 发动机扭矩模式3
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[0]&BIT(3)) // 发动机扭矩模式4
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS22_ADDR] &= ~BIT(3); // 清0

    zxengine_buffer[ZXENGINE_A5EF_POS1_ADDR] = pdata[3]; // 发动机转速
    zxengine_buffer[ZXENGINE_A5EF_POS1_ADDR+1] = pdata[4];
    zxengine_buffer[ZXENGINE_A5EF_POS2_ADDR] = pdata[2]; // 实际扭矩百分比
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEEE00:
    zxengine_buffer[ZXENGINE_A5EF_POS6_ADDR] = pdata[0]; // 冷却液温度
    zxengine_buffer[ZXENGINE_A5EF_POS7_ADDR] = pdata[2]; // 机油温度
    zxengine_buffer[ZXENGINE_A5EF_POS7_ADDR+1] = pdata[3];
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEEF00:
    zxengine_buffer[ZXENGINE_A5EF_POS8_ADDR] = pdata[2]; // 机油液位
    zxengine_buffer[ZXENGINE_A5EF_POS9_ADDR] = pdata[3]; // 机油压力
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
 
  case 0x18FEF600:
    zxengine_buffer[ZXENGINE_A5EF_POS4_ADDR] = pdata[1]; // 进气歧管压力
    zxengine_buffer[ZXENGINE_A5EF_POS5_ADDR] = pdata[2]; // 进气歧管温度
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
   
  case 0x18FEE500:
    zxengine_buffer[ZXENGINE_A5EF_POS10_ADDR] = pdata[0]; // 发动机总运行时间
    zxengine_buffer[ZXENGINE_A5EF_POS10_ADDR+1] = pdata[1];
    zxengine_buffer[ZXENGINE_A5EF_POS10_ADDR+2] = pdata[2];
    zxengine_buffer[ZXENGINE_A5EF_POS10_ADDR+3] = pdata[3];
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x18FEDF00:
    zxengine_buffer[ZXENGINE_A5EF_POS3_ADDR] = pdata[0]; // 摩擦扭矩百分比
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;

  case 0x18FEF100:
    zxengine_buffer[ZXENGINE_A5EF_POS13_ADDR] = pdata[3];
    
    if (pdata[4]&BIT(0)) // 巡航设置开关1
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[4]&BIT(1)) // 巡航设置开关1
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(1); // 清0
    
    if (pdata[4]&BIT(2)) // 巡航减速开关1
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(2); // 清0
    
    if (pdata[4]&BIT(3)) // 巡航减速开关2
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(3); // 清0
    
    if (pdata[4]&BIT(6)) // 巡航控制加速开关1
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(4); // 清0
    
    if (pdata[4]&BIT(7)) // 巡航控制加速开关2
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] |= BIT(5); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS14_ADDR] &= ~BIT(5); // 清0
    
    if (pdata[6]&BIT(5)) // 巡航控制状态1
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(4); // 清0

    if (pdata[6]&BIT(6)) // 巡航控制状态2
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(5); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(5); // 清0

    if (pdata[6]&BIT(7)) // 巡航控制状态3
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(6); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(6); // 清0

    zxengine_buffer[ZXENGINE_A5EF_POS12_ADDR] = pdata[1]; // 车速
    zxengine_buffer[ZXENGINE_A5EF_POS12_ADDR+1] = pdata[2];
    zxengine_buffer[ZXENGINE_A5EF_POS15_ADDR] = pdata[5]; // 巡航设定速度
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEF200:
    zxengine_buffer[ZXENGINE_A5EF_POS16_ADDR] = pdata[0]; // 发动机燃油消耗率
    zxengine_buffer[ZXENGINE_A5EF_POS16_ADDR+1] = pdata[1];
    zxengine_buffer[ZXENGINE_A5EF_POS17_ADDR] = pdata[4]; // 发动机平均燃油消耗率
    zxengine_buffer[ZXENGINE_A5EF_POS17_ADDR+1] = pdata[5];
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEE900:
    zxengine_buffer[ZXENGINE_A5EF_POS18_ADDR] = pdata[4]; // 燃油总油耗量
    zxengine_buffer[ZXENGINE_A5EF_POS18_ADDR+1] = pdata[5];
    zxengine_buffer[ZXENGINE_A5EF_POS18_ADDR+2] = pdata[6];
    zxengine_buffer[ZXENGINE_A5EF_POS18_ADDR+3] = pdata[7];
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEE000:
    zxengine_buffer[ZXENGINE_A5EF_POS19_ADDR] = pdata[4]; // 总行驶里程
    zxengine_buffer[ZXENGINE_A5EF_POS19_ADDR+1] = pdata[5];
    zxengine_buffer[ZXENGINE_A5EF_POS19_ADDR+2] = pdata[6];
    zxengine_buffer[ZXENGINE_A5EF_POS19_ADDR+3] = pdata[7];
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEFC17:
    zxengine_buffer[ZXENGINE_A5EF_POS20_ADDR] = pdata[1]; // 燃油液位1
    zxengine_buffer[ZXENGINE_A5EF_POS21_ADDR] = pdata[6]; // 燃油液位2
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FD0700:
    if (pdata[0]&BIT(2)) // 驾驶员报警灯DWL-1
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(0); // 清0
    
    if (pdata[0]&BIT(3)) // 驾驶员报警灯DWL-2
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(1); // 清0
    
    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
    
  case 0x18FEFF00:
    if (pdata[0]&BIT(0)) // 油中有水指示灯-1
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(2); // 清0

    if (pdata[0]&BIT(1)) // 油中有水指示灯-2
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5EF_POS23_ADDR] &= ~BIT(3); // 清0

    tlv_a5ef_valid_flag = 1;
    retval = 0x01;
    break;
  
  //==A5F0===================================================================================
  //GPS计算，根据发动机PTO区分上下车工作
  
  //==A5F1===================================================================================
  case 0x18FF203D:
    zxengine_buffer[ZXENGINE_A5F1_POS1_ADDR] = pdata[0]; // 尿素喷射状态
    zxengine_buffer[ZXENGINE_A5F1_POS2_ADDR] = pdata[5]; // T15_DCU
    zxengine_buffer[ZXENGINE_A5F1_POS3_ADDR] = pdata[6]; // 尿素泵压力
    zxengine_buffer[ZXENGINE_A5F1_POS3_ADDR+1] = pdata[7];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FE563D:
    zxengine_buffer[ZXENGINE_A5F1_POS4_ADDR] = pdata[0]; // 尿素箱液位
    zxengine_buffer[ZXENGINE_A5F1_POS5_ADDR] = pdata[1]; // 尿素箱温度
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x0CF0233D:
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR] = pdata[6]; // 尿素喷射量
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR+1] = pdata[7];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18F00E51:
    zxengine_buffer[ZXENGINE_A5F1_POS7_ADDR] = pdata[0]; // SCR上游NOx浓度
    zxengine_buffer[ZXENGINE_A5F1_POS7_ADDR+1] = pdata[1];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18F00F52:
    zxengine_buffer[ZXENGINE_A5F1_POS8_ADDR] = pdata[0]; // SCR下游NOx浓度
    zxengine_buffer[ZXENGINE_A5F1_POS8_ADDR+1] = pdata[1];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FD3E3D:
    zxengine_buffer[ZXENGINE_A5F1_POS9_ADDR] = pdata[0]; // SCR上游排气温度(T6温度)
    zxengine_buffer[ZXENGINE_A5F1_POS9_ADDR+1] = pdata[1];
    zxengine_buffer[ZXENGINE_A5F1_POS10_ADDR] = pdata[3]; // SCR下游排气温度(T7温度)
    zxengine_buffer[ZXENGINE_A5F1_POS10_ADDR+1] = pdata[4];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FD9BA3:
    if (pdata[3]&BIT(0)) // 品质温度传感器FMI (SPN 3519)1
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] &= ~BIT(0); // 清0

    if (pdata[3]&BIT(1)) // 品质温度传感器FMI (SPN 3519)2
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] &= ~BIT(1); // 清0

    if (pdata[3]&BIT(2)) // 品质温度传感器FMI (SPN 3519)3
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] &= ~BIT(2); // 清0

    if (pdata[3]&BIT(3)) // 品质温度传感器FMI (SPN 3519)4
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] &= ~BIT(3); // 清0

    if (pdata[3]&BIT(4)) // 品质温度传感器FMI (SPN 3519)5
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS14_ADDR] &= ~BIT(4); // 清0

    if (pdata[4]&BIT(0)) // 品质传感器FMI (SPN3520)1
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] &= ~BIT(0); // 清0

    if (pdata[4]&BIT(1)) // 品质传感器FMI (SPN3520)2
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] &= ~BIT(1); // 清0

    if (pdata[4]&BIT(2)) // 品质传感器FMI (SPN3520)3
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] &= ~BIT(2); // 清0

    if (pdata[4]&BIT(3)) // 品质传感器FMI (SPN3520)4
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] &= ~BIT(3); // 清0

    if (pdata[4]&BIT(4)) // 品质传感器FMI (SPN3520)5
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS15_ADDR] &= ~BIT(4); // 清0

    if (pdata[5]&BIT(0)) // 催化剂试剂类型(SPN3521)1
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // 催化剂试剂类型(SPN3521)2
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(2)) // 催化剂试剂类型(SPN3521)3
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] &= ~BIT(2); // 清0

    if (pdata[5]&BIT(3)) // 催化剂试剂类型(SPN3521)4
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS16_ADDR] &= ~BIT(3); // 清0

    zxengine_buffer[ZXENGINE_A5F1_POS13_ADDR] = pdata[1]; // 尿素品质传感器温度(SPN 3515)
    zxengine_buffer[ZXENGINE_A5F1_POS11_ADDR] = pdata[1]; // 尿素浓度(SPN 3516)
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FCBD3D:
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR] = pdata[4]; // 累计尿素消耗量
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR+1] = pdata[5];
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR+2] = pdata[6];
    zxengine_buffer[ZXENGINE_A5F1_POS6_ADDR+3] = pdata[7];
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FE56A3:
    if (pdata[4]&BIT(0)) // 尿素箱液位传感器失效模式FMI-1
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] &= ~BIT(0); // 清0

    if (pdata[4]&BIT(1)) // 尿素箱液位传感器失效模式FMI-2
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] &= ~BIT(1); // 清0

    if (pdata[4]&BIT(2)) // 尿素箱液位传感器失效模式FMI-3
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] &= ~BIT(2); // 清0

    if (pdata[4]&BIT(3)) // 尿素箱液位传感器失效模式FMI-4
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] &= ~BIT(3); // 清0

    if (pdata[4]&BIT(4)) // 尿素箱液位传感器失效模式FMI-5
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS17_ADDR] &= ~BIT(4); // 清0

    if (pdata[5]&BIT(0)) // 尿素箱温度传感器失效模式FMI-1
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // 尿素箱温度传感器失效模式FMI-2
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(2)) // 尿素箱温度传感器失效模式FMI-3
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] &= ~BIT(2); // 清0

    if (pdata[5]&BIT(3)) // 尿素箱温度传感器失效模式FMI-4
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] &= ~BIT(3); // 清0

    if (pdata[5]&BIT(4)) // 尿素箱温度传感器失效模式FMI-5
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] |= BIT(4); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS18_ADDR] &= ~BIT(4); // 清0
    
    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FEDF3D:
    if (pdata[7]&BIT(0)) // Nox传感器露点状态1
      zxengine_buffer[ZXENGINE_A5F1_POS19_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS19_ADDR] &= ~BIT(0); // 清0

    if (pdata[7]&BIT(1)) // Nox传感器露点状态2
      zxengine_buffer[ZXENGINE_A5F1_POS19_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F1_POS19_ADDR] &= ~BIT(1); // 清0

    tlv_a5f1_valid_flag = 1;
    retval = 0x01;
    break;
  
  //==A5F2===================================================================================
  case 0x18FD2000:
    zxengine_buffer[ZXENGINE_A5F2_POS1_ADDR] = pdata[0]; // DOC上游排气温度
    zxengine_buffer[ZXENGINE_A5F2_POS1_ADDR+1] = pdata[1];
    zxengine_buffer[ZXENGINE_A5F2_POS2_ADDR] = pdata[2]; // DPF上游排气温度
    zxengine_buffer[ZXENGINE_A5F2_POS2_ADDR+1] = pdata[3];
    tlv_a5f2_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FF1400:
    if (pdata[1]&BIT(0)) // DPF再生激活状态1
      zxengine_buffer[ZXENGINE_A5F2_POS5_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS5_ADDR] &= ~BIT(0); // 清0

    if (pdata[1]&BIT(1)) // DPF再生激活状态2
      zxengine_buffer[ZXENGINE_A5F2_POS5_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS5_ADDR] &= ~BIT(1); // 清0

    zxengine_buffer[ZXENGINE_A5F2_POS3_ADDR] = pdata[0]; // DPF碳载量负荷率
    tlv_a5f2_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FDB200:
    zxengine_buffer[ZXENGINE_A5F2_POS4_ADDR] = pdata[0]; // DPF压差
    zxengine_buffer[ZXENGINE_A5F2_POS4_ADDR+1] = pdata[1];
    tlv_a5f2_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18FD7C00:
    if (pdata[0]&BIT(0)) // DPF再生指示灯状态1
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] &= ~BIT(0); // 清0

    if (pdata[0]&BIT(1)) // DPF再生指示灯状态2
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] &= ~BIT(1); // 清0

    if (pdata[0]&BIT(2)) // DPF再生指示灯状态3
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS6_ADDR] &= ~BIT(2); // 清0

    if (pdata[2]&BIT(0)) // DPF再生禁止状态1
      zxengine_buffer[ZXENGINE_A5F2_POS7_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS7_ADDR] &= ~BIT(0); // 清0

    if (pdata[2]&BIT(1)) // DPF再生禁止状态2
      zxengine_buffer[ZXENGINE_A5F2_POS7_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS7_ADDR] &= ~BIT(1); // 清0
      
    tlv_a5f2_valid_flag = 1;
    retval = 0x01;
    break;
  case 0x18E00021:
    if (pdata[5]&BIT(0)) // DPF再生开关状态1
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] |= BIT(0); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] &= ~BIT(0); // 清0

    if (pdata[5]&BIT(1)) // DPF再生开关状态2
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] |= BIT(1); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] &= ~BIT(1); // 清0

    if (pdata[5]&BIT(2)) // DPF再生禁止开关状态1
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] |= BIT(2); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] &= ~BIT(2); // 清0

    if (pdata[5]&BIT(3)) // DPF再生禁止开关状态2
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] |= BIT(3); // 置1
    else
      zxengine_buffer[ZXENGINE_A5F2_POS8_ADDR] &= ~BIT(3); // 清0

    tlv_a5f2_valid_flag = 1;
    retval = 0x01;
    break;
  
  default:
    retval = 0x00;
    break;
  }
  
  return retval;
}


