#ifndef BL_BOARD_XTRA_H
#define BL_BOARD_XTRA_H

#include <pios.h>

extern uintptr_t pios_com_telem_serial_id;
//#define PIOS_COM_TELEM_USB        (pios_com_telem_serial_id)

extern bool PIOS_USB_HaveVSense(uintptr_t id);
extern bool PIOS_USB_CableConnected(uintptr_t id);

#endif //BL_BOARD_XTRA