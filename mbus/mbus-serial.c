#ifdef _WIN32
//window
#include <string.h>
#include <BaseTsd.h>
#else
//linux
#include <unistd.h>
#include <strings.h>
#include <string.h>
#endif

#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#include "mbus-serial.h"
#include "mbus-protocol.h"

#define MBUS_ERROR(...) fprintf (stderr, __VA_ARGS__)
#ifdef DEBUG
#define MBUS_SERIAL_DEBUG
#endif

#define PACKET_BUFF_SIZE 300

#ifdef _WIN32
#ifndef SSIZE_MAX
#ifdef _WIN64
#define SSIZE_MAX _I64_MAX
#else
#define SSIZE_MAX LONG_MAX
#endif
#endif
#define __PRETTY_FUNCTION__       __FUNCTION__
#define O_NOCTTY 0x0000 // No idea if this makes sense
typedef SSIZE_T ssize_t;
#define read(...)       readFromSerial(__VA_ARGS__)
#define write(...)      writeToSerial(__VA_ARGS__)
#define select(...)     selectSerial(__VA_ARGS__)
#define open(...)       openSerial(__VA_ARGS__)
#define close(...)      closeSerial(__VA_ARGS__)
#endif

char* mbus_serial_get_device(mbus_handle *handle) 
{
    mbus_serial_data *serial_data;

    if (handle == NULL) {
        return NULL;
    }

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL) {
        return NULL;
    }

    return serial_data->device;
}

int  mbus_serial_set_device(mbus_handle *handle, const char *device) 
{
    size_t len;
    mbus_serial_data *serial_data;

    if (handle == NULL || device == NULL) {
        return -1;
    }

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL) {
        return -1;
    }

    len = strlen(device);
    if (len <= 0) {
        return -1;
    }

    if (serial_data->device) {
        free(serial_data->device);
        serial_data->device = NULL;
    }

    serial_data->device = malloc(len + 1);
    memcpy(serial_data->device, device, len + 1);
    return 0;
}

long mbus_serial_get_baudrate(mbus_handle *handle)
{
    mbus_serial_data *serial_data;

    if (handle == NULL)
        return -1; 
    
    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL)
        return -1;

    return serial_data->speed;
}

int mbus_serial_set_baudrate(mbus_handle *handle, long baudrate)
{
    mbus_serial_data *serial_data;

    if (handle == NULL)
        return -1;

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL)
        return -1;

    switch (baudrate) {
        case 300:
            serial_data->speed = B300;
            serial_data->t.c_cc[VTIME] = (cc_t) 13; // Timeout in 1/10 sec
            break;
        case 600:
            serial_data->speed = B600;
            serial_data->t.c_cc[VTIME] = (cc_t) 8;  // Timeout in 1/10 sec
            break;
        case 1200:
            serial_data->speed = B1200;
            serial_data->t.c_cc[VTIME] = (cc_t) 5;  // Timeout in 1/10 sec
            break;
        case 2400:
            serial_data->speed = B2400;
            serial_data->t.c_cc[VTIME] = (cc_t) 3;  // Timeout in 1/10 sec
            break;
        case 4800:
            serial_data->speed = B4800;
            serial_data->t.c_cc[VTIME] = (cc_t) 3;  // Timeout in 1/10 sec
            break;
        case 9600:
            serial_data->speed = B9600;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;
        case 19200:
            serial_data->speed = B19200;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;
        case 38400:
            serial_data->speed = B38400;
            serial_data->t.c_cc[VTIME] = (cc_t) 2;  // Timeout in 1/10 sec
            break;
       default:
            return -1; // unsupported baudrate
    }

    if (handle->fd != -1) {
        // Set input baud rate
        if (cfsetispeed(&(serial_data->t), serial_data->speed) != 0) {
            return -1;
        }
        // Set output baud rate
        if (cfsetospeed(&(serial_data->t), serial_data->speed) != 0) {
            return -1;
        }
        // Change baud rate immediately
        if (tcsetattr(handle->fd, TCSANOW, &(serial_data->t)) != 0) {
            return -1;
        }
    }
    return 0;
}

int mbus_serial_set_format(mbus_handle *handle, int databits, int stopbits, int parity)
{ 
    mbus_serial_data *serial_data;
    struct termios *options;

    if (handle == NULL)
        return -1;

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL)
        return -1;

    options = (struct termios *)&serial_data->t;
    options->c_cflag &= ~CSIZE;  
    /* Setup Databits */
    switch (databits) {  
        case 7:  
            options->c_cflag |= CS7;  
            break;   
        case 8:  
            options->c_cflag |= CS8;  
            break;  
        default:  
            MBUS_ERROR("Unsupported data sizen"); 
            return -1;  
    }  
    /* Setup parity */
    switch (parity) {  
        case 'n':  
        case 'N':  
            options->c_cflag &= ~PARENB; /* Clear parity enable */   
            options->c_iflag &= ~INPCK; /* Enable parity checking */  
            break;  
        case 'o':  
        case 'O':  
            options->c_cflag |= (PARODD | PARENB); /* odd */  
            options->c_iflag |= INPCK; /* Disnable parity checking */  
            break;  
        case 'e':  
        case 'E':  
            options->c_cflag |= PARENB; /* Enable parity */  
            options->c_cflag &= ~PARODD; /* event */   
            options->c_iflag |= INPCK; /* Disnable parity checking */  
            break;  
        case 'S':  
        case 's': /*as no parity*/  
            options->c_cflag &= ~PARENB;  
            options->c_cflag &= ~CSTOPB;break;  
        default:  
            MBUS_ERROR("Unsupported parityn");  
            return -1;
    }
    /* Set Stopbits */
    switch (stopbits) {  
        case 1:  
            options->c_cflag &= ~CSTOPB;  
            break;  
        case 2:  
            options->c_cflag |= CSTOPB;  
            break;  
        default:  
            MBUS_ERROR("Unsupported stop bitsn");  
            return -1;  
    } 
    /* Set input parity option */  
    if (parity != 'n')  
        options->c_iflag |= INPCK;  
  
    if (handle->fd != -1) {
        tcflush(handle->fd, TCIFLUSH);  
        if (tcsetattr(handle->fd, TCSANOW, options) != 0)  {  
            MBUS_ERROR("Setup Serial format failure");  
            return -1;  
        }  
    }
    return 0;
}

int mbus_serial_connect(mbus_handle *handle)
{
    mbus_serial_data *serial_data;
    char *device;
    struct termios *term;

    if (handle == NULL)
        return -1;

    serial_data = (mbus_serial_data *) handle->auxdata;
    if (serial_data == NULL || serial_data->device == NULL)
        return -1;

    device = serial_data->device;
    term = &(serial_data->t);

    // Use blocking read and handle it by serial port VMIN/VTIME setting
    if ((handle->fd = open(device, O_RDWR | O_NOCTTY)) < 0) {
        MBUS_ERROR("%s: failed to open %s.\n", __PRETTY_FUNCTION__, device);
        return -1;
    }   
    cfsetispeed(term, serial_data->speed);
    cfsetospeed(term, serial_data->speed);
    tcsetattr(handle->fd, TCSANOW, term);
    return 0;
}

int mbus_serial_disconnect(mbus_handle *handle)
{
    if (handle == NULL) {
        return -1;
    }

    if (handle->fd < 0) {
        return -1;
    }

    close(handle->fd);
    handle->fd = -1;
    return 0;
}

void mbus_serial_data_init(mbus_handle *handle) 
{
    mbus_serial_data *serial_data;

    if (handle) {
        if (handle->auxdata != NULL) {
            return;
        }
        serial_data = malloc(sizeof(mbus_serial_data));
        serial_data->device = NULL;
        memset(&serial_data->t, 0, sizeof(struct termios));
        serial_data->t.c_cflag |= (CS8|CREAD|CLOCAL);
        serial_data->t.c_cflag |= PARENB;
        serial_data->t.c_cc[VMIN] = (cc_t) 0;
        serial_data->speed = B2400;
        cfsetispeed(&serial_data->t, serial_data->speed);
        cfsetospeed(&serial_data->t, serial_data->speed);
        handle->auxdata = serial_data;
    }
}

void mbus_serial_data_free(mbus_handle *handle)
{
    mbus_serial_data *serial_data;

    if (handle) {
        serial_data = (mbus_serial_data *) handle->auxdata;
        if (serial_data == NULL) {
            return;
        }
        if (serial_data->device) {
            free(serial_data->device);
            serial_data->device = NULL;
        }
        free(serial_data);
        handle->auxdata = NULL;
    }
}

int mbus_serial_send_frame(mbus_handle *handle, mbus_frame *frame)
{
    unsigned char buff[PACKET_BUFF_SIZE];
    int len, ret;

    if (handle == NULL || frame == NULL) {
        return -1;
    }

    #ifdef _WIN32
    if (GetFileType(getHandle()) != FILE_TYPE_CHAR )
    #else
    if (isatty(handle->fd) == 0)
    #endif
    {
        MBUS_ERROR("%s: connection not open\n", __PRETTY_FUNCTION__);
        return -1;
    }

    len = mbus_frame_pack(frame, buff, sizeof(buff));
    if (len <= 0) {
        MBUS_ERROR("%s: mbus_frame_pack failed\n", __PRETTY_FUNCTION__);
        return -1;
    }

    ret = write(handle->fd, buff, len);
    if (ret != len) {
        MBUS_ERROR("%s: Failed to write frame to socket (ret = %d: %s)\n", __PRETTY_FUNCTION__, ret, strerror(errno));
        return -1;
    }

    tcdrain(handle->fd);
    return 0;
}

int mbus_serial_recv_frame(mbus_handle *handle, mbus_frame *frame)
{
    unsigned char buff[PACKET_BUFF_SIZE];
    int timeouts;
    ssize_t len, nread;

    if (handle == NULL || frame == NULL) {
        MBUS_ERROR("%s: Invalid parameter.\n", __PRETTY_FUNCTION__);
        return MBUS_RECV_RESULT_ERROR;
    }

    #ifdef _WIN32
    if (GetFileType(getHandle()) != FILE_TYPE_CHAR )
    #else
    if (isatty(handle->fd) == 0)
    #endif
    {
        MBUS_ERROR("%s: Serial connection is not available.\n", __PRETTY_FUNCTION__);
        return MBUS_RECV_RESULT_ERROR;
    }

    memset((void *)buff, 0, sizeof(buff));

    len = 0;
    timeouts = 0;
    while (1) {
        if (len > PACKET_BUFF_SIZE) {
            return MBUS_RECV_RESULT_ERROR;
        }

        if (len == 0) {
            nread = read(handle->fd, &buff[0], 1);
        } else {
            nread = read(handle->fd, &buff[len], PACKET_BUFF_SIZE - len);
        }
        
        if (nread == -1) {
            return MBUS_RECV_RESULT_ERROR;
        } else if (nread == 0) {
            if (timeouts++ >= 3) {
                return MBUS_RECV_RESULT_TIMEOUT;
            }
        } else {
            timeouts = 0;
        }

        if (len == 0 && MBUS_FRAME_START != buff[0]) {
            continue;
        }

        len += nread;
        if (0 == mbus_parse(frame, buff, len)) {
            return MBUS_RECV_RESULT_OK;
        }
    };
}
