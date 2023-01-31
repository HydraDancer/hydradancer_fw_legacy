#include <stdarg.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "serdes.h"
#include "usb20.h"

#include "log.h"

void
log_to_evaluator(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    if (bsp_switch()) {
        // If is top board
        usb_log("[BOARD TOP]    "); // Add prefix for top board
        usb_log(fmt, ap);
    } else {
        // Else is bottom board
        serdes_log("[BOARD BOTTOM] "); // Add prefix for bottom board
        serdes_log(fmt, ap);
    }

}
