#include "CH56x_debug_log.h"

#include "CH56x_usb30_util.h"


void
cprintf_st_cusp(PUSB_SETUP st_pusb) {
    cprintf("bRequestType: %d\r\n", st_pusb->bRequestType);
    cprintf("bRequest    : %d\r\n", st_pusb->bRequest    );
    cprintf("wValue      : %d\r\n", st_pusb->wValue      );
    cprintf("wIndex      : %d\r\n", st_pusb->wIndex      );
    cprintf("wLength     : %d\r\n", st_pusb->wLength     );
}
