#include "mbus-protocol.h"

unsigned char calc_checksum(mbus_frame *frame) {
    size_t i;
    unsigned char cksum = MBUS_FRAME_START;

    cksum += frame->meter_type;
    for (i = 0; i < MBUS_ADDR_LEN; i++) {
        cksum += frame->addr[i];
    }
    cksum += frame->control;
    cksum += frame->data_len;
    for (i = 0; i < frame->data_len; i++) {
        cksum += frame->data[i];
    }
    return cksum;
}

int mbus_frame_pack(mbus_frame *frame, unsigned char *data, size_t data_size)
{
    size_t offset = 0;

    if (!frame && !data) {
        return -1;
    }

    frame->checksum = calc_checksum(frame);

    data[offset++] = 0xFE;
    data[offset++] = 0xFE;
    data[offset++] = MBUS_FRAME_START;
    data[offset++] = frame->meter_type;
    memcpy(&data[offset],  frame->addr, MBUS_ADDR_LEN);
    offset += MBUS_ADDR_LEN;
    data[offset++] = frame->control;
    data[offset++] = frame->data_len;
    memcpy(&data[offset],  frame->data, frame->data_len);
    offset += frame->data_len;  
    data[offset++] = frame->checksum;
    data[offset++] = MBUS_FRAME_STOP;
    return offset;
}

int mbus_parse(mbus_frame *frame, unsigned char *data, size_t data_size) {
    size_t len;

    if (!frame ||!data || data_size < (MBUS_DATA_STAT_POS + 2 + 1))
        return -1;

    if (MBUS_FRAME_START != data[0]) {
        return -4;
    }

    if (MBUS_FRAME_STOP != data[data_size - 1]) {
        return -4;
    }

    len = data[MBUS_DATA_LEN_POS];
    if ((len + MBUS_DATA_STAT_POS + 2) != data_size) {
        return -4;
    }

    frame->meter_type = data[MBUS_METER_TYPE_POS];
    frame->control    = data[MBUS_CTRL_POS];
    frame->data_len   = len;
    frame->checksum   = data[MBUS_DATA_STAT_POS + len];
    memcpy(frame->addr, &data[MBUS_ADDR_POS], MBUS_ADDR_LEN);
    memcpy(frame->data, &data[MBUS_DATA_STAT_POS], len);
    if (frame->checksum != calc_checksum(frame)) {
        return -4;
    }
    return 0;
}
