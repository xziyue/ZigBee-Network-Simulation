#include "zb_common.h"

#ifdef ZB_TRANSPORT_USE_LINUX_WPAN

#include "zb_common.h"
#include "zb_bufpool.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"


void zb_mac_transport_init(zb_char_t *wpanName){
    int ret, sd;
    ssize_t len;
    struct sockaddr_ll sll;
    struct ifreq ifr;

    // initialize with default value
    ZIG->ioctx.sd = -1;

    /* Create AF_PACKET address family socket for the SOCK_RAW type */
    sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IEEE802154));
    if (sd < 0) {
        perror("socket");
        ZB_ABORT;
    }

    /* Using a monitor interface here results in a bad FCS and two missing
     * bytes from payload, using the normal IEEE 802.15.4 interface here */
    strncpy(ifr.ifr_name, "wpan0", IFNAMSIZ);
    ret = ioctl(sd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl");
        close(sd);
        ZB_ABORT;
    }

    /* Prepare destination socket address struct */
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_IEEE802154);

    /* Bind socket on this side */
    ret = bind(sd, (struct sockaddr *)&sll, sizeof(sll));
    if (ret < 0) {
        perror("bind");
        close(sd);
        ZB_ABORT;
    }

    // socket initialization finished
    ZIG->ioctx.sd = sd;
}


#endif