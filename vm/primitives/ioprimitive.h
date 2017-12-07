#ifndef IO_PRIMITIVE_H
#define IO_PRIMITIVE_H

#include "../gdef.h"
#include "../memory.h"
#include "../names.h"
#ifdef USE_X_MODEM
#include "../../utils/xmodem.h"
#endif
#include "../../io/uart.h"
#include "../../wiring.h"
#include "../../utils/kilo.h"
#include "../../fs/fatfs/ff.h"
#include "../../fs/fatfs/diskio.h"

#define MAXFILES 20
object ioPrimitive(int number, object * arguments);
#endif