#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "settings_from_pipe.h"

#ifndef ZB_TRANSPORT_USE_LINUX_WPAN
#error "only runs in Linux WPAN mode"
#endif

#define ZB_TEST_DUMMY_DATA_SIZE 10

/*
 * 1. needs network address
 * 2. needs PAN ID (for coordinators)
 * 3. needs role
 */

/*! \addtogroup ZB_TESTS */
/*! @{ */

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif


#define PARAM_CHAR_SIZE 100

/*
  The test is: ZC starts PAN, ZR joins to it by association and send APS data packet, when ZC
  received packet, it sends packet to ZR, when ZR received packet, it sends
  packet to ZC etc.
 */

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr);

void data_indication(zb_uint8_t param) ZB_CALLBACK;

MAIN() {
    ARGV_UNUSED;

    // fetch config from pipe
    void *params = read_and_parse_stdin();

    // fetch device address
    load_item_into_buffer(params, "device-mac");
    _eightbytes macAddress = get_ieee_addr_from_buffer();

    // fetch wpan interface name
    char *wpanName = malloc(PARAM_CHAR_SIZE);
    load_item_into_buffer(params, "wpan-interface");
    strcpy(wpanName, get_item_buffer());

    // fetch device role
    char *deviceRole = malloc(PARAM_CHAR_SIZE);
    load_item_into_buffer(params, "device-role");
    strcpy(deviceRole, get_item_buffer());

    // fetch PAN id
    unsigned short panId = 0;
    load_item_into_buffer(params, "pan-id");
    panId = get_pan_id_from_buffer();

    free_parsed_stdin(params);

    if(strcmp(deviceRole, "coordinator") != 0){
        printf("%s\n", "this program must run as coordinator!");
        exit(1);
    }


    // TODO: call init
    ZB_INIT("zdo_zc", wpanName);

#ifdef ZB_SECURITY
    ZG->nwk.nib.security_level = 0;
#endif

#ifdef ZB_TRANSPORT_LINUX_PIPES
    ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &macAddress);
    MAC_PIB().mac_pan_id = panId;
#endif

    /* let's always be coordinator */
    ZB_AIB().aps_designated_coordinator = 1;

    if (zdo_dev_start() != RET_OK) {
        TRACE_MSG(TRACE_ERROR, "zdo_dev_start failed", (FMT__0));
    } else {
        zdo_main_loop();
    }

    TRACE_DEINIT();

    MAIN_RETURN(0);
}

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);
    TRACE_MSG(TRACE_APS3, ">>zb_zdo_startup_complete status %d", (FMT__D, (int) buf->u.hdr.status));
    if (buf->u.hdr.status == 0) {
        TRACE_MSG(TRACE_APS1, "Device STARTED OK", (FMT__0));
        zb_af_set_data_indication(data_indication);
    } else {
        TRACE_MSG(TRACE_ERROR, "Device start FAILED status %d", (FMT__D, (int) buf->u.hdr.status));
    }
    zb_free_buf(buf);
}


/*
   Trivial test: dump all APS data received
 */


void data_indication(zb_uint8_t param) ZB_CALLBACK
{
    zb_uint8_t *ptr;
    zb_buf_t *asdu = (zb_buf_t *) ZB_BUF_FROM_REF(param);
    zb_apsde_data_indication_t *ind = ZB_GET_BUF_PARAM(asdu, zb_apsde_data_indication_t);

    /* Remove APS header from the packet */
    ZB_APS_HDR_CUT_P(asdu, ptr);

    TRACE_MSG(TRACE_APS3, "apsde_data_indication: packet %p len %d handle 0x%x", (FMT__P_D_D,
            asdu, (int) ZB_BUF_LEN(asdu), asdu->u.hdr.status));

    /* send packet back to ZR */
    zc_send_data(asdu, ind->src_addr);
}

static void zc_send_data(zb_buf_t *buf, zb_uint16_t addr) {
    zb_apsde_data_req_t *req;
    zb_uint8_t *ptr = NULL;
    zb_short_t i;

    ZB_BUF_INITIAL_ALLOC(buf, ZB_TEST_DUMMY_DATA_SIZE, ptr);
    req = ZB_GET_BUF_TAIL(buf, sizeof(zb_apsde_data_req_t));
    req->dst_addr.addr_short = addr; /* send to ZR */
    req->addr_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    req->tx_options = ZB_APSDE_TX_OPT_ACK_TX;
    req->radius = 1;
    req->profileid = 2;
    req->src_endpoint = 10;
    req->dst_endpoint = 10;
    buf->u.hdr.handle = 0x11;
    for (i = 0; i < ZB_TEST_DUMMY_DATA_SIZE; i++) {
        ptr[i] = i + '0';
    }
    TRACE_MSG(TRACE_APS3, "Sending apsde_data.request", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_apsde_data_request, ZB_REF_FROM_BUF(buf));
}

/*! @} */
