/* * GPS
 */


#include "types.h"
#include "link_c.h"



__LINK_C void uart_receive_gps(uint8_t byte);

__LINK_C void uart_init_gps();

__LINK_C void readout_fifo_gps();

__LINK_C void clear_fifo_gps();

//__LINK_C void fifo_init(fifo_t * f, uint8_t * buffer, int size);
