#ifndef __AES_CONFIG_H__
#define __AES_CONFIG_H__

#include <Arduino.h>
#define printf_P printf

#define N_ROW                   4
#define N_COL                   4
#define N_BLOCK   (N_ROW * N_COL)
#define N_MAX_ROUNDS           14
#define KEY_SCHEDULE_BYTES ((N_MAX_ROUNDS + 1) * N_BLOCK)
#define SUCCESS (0)
#define FAILURE (-1)

#endif
