#include "mgos.h"

//static void led_timer_cb(void *arg) {
//    bool val = mgos_gpio_toggle(mgos_sys_config_get_pins_led());
//    LOG(LL_INFO, ("%s uptime: %.2lf, RAM: %lu, %lu free", val ? "Tick" : "Tock",
//                mgos_uptime(), (unsigned long) mgos_get_heap_size(),
//                (unsigned long) mgos_get_free_heap_size()));
//    (void) arg;
//}

enum mgos_app_init_result mgos_app_init(void) {
  return MGOS_APP_INIT_SUCCESS;
}
