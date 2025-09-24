#ifndef SRCP_MSG_H
#define SRCP_MSG_H

#include <stdio.h>

#define SRCP_MAGIC            0xadbd
#define SRCP_TYPE             0xaa55

#define CMD_TYPE_REQ          0x1
#define CMD_TYPE_RESP         0x2

#define CMD_PROTOCOL_VERSION  0x1000

#define CMD_SRC_MAIN_MCU      0x0
#define CMD_SRC_SAFE_MCU      0x1
#define CMD_SRC_X86           0x2
#define CMD_SRC_2160          0x3
#define CMD_SRC_BMC           0x4
#define CMD_SRC_FPGA          0x5

#define CMD_DES_MAIN_MCU      0x0
#define CMD_DES_SAFE_MCU      0x1
#define CMD_DES_X86           0x2
#define CMD_DES_2160          0x3
#define CMD_DES_BMC           0x4
#define CMD_DES_FPGA          0x5

#define CMD_ACK               0x0
#define CMD_SHUTDOWN          0x1
#define CMD_DELAY_SHUTDOWN    0x2
#define CMD_RESERVE           0x3
#define CMD_HEART_BEAT        0x4

#define SOC_DELAY_REASON_OTA  0x1
#define SOC_DELAY_MAX         -1      //mcu will not shutdown the system

#define SPL5_STATUS_DEFAULT   0x0
#define SPL5_STATUS_LOW       0x1
#define SPL5_STATUS_HIGH      0x2
#define SPL5_STATUS_UNKNOWN   0x3

typedef struct {
    unsigned int  cmd_type;
    unsigned int  protocol_version;
    unsigned int  timestamp;
    unsigned char cmd_src;
    unsigned char cmd_des;
    unsigned char action;
    unsigned char delay_reason;
    int           delay_time;
    unsigned int  spl5_status;
    unsigned int  reserve_1;
    unsigned char reserve_2[3];
    unsigned char check_sum;
} pd_cmd_blk;

typedef struct {
    unsigned short magic_num;
    unsigned short type;
    unsigned short flags;
    unsigned short seq_no;
    unsigned int timestamp;
    unsigned short reserve;
    unsigned short payload_len;
    pd_cmd_blk payload;
    //uint8 payload[];
} srcp_msg_hdr_t;

#endif