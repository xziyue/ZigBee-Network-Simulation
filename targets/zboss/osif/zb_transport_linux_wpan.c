#include "zb_common.h"

#ifdef ZB_TRANSPORT_USE_LINUX_WPAN

#include "zb_common.h"
#include "zb_bufpool.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include <linux/if_ether.h>
#include <errno.h>
#include <netinet/udp.h>


void zb_mac_transport_init(zb_init_params *params){
    int ret, sd;

    // initialize with default value
    ZIG->ioctx.sd = -1;

    /* Create AF_PACKET address family socket for the SOCK_RAW type */

#ifdef ZB_WPAN_USE_802154
    struct ifreq ifr;
    sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IEEE802154));
#endif

#ifdef ZB_WPAN_USE_UDP
    sd = socket(AF_INET, SOCK_DGRAM, 0);
#endif

    if (sd < 0) {
        perror("socket");
        WPAN_ABORT;
    }

#ifdef ZB_WPAN_USE_802154
    /* Using a monitor interface here results in a bad FCS and two missing
     * bytes from payload, using the normal IEEE 802.15.4 interface here */
    strncpy(ifr.ifr_name, params->wpanName, IFNAMSIZ);
    ret = ioctl(sd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl");
        close(sd);
        WPAN_ABORT;
    }
    printf("%s%d\n", "device index: ", ifr.ifr_ifindex);
#endif

    // zero out address
    struct ZB_WPAN_SOCK_ADDR_T *sll = &ZIG->ioctx.sll;
    memset(sll, 0, sizeof(struct ZB_WPAN_SOCK_ADDR_T));

#ifdef ZB_WPAN_USE_802154
    /* Prepare destination socket address struct */
    sll->sll_family = AF_PACKET;
    sll->sll_ifindex = ifr.ifr_ifindex;
    sll->sll_halen = ETH_ALEN;
    sll->sll_protocol = htons(ETH_P_IEEE802154);
#endif

#ifdef ZB_WPAN_USE_UDP
    ZIG->ioctx.from_ip = params->from_ip;
    ZIG->ioctx.to_ip = params->to_ip;
    ZIG->ioctx.dport = params->dport;
    ZIG->ioctx.sport = params->sport;
    sll->sin_family = AF_INET;
    sll->sin_addr.s_addr = ZIG->ioctx.from_ip;
    sll->sin_port = htons(ZIG->ioctx.sport);
#endif

    /* Bind socket on this side */
    ret = bind(sd, (struct sockaddr *)sll, sizeof(struct ZB_WPAN_SOCK_ADDR_T));
    if (ret < 0) {
        perror("bind");
        close(sd);
        WPAN_ABORT;
    }

    // socket initialization finished
    ZIG->ioctx.sd = sd;

    // set default timeout
    ZIG->ioctx.timeout = 1;

    ZB_TIMER_INIT();
}



void zb_mac_transport_start_recv(zb_buf_t *buf, zb_short_t dummy){
    zb_ret_t ret = 0;

    TRACE_MSG(TRACE_MAC1, ">>zb_mac_start_recv buf %p", (FMT__P, buf));

    /* check if we already have buffer for input */
    if ( ZIG->ioctx.recv_data_buf )
    {
        TRACE_MSG(TRACE_ERROR, "incoming buffer is already in progress", (FMT__0));
        ret = RET_ALREADY_EXISTS;
    }
    else
    {
        ZIG->ioctx.recv_data_buf = buf;
        ZIG->ioctx.recv_data_buf->u.hdr.len = 0;
    }

    TRACE_MSG(TRACE_MAC1, "<<zb_mac_start_recv ret %d", (FMT__D, ret));
}

// read packet from WPAN interface
zb_ret_t read_from_wpan(){
    zb_ret_t ret = 0;
    int bts_read = 0;

    TRACE_MSG(TRACE_MAC1, ">>read_from_wpan", (FMT__0));

    if ( !ZIG->ioctx.recv_data_buf )
    {
        TRACE_MSG(TRACE_MAC1, "no buffer available, call mac to get incoming buffer", (FMT__0));
        /* call mac to get incoming buffer. exit */
        ZG->sched.mac_receive_pending = 1;
        ret = 1;
        TRACE_MSG(TRACE_MAC1, "<<read_from_wpan %d", (FMT__D, ret));
        return ret;
    }

    if ( !ZB_BUF_LEN(ZIG->ioctx.recv_data_buf) ){
        char recvBuf[MAX_PACKET_LEN];

        // read data from socket
#ifdef ZB_WPAN_USE_802154
        bts_read = recv(ZIG->ioctx.sd, recvBuf, MAX_PACKET_LEN, 0);
#endif

#ifdef ZB_WPAN_USE_UDP

        struct ZB_WPAN_SOCK_ADDR_T inAddr;
        inAddr.sin_family = AF_INET;
        inAddr.sin_port = htons(ZIG->ioctx.sport);
        inAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        socklen_t addrLen = sizeof(struct ZB_WPAN_SOCK_ADDR_T);

        // read into recvBuf first
        bts_read = recvfrom(ZIG->ioctx.sd, recvBuf, MAX_PACKET_LEN, 0, (struct sockaddr *)&inAddr, &addrLen);
#endif

        if(bts_read > MAX_PACKET_LEN){
            printf("%s\n", "packet received from WPAN is larger than MAX_PACKET_LEN");
            WPAN_ABORT;
        }

        if(bts_read < 0){
            perror("error when reading from socket");
            WPAN_ABORT;
        }

        // allocate buffer
        zb_uint8_t *ptr = NULL;
        ZB_BUF_INITIAL_ALLOC(ZIG->ioctx.recv_data_buf, bts_read + 1, ptr);

        // copy from recvBuf to actual buffer
        memcpy(ZB_BUF_BEGIN(ZIG->ioctx.recv_data_buf) + 1, recvBuf, bts_read);

        // set the first byte to packet length
        *(unsigned char*)ZB_BUF_BEGIN(ZIG->ioctx.recv_data_buf) = (unsigned char)bts_read;

        TRACE_MSG(TRACE_MAC1, "received packet from wpan (len: %d)", (FMT__D, bts_read));

#ifdef ZB_NS_BUILD
        {
            // simulate hardware behavior on RX
            zb_uint8_t *p;
            /* If got data from ns-3, imitate UZ trailer (9b) */
            ZB_BUF_ALLOC_RIGHT(ZIG->ioctx.recv_data_buf, ZB_MAC_EXTRA_DATA_SIZE, p);

            /* imitate 9b of LQI, RSSI, Frame timer, Superframe counter */
            ZB_MEMSET(p, 0, ZB_MAC_EXTRA_DATA_SIZE);
        }
#endif

        ZIG->ioctx.recv_data_buf = NULL;
        ZB_MAC_STOP_IO();
    }

    if ( bts_read ){
        ret = bts_read;
    }
    TRACE_MSG(TRACE_MAC1, "<<read_from_wpan %d", (FMT__D, ret));
    return ret;
}


zb_ret_t write_to_wpan()
{
    zb_ret_t ret = 0;
    int bts_written = 0;

    TRACE_MSG(TRACE_MAC1, ">>write_to_wpan", (FMT__0));

    if ( ZIG->ioctx.send_data_buf )
    {

        TRACE_MSG(TRACE_MAC1, "writing to wpan data length: %d", (FMT__D, ZB_BUF_LEN(ZIG->ioctx.send_data_buf)));
#ifdef ZB_WPAN_USE_802154
        // send data thru socket
        bts_written = send(ZIG->ioctx.sd, ZB_BUF_BEGIN(ZIG->ioctx.send_data_buf), ZB_BUF_LEN(ZIG->ioctx.send_data_buf), 0);
#endif
#ifdef ZB_WPAN_USE_UDP
        struct ZB_WPAN_SOCK_ADDR_T addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ZIG->ioctx.to_ip;
        addr.sin_port = htons(ZIG->ioctx.dport);

        bts_written = sendto(ZIG->ioctx.sd, ZB_BUF_BEGIN(ZIG->ioctx.send_data_buf),
                ZB_BUF_LEN(ZIG->ioctx.send_data_buf), 0, (struct sockaddr *)&addr,
                sizeof(struct ZB_WPAN_SOCK_ADDR_T));
#endif


        if(bts_written < 0){
            perror("error when reading from socket");
            WPAN_ABORT;

        }
        if(bts_written != ZB_BUF_LEN(ZIG->ioctx.send_data_buf)){
            printf("%s\n", "unable to send the entire packet");
            WPAN_ABORT;
        }

        // notify mac
        TRACE_MSG(TRACE_MAC1, "out buffer is sent", (FMT__0));
        ZB_SET_SEND_STATUS(ZB_SEND_FINISHED);
        ZIG->ioctx.send_data_buf = NULL;
    }

    if ( ret >= 0 && bts_written )
    {
        ret = bts_written;
    }
    TRACE_MSG(TRACE_MAC1, "<<write_to_wpan %d", (FMT__D, bts_written));
    return ret;
}

void zb_mac_transport_put_data(zb_buf_t *buf){
    TRACE_MSG(TRACE_MAC1, ">>zb_mac_put_data buf %p", (FMT__P, buf));

    /* check if we already have buffer for output */
    if ( ZIG->ioctx.send_data_buf )
    {
        TRACE_MSG(TRACE_ERROR, "output buffer is already in progress", (FMT__0));
        ZB_ASSERT(0);
    }
    else
    {
        ZIG->ioctx.send_data_buf = buf;

        ZB_DUMP_OUTGOING_DATA(buf);

        /* call to write data here - it will work like 8051 sync logic for send */
        write_to_wpan();

        MAC_CTX().tx_cnt++;
    }

    TRACE_MSG(TRACE_MAC1, "<<zb_mac_put_data ret", (FMT__0));
}

void zb_mac_wait_for_ext_event()
{
    zb_ret_t ret = 0;
    zb_uint8_t done_smsng;
    TRACE_MSG(TRACE_MAC1, ">>zb_mac_wait_for_ext_event", (FMT__0));


    /* send data to wpan if we have smth to write. If written something, no need
     * to read for recv - continue processing in the main loop. */
    if ((done_smsng = write_to_wpan()) == 0 && ZIG->ioctx.sd != -1 )
    {
        struct timeval tv;
        static struct timeval start_t; /* static to take into account time we spent
                                    * not here */
        fd_set read_set;
        int maxfd;
        zb_time_t tmo = (zb_time_t)~0;


        // see if there is something to read...

        /* fill strucutes for select */
        FD_ZERO(&read_set);
        FD_SET(ZIG->ioctx.sd, &read_set);
        maxfd = ZIG->ioctx.sd;

        /* Implement stack timer: track time we spent sleepint inside select() */
        TRACE_MSG(TRACE_MAC3, "timer started = %d", (FMT__D, ZB_TIMER_CTX().started));
        if (ZB_TIMER_CTX().started)
        {
            if (ZB_TIME_GE(ZB_TIMER_CTX().timer_stop, ZB_TIMER_CTX().timer) && ZB_TIMER_CTX().timer_stop != ZB_TIMER_CTX().timer)
            {
                tmo = ZB_TIME_SUBTRACT(ZB_TIMER_CTX().timer_stop, ZB_TIMER_CTX().timer);
                TRACE_MSG(TRACE_MAC3, "timer stop %d timer %d  tmo %d", (FMT__D_D_D, ZB_TIMER_CTX().timer_stop, ZB_TIMER_CTX().timer, tmo));
            }
        }

        if (tmo != (zb_time_t)~0)
        {
            tv.tv_sec = ZB_TIME_BEACON_INTERVAL_TO_MSEC(tmo) / 1000;
            tv.tv_usec = (ZB_TIME_BEACON_INTERVAL_TO_MSEC(tmo) % 1000) * 1000;
        }
        else
        {
            TRACE_MSG(TRACE_MAC3, "default timeout %d", (FMT__D, ZIG->ioctx.timeout));
            tv.tv_sec = ZIG->ioctx.timeout;
            tv.tv_usec = 0;
        }

        TRACE_MSG(TRACE_MAC3, "select() timeout %d.%d", (FMT__D_D, tv.tv_sec, tv.tv_usec));

        /* start time initialization - do it only once */
        if (start_t.tv_sec == 0)
        {
            gettimeofday(&start_t, NULL);
        }

        ret = select(maxfd + 1, &read_set, NULL, NULL, &tv);

        /* deal with time */
        {
            zb_time_t delta;
            struct timeval end_t;

            gettimeofday(&end_t, NULL);
            /* remember time we spent in select(), msec */
            delta = (((zb_int_t)(end_t.tv_sec - start_t.tv_sec)) * 1000 +
                     ((zb_int_t)(end_t.tv_usec - start_t.tv_usec)) / 1000);
            TRACE_MSG(TRACE_MAC3, "delta %d (%d)", (FMT__D_D, delta, ZB_MILLISECONDS_TO_BEACON_INTERVAL(delta)));
            if (ZB_TIMER_CTX().started)
            {
                /* imitate 8051: move timer if it started only */
                ZB_TIMER_CTX().timer = ZB_TIME_ADD(ZB_TIMER_CTX().timer,
                                                   ZB_MILLISECONDS_TO_BEACON_INTERVAL(delta + ZIG->ioctx.time_delta_rest_ms + ZIG->ioctx.time_delta_rest_us / 1000));
                if (ZB_MILLISECONDS_TO_BEACON_INTERVAL(delta + ZIG->ioctx.time_delta_rest_ms) == 0)
                {
                    ZIG->ioctx.time_delta_rest_ms = delta + ZIG->ioctx.time_delta_rest_ms;
                    ZIG->ioctx.time_delta_rest_us += abs((zb_int_t)(end_t.tv_usec - start_t.tv_usec));
                }
                else
                {
                    ZIG->ioctx.time_delta_rest_ms = 0;
                    ZIG->ioctx.time_delta_rest_us = 0;
                }
            }
            else
            {
                TRACE_MSG(TRACE_MAC3, "timer is not started", (FMT__0));
            }
            TRACE_MSG(TRACE_MAC3, "timer %d, delta_rest %d", (FMT__D_D, ZB_TIMER_CTX().timer, ZIG->ioctx.time_delta_rest_ms));
            /* remember current time for the next run */
            start_t = end_t;
        }

        if ( ret >= 0 )
        {
            if ( FD_ISSET(ZIG->ioctx.sd, &read_set) )
            {
                done_smsng = 1;
                ret = read_from_wpan();
                if ( ret == 0 )
                {
                    /*
                    close(ZIG->ioctx.rpipe);
                    TRACE_MSG(TRACE_MAC1, "seems rpipe is closed from the other side, reopen it", (FMT__0));
                    ZIG->ioctx.rpipe = open(ZIG->ioctx.rpipe_path, O_RDONLY | O_NONBLOCK);
                    TRACE_MSG(TRACE_MAC1, "open rpipe %d. %s", (FMT__D, ZIG->ioctx.rpipe, strerror(errno)));
                     */
                    printf("%s\n", "error: received 0 bytes!");
                    TRACE_MSG(TRACE_MAC1, "read_from_wpan returns 0 bytes", (FMT__0));
                    WPAN_ABORT;
                }
            }
            else if ( ret == 0 )
            {
                TRACE_MSG(TRACE_MAC1, "timeout", (FMT__0));
            }
        }
        else
        {
            TRACE_MSG(TRACE_MAC1, "select error %d. %s", (FMT__D, ret, strerror(errno)));
        }
    }
    if (done_smsng)
    {
        TRACE_MSG(TRACE_MAC1, "Schedule mac main loop 2", (FMT__0));
    }

    TRACE_MSG(TRACE_MAC1, "<<zb_mac_wait_for_ext_event %d", (FMT__D, ret));
}


#endif