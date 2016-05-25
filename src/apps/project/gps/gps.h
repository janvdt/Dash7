/* * GPS
 */


#include "types.h"
#include "link_c.h"


__LINK_C void uart_receive_gps(uint8_t byte);

__LINK_C void readout_fifo_gps();

__LINK_C void clear_fifo_gps();

__LINK_C void init_gps();

__LINK_C void enable_gps();

__LINK_C void shut_down_gps();

__LINK_C void set_latitude(uint32_t new_latitude);

__LINK_C void set_longitude(uint32_t new_longitude);

__LINK_C int get_longitude();

__LINK_C int get_latitude();

__LINK_C bool gps_is_fixed();

__LINK_C void gps_fix_failed();
