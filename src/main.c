#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_rpc.h"
#include "mgos_mqtt.h"
#ifdef MGOS_HAVE_WIFI
#include "mgos_wifi.h"
#endif

#define RELAY_PIN 5

static void timer_cb() {
    mgos_mqtt_pub("my/topic", "hi", 2, 1, 0); 
}



#ifdef MGOS_HAVE_WIFI
static void wifi_cb(int ev, void *evd, void *arg) {
    switch (ev) {
        case MGOS_WIFI_EV_STA_DISCONNECTED: {
                                                struct mgos_wifi_sta_disconnected_arg *da =
                                                    (struct mgos_wifi_sta_disconnected_arg *) evd;
                                                LOG(LL_INFO, ("WiFi STA disconnected, reason %d", da->reason));
                                                break;
                                            }
        case MGOS_WIFI_EV_STA_CONNECTING:
                                            LOG(LL_INFO, ("WiFi STA connecting %p", arg));
                                            break;
        case MGOS_WIFI_EV_STA_CONNECTED:
                                            LOG(LL_INFO, ("WiFi STA connected %p", arg));
                                            break;
        case MGOS_WIFI_EV_STA_IP_ACQUIRED:
                                            LOG(LL_INFO, ("WiFi STA IP acquired %p", arg));
                                            break;
        case MGOS_WIFI_EV_AP_STA_CONNECTED: {
                                                struct mgos_wifi_ap_sta_connected_arg *aa =
                                                    (struct mgos_wifi_ap_sta_connected_arg *) evd;
                                                LOG(LL_INFO, ("WiFi AP STA connected MAC %02x:%02x:%02x:%02x:%02x:%02x",
                                                            aa->mac[0], aa->mac[1], aa->mac[2], aa->mac[3], aa->mac[4],
                                                            aa->mac[5]));
                                                break;
                                            }
        case MGOS_WIFI_EV_AP_STA_DISCONNECTED: {
                                                   struct mgos_wifi_ap_sta_disconnected_arg *aa =
                                                       (struct mgos_wifi_ap_sta_disconnected_arg *) evd;
                                                   LOG(LL_INFO,
                                                           ("WiFi AP STA disconnected MAC %02x:%02x:%02x:%02x:%02x:%02x",
                                                            aa->mac[0], aa->mac[1], aa->mac[2], aa->mac[3], aa->mac[4],
                                                            aa->mac[5]));
                                                   break;
                                               }
    }
    (void) arg;
}
#endif /* MGOS_HAVE_WIFI */
static void relay_toggle_handler (struct mg_rpc_request_info *ri, void *cb_arg,
        struct mg_rpc_frame_info *fi, struct mg_str args){
    struct mbuf fb;
    struct json_out out = JSON_OUT_MBUF(&fb);
    mbuf_init(&fb, 20);
    if(args.p != NULL)
        printf("args not null");

    if(fi != NULL)
        printf("fi not null");
    if(ri != NULL)
        printf("ri not null");
    if(cb_arg != NULL)
        printf("cb_arg not null");

    mgos_gpio_toggle(RELAY_PIN);
    json_printf(&out, "OK");
    mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
    ri = NULL;

    mbuf_free(&fb);
}

static void relay_on_handler (struct mg_rpc_request_info *ri, void *cb_arg,
        struct mg_rpc_frame_info *fi, struct mg_str args){
    struct mbuf fb;
    struct json_out out = JSON_OUT_MBUF(&fb);
    mbuf_init(&fb, 20);
    if(args.p != NULL)
        printf("args not null");

    if(fi != NULL)
        printf("fi not null");
    if(ri != NULL)
        printf("ri not null");
    if(cb_arg != NULL)
        printf("cb_arg not null");

    mgos_gpio_write(RELAY_PIN,1);
    json_printf(&out, "ON");
    mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
    ri = NULL;

    mbuf_free(&fb);
}

static void relay_off_handler (struct mg_rpc_request_info *ri, void *cb_arg,
        struct mg_rpc_frame_info *fi, struct mg_str args){
    struct mbuf fb;
    struct json_out out = JSON_OUT_MBUF(&fb);
    mbuf_init(&fb, 20);
    if(args.p != NULL)
        printf("args not null");

    if(fi != NULL)
        printf("fi not null");
    if(ri != NULL)
        printf("ri not null");
    if(cb_arg != NULL)
        printf("cb_arg not null");

    mgos_gpio_write(RELAY_PIN,0);
    json_printf(&out, "OFF");
    mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
    ri = NULL;

    mbuf_free(&fb);
}

static void inc_handler(struct mg_rpc_request_info *ri, void *cb_arg,
        struct mg_rpc_frame_info *fi, struct mg_str args) {
    struct mbuf fb;
    struct json_out out = JSON_OUT_MBUF(&fb);

    mbuf_init(&fb, 20);

    int num = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &num) == 1) {
        json_printf(&out, "{num: %d}", num + 1);
    } else {
        json_printf(&out, "{error: %Q}", "num is required");
    }

    mgos_gpio_toggle(mgos_sys_config_get_board_led1_pin());

    printf("%d + 1 = %d\n", num, num + 1);

    mg_rpc_send_responsef(ri, "%.*s", fb.len, fb.buf);
    ri = NULL;

    mbuf_free(&fb);

    (void) cb_arg;
    (void) fi;
}

static void mqtt_handler(struct mg_connection *c, const char *topic, int topic_len,const char *msg, int msg_len, void *userdata) {
    
    LOG(LL_INFO, ("Got message on topic %.*s: %.*s\nLen:%d", topic_len, topic, msg_len, msg, msg_len));
    if(strncmp(msg,"on", msg_len)==0){
        LOG(LL_INFO, ("ON"));
        mgos_gpio_write(RELAY_PIN,1);
    }
    
    if(strncmp(msg,"off", msg_len)==0){
        LOG(LL_INFO, ("OFF"));
        mgos_gpio_write(RELAY_PIN,0);
    }
    
  //mgos_gpio_toggle(RELAY_PIN);
  
    if(c==NULL || topic == NULL || topic_len==0 || msg==NULL || msg_len==0 || userdata==NULL)
        return;
}

enum mgos_app_init_result mgos_app_init(void) {
    struct mg_rpc *c = mgos_rpc_get_global();

    mg_rpc_add_handler(c, "Example.Increment", "{num: %d}", inc_handler, NULL);
    mg_rpc_add_handler(c, "Relay.Toggle", NULL , relay_toggle_handler, NULL);
    mg_rpc_add_handler(c, "Relay.On", NULL , relay_on_handler, NULL);
    mg_rpc_add_handler(c, "Relay.Off", NULL , relay_off_handler, NULL);

    /* Simple repeating timer */
    mgos_set_timer(5000, MGOS_TIMER_REPEAT, timer_cb, NULL);

    mgos_gpio_setup_output(RELAY_PIN,0);
    mgos_mqtt_sub("test/#", mqtt_handler, NULL);
    

#ifdef MGOS_HAVE_WIFI
    mgos_event_add_group_handler(MGOS_WIFI_EV_BASE, wifi_cb, NULL);
#endif


    return MGOS_APP_INIT_SUCCESS;
}
