#ifndef FILE_ZB_TRANSPORT_LINUX_WPAN_H
#define FILE_ZB_TRANSPORT_LINUX_WPAN_H

#include "zb_bufpool.h"
#include "zb_types.h"

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define IEEE802154_ADDR_LEN 8
#define MAX_PACKET_LEN 127

#ifndef ETH_P_IEEE802154
#define ETH_P_IEEE802154 0x00F6

typedef struct zb_io_ctx_s
{
#ifndef ZB_SNIFFER
    zb_buf_t *send_data_buf;    /* pointer to zigbee data buffer to send, can be sent via UART or SPI */
    zb_buf_t *recv_data_buf;    /* pointer to buffer to receive to */
    zb_ushort_t bytes_to_recv;  /* bytes number to receive; if 0, calculate this number automatically */
    zb_uint8_t recv_finished;    /* receive status - ZB_NO_IO, ZB_IO_ERROR, ZB_RECV_PENDING, ZB_RECV_FINISHED */
    zb_uint8_t send_finished;    /* send status - ZB_NO_IO, ZB_IO_ERROR, ZB_SEND_IN_PROGRESS, ZB_SEND_FINISHED */
#endif
    zb_uint16_t int_counter;

    int sd; // socket number
}
zb_io_ctx_t;


#define ZB_TIMER_INIT() /* nothing to do here */

#define ZB_CHECK_TIMER_IS_ON() 1 /* always on in linux */

#define ZB_START_HW_TIMER() /* nothing to do here */

void zb_mac_wait_for_ext_event();
#define ZB_TRY_IO() (zb_mac_wait_for_ext_event(), RET_OK)

#define ZB_GO_IDLE()
#define CHECK_INT_N_TIMER()
#define ZB_CORE_IDLE()

#endif

#endif