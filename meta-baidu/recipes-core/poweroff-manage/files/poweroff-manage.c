#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#include "log.h"
#include "srcp_msg.h"

#define SERVER_PORT 6000

typedef void (*sighandler_t)(int);
static unsigned short seq_number = 1;
static int g_sockfd = -1;

static const char rel_version[] = "rel_verion: 1.0.0.1";

unsigned char check_sum_func(const unsigned char *src_data,unsigned int len) {
    unsigned int check_sum_rst = 0;
    int i = 0;
    for(i = 0; i < len; i++) {
        check_sum_rst += src_data[i];
    }

    return  (check_sum_rst & 0xff);
}

void check_power_status() {
    LOG_INFO("Check power status");
    FILE *pipe;
    char buffer[128];
    const char *cmd = "ipmitool chassis power status";

    pipe = popen(cmd, "r");
    if (pipe == NULL) {
        LOG_ERROR("popen error");
        return;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        if (strstr(buffer, "Chassis Power is off")) {
            LOG_INFO("X86 Power Status: OFF");
        } else if (strstr(buffer, "Chassis Power is on")) {
            LOG_INFO("X86 Power Status: ON");
        } else {
            LOG_ERROR("Unknown X86 Power Status: %s", buffer);
        }
    }
    pclose(pipe);
}

void print_srcp_msg(const srcp_msg_hdr_t *srcp_msg_hdr_ptr) {
    if (srcp_msg_hdr_ptr == NULL) {
        LOG_ERROR("Invalid srcp message");
        return;
    }

    LOG_DEBUG("srcp head:");
    LOG_DEBUG(" srcp magic_num  : 0x%x", srcp_msg_hdr_ptr->magic_num);
    LOG_DEBUG(" srcp type       : 0x%x", srcp_msg_hdr_ptr->type);
    LOG_DEBUG(" srcp flags      : 0x%x", srcp_msg_hdr_ptr->flags);
    LOG_DEBUG(" srcp seq_no     : 0x%x", srcp_msg_hdr_ptr->seq_no);
    LOG_DEBUG(" srcp timestamp  : 0x%x", srcp_msg_hdr_ptr->timestamp);
    LOG_DEBUG(" srcp reserve    : 0x%x", srcp_msg_hdr_ptr->reserve);
    LOG_DEBUG(" srcp payload_len: 0x%x", srcp_msg_hdr_ptr->payload_len);

    LOG_DEBUG("payload head:");
    LOG_DEBUG(" cmd_type        : 0x%x", srcp_msg_hdr_ptr->payload.cmd_type);
    LOG_DEBUG(" protocol_version: 0x%x", srcp_msg_hdr_ptr->payload.protocol_version);
    LOG_DEBUG(" timestamp       : %d", srcp_msg_hdr_ptr->payload.timestamp);
    LOG_DEBUG(" cmd_src         : %d", srcp_msg_hdr_ptr->payload.cmd_src);
    LOG_DEBUG(" cmd_des         : %d", srcp_msg_hdr_ptr->payload.cmd_des);
    LOG_DEBUG(" action          : 0x%x", srcp_msg_hdr_ptr->payload.action);
    LOG_DEBUG(" delay_reason    : 0x%x", srcp_msg_hdr_ptr->payload.delay_reason);
    LOG_DEBUG(" delay_time      : %d(s)", srcp_msg_hdr_ptr->payload.delay_time);
    LOG_DEBUG(" spl5_status     : 0x%x", srcp_msg_hdr_ptr->payload.spl5_status);
    LOG_DEBUG(" check_sum       : 0x%x", srcp_msg_hdr_ptr->payload.check_sum);
}

int resp_to_mcu(const int sockfd, const struct sockaddr_in *client_addr, unsigned char action) {
    if (sockfd < 0) {
        LOG_ERROR("Invalid socket fd");
        return -1;
    }

    if (client_addr == NULL) {
        LOG_ERROR("Invalid client address");
        return -1;
    }

    srcp_msg_hdr_t srcp_msg_hdr_resp;
    struct timeval time_now;

    //fill in the srcp head
    memset(&srcp_msg_hdr_resp, 0, sizeof(srcp_msg_hdr_t));
    srcp_msg_hdr_resp.magic_num = SRCP_MAGIC;
    srcp_msg_hdr_resp.type = SRCP_TYPE;
    srcp_msg_hdr_resp.flags = 0;
    srcp_msg_hdr_resp.seq_no = seq_number++;
    srcp_msg_hdr_resp.timestamp = 0;
    srcp_msg_hdr_resp.reserve = 0;
    srcp_msg_hdr_resp.payload_len = sizeof(pd_cmd_blk);

    srcp_msg_hdr_resp.payload.cmd_type = CMD_TYPE_RESP;
    srcp_msg_hdr_resp.payload.protocol_version = CMD_PROTOCOL_VERSION;
    gettimeofday(&time_now, NULL);
    srcp_msg_hdr_resp.payload.timestamp = time_now.tv_sec;
    srcp_msg_hdr_resp.payload.cmd_src = CMD_SRC_BMC;
    srcp_msg_hdr_resp.payload.cmd_des = CMD_DES_MAIN_MCU;
    srcp_msg_hdr_resp.payload.action = action;
    srcp_msg_hdr_resp.payload.delay_reason = 0;
    srcp_msg_hdr_resp.payload.delay_time = 0;
    srcp_msg_hdr_resp.payload.check_sum = check_sum_func((unsigned char *)(&srcp_msg_hdr_resp.payload), \
        sizeof(pd_cmd_blk) - 1);

    LOG_DEBUG("Respond Msg to MCU: ");
    print_srcp_msg(&srcp_msg_hdr_resp);

    int ret_val = sendto(sockfd, &srcp_msg_hdr_resp, sizeof(srcp_msg_hdr_t), 0, \
        (const struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
    
    if (ret_val < 0) {
        LOG_ERROR("sendto error");
        return -1;
    }

    LOG_INFO("Send action to MCU: 0x%x", srcp_msg_hdr_resp.payload.action);

    return 0;
}

static void sig_handle(int sig_num)
{
    LOG_INFO("Catch signal %d, releas all resources", sig_num);
    if (g_sockfd > 0) {
        close(g_sockfd);
        g_sockfd = -1;
    }
    usleep(1000);
    exit(0);
}

int main(int argc,char **argv)
{
    int ret_val = -1, reuse = 1;
    char tmp_cmd_buf[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    unsigned char cal_check_sum = 0;
    srcp_msg_hdr_t srcp_msg_hdr_req, srcp_msg_hdr_resp;
    struct timeval time_now;
    sighandler_t sig_ret = SIG_DFL;

    sig_ret = signal(SIGINT, sig_handle);
    if (sig_ret == SIG_ERR) {
        LOG_ERROR("signal SIGINT error");
        return -1;
    }

    sig_ret = signal(SIGTERM, sig_handle);
    if (sig_ret == SIG_ERR) {
        LOG_ERROR("signal SIGTERM error");
        return -1;
    }

    LOG_INFO("BMC Poweroff Manage version: %s", rel_version);
    LOG_INFO("BMC Poweroff Manage start running");

    g_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_sockfd < 0) {
        LOG_ERROR("Create socket error");
        return -1;
    }

    ret_val = setsockopt(g_sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if (ret_val < 0) {
        LOG_ERROR("Setsockopt error");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    ret_val = bind(g_sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret_val < 0) {
        LOG_ERROR("Bind error");
        return -1;
    }

    while(1) {
        memset(&srcp_msg_hdr_req, 0, sizeof(srcp_msg_hdr_t));
        ret_val = recvfrom(g_sockfd, &srcp_msg_hdr_req, sizeof(srcp_msg_hdr_t), 0, \
                (struct sockaddr *)&client_addr, &addr_len);
        if (ret_val < 0) {
            LOG_ERROR("Recv mcu srcp req msg error");
            continue;
        }

        LOG_DEBUG("Recv Msg from MCU, len = %d", ret_val);
        print_srcp_msg(&srcp_msg_hdr_req);

        cal_check_sum = check_sum_func((unsigned char *)(&srcp_msg_hdr_req.payload), sizeof(pd_cmd_blk) - 1);
        LOG_INFO("Recv action from MCU: 0x%x", srcp_msg_hdr_req.payload.action);
        if (cal_check_sum == srcp_msg_hdr_req.payload.check_sum)
        {
            if (srcp_msg_hdr_req.payload.action == CMD_SHUTDOWN) {
                int resp_ret = resp_to_mcu(g_sockfd, &client_addr, CMD_ACK);
                if (resp_ret < 0) {
                    LOG_ERROR("Respond to MCU error");
                } else {
                    LOG_INFO("Respond to MCU success");
                    break;
                }
            } else {
                LOG_ERROR("Recv wrong cmd atcion: 0x%x, continue!", srcp_msg_hdr_req.payload.action);
            }
        } else {
            LOG_ERROR("Cal_check_sum error, rcv checksum 0x%x, cal checksum 0x%x, continue!", \
                    srcp_msg_hdr_req.payload.check_sum, cal_check_sum);
        }
    }

    check_power_status();

    if (g_sockfd > 0) {
        close(g_sockfd);
        g_sockfd = -1;
    }

    LOG_INFO("Start to shutdown the system");
    FILE *fp = popen("sleep 1 && shutdown -h now &", "r");
    if (!fp) {
        LOG_ERROR("Failed to execute shutdown command");
        return -1;
    } else {
        pclose(fp);
    }

    LOG_INFO("BMC will shutdown after 1s");
    LOG_INFO("BMC Poweroff Manage stop running");
    while(1) {
        usleep(1000);
    }
    return 0;
}