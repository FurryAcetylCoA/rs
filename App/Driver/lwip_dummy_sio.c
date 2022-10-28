
/*  sio在LWIP的SLIP模块中被使用
 *  该模块的功能是使用串口网卡
 *  如果不需要串口网卡，本应提供dummy实现
 *  但cube带的LWIP对AC6编译器支持不好，导致dummy实现不会被编译
 *  所以我这里手动复制了一个补上
 *  以下代码复制自lwip.c
 */

#include "lwip/sio.h"

/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  sio_fd_t sd;
	LWIP_UNUSED_ARG(devnum);
  sd = 0; // dummy code


  return sd;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
		LWIP_UNUSED_ARG(c);
	  LWIP_UNUSED_ARG(fd);
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
	LWIP_UNUSED_ARG(fd);
	LWIP_UNUSED_ARG(data);
	LWIP_UNUSED_ARG(len);	

  return 0;
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
	
	LWIP_UNUSED_ARG(fd);
	LWIP_UNUSED_ARG(data);
	LWIP_UNUSED_ARG(len);	

  return 0;
}
