/* * GPS
 */


#include "types.h"
#include "link_c.h"


__LINK_C void uart_receive_gps(uint8_t byte);

__LINK_C void readout_fifo_gps();

__LINK_C void clear_fifo_gps();

__LINK_C void init_fifo_gps();
