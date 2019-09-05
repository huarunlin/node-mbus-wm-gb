#ifndef _MBUS_PROTOCOL_H_
#define _MBUS_PROTOCOL_H_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MBUS_FRAME_START           0x68
#define MBUS_FRAME_STOP            0x16

#define MBUS_ADDR_LEN              7
#define MBUS_FRAME_DATA_LENGTH     256 

#define MBUS_METER_TYPE_POS        (1)
#define MBUS_ADDR_POS              (2)
#define MBUS_CTRL_POS              (MBUS_ADDR_POS + MBUS_ADDR_LEN)
#define MBUS_DATA_LEN_POS          (MBUS_CTRL_POS + 1)
#define MBUS_DATA_STAT_POS         (MBUS_DATA_LEN_POS + 1)

#define MBUS_DEFAULT_METER_TYPE     (0x10)

typedef struct _mbus_frame {
    unsigned char   meter_type;
    unsigned char   addr[MBUS_ADDR_LEN];
    unsigned char   control;
    unsigned char   data_len;
    unsigned char   data[MBUS_FRAME_DATA_LENGTH];
    unsigned char   checksum;
} mbus_frame;

int mbus_frame_pack(mbus_frame *frame, unsigned char *data, size_t data_size);
int mbus_parse(mbus_frame *frame, unsigned char *data, size_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* _MBUS_PROTOCOL_H_ */
