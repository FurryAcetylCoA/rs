#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <main.h>   
#include "lwip.h"   


void    tcp_client_init(void);
uint8_t tcp_client_send_message(void * msg, uint32_t len);
#endif