#include <tcp_client.h>
#include <main.h> 
#include "lwip/tcp.h"
#include <string.h>

#define TCPREVDATALEN 2048
struct tcp_client_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};

enum tcp_client_states
{
  CS_NONE = 0,
  CS_CONNECTED,
  CS_RECEIVED,
  CS_CLOSING
};

static uint8_t recevRxBufferTcpEth[TCPREVDATALEN];

static struct tcp_pcb *tcp_client_pcb;
static struct tcp_client_struct *cs0;


static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_client_send(struct tcp_pcb *tpcb,struct tcp_client_struct *cs); 
static void  tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_struct *cs);
static void  tcp_client_error(void *arg, err_t err);

static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){
		
	struct tcp_client_struct *cs;
	err_t errt = ERR_OK;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	cs = (struct tcp_client_struct*) arg;
	
	if( p == NULL ){ //对方请求关闭连接
		
		cs->state = CS_CLOSING;
		if(cs->p == NULL){//这边也没有要发的了
			tcp_client_close(tpcb,cs);
		}
		errt = ERR_OK;
	}else if (err != ERR_OK){ //下层出问题了
		
		cs->p = NULL;
		pbuf_free(p);
		errt = err;
	}else if (cs->state == CS_CONNECTED){ //没有实现buffer chain
		
		memset(recevRxBufferTcpEth,0,TCPREVDATALEN);
		if(p->len > TCPREVDATALEN){
			printf("warning: p->len > TCPREVDATALEN\n");
			p->len = TCPREVDATALEN;
		}
		memcpy(&recevRxBufferTcpEth,p->payload,p->len);
		pbuf_free(p);
		errt = ERR_OK;
		tcp_recved(tpcb,p->tot_len);
	}else { //data received when connection is already closed
		tcp_recved(tpcb,p->tot_len);
		pbuf_free(p);
		errt = ERR_OK;
	}
	
	
	return errt;
}


static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err){
	printf("tcp_client_connected\n");
	err_t errt = ERR_OK;
	struct tcp_client_struct *cs;
	LWIP_UNUSED_ARG(arg);
	if(err == ERR_MEM){
		tcp_client_close(tpcb,cs);
	}
	cs = mem_malloc(sizeof(struct tcp_client_struct));
	if (cs == NULL){
		errt = ERR_MEM;
		return errt;
	}
	cs->pcb   = tpcb;
	cs->p     = NULL;
	cs->state = CS_CONNECTED;
	cs0 = cs;
	tcp_arg(tpcb,cs);
	tcp_recv(tpcb,tcp_client_recv);
	return errt;

}

static void tcp_client_close(struct tcp_pcb *tpcb, struct tcp_client_struct *cs){
	
	tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (cs != NULL) {
		if(cs->p != NULL){
			pbuf_free(cs->p);
		}
		cs0 = NULL;
    mem_free(cs);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);

}


static void  tcp_client_send(struct tcp_pcb *tpcb,struct tcp_client_struct *cs){

  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (cs->p != NULL) && 
         (cs->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = cs->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK) {
      u16_t plen;

      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      cs->p = ptr->next;
      
      if (cs->p != NULL) {
        /* increment reference count for es->p */
        pbuf_ref(cs->p);//成功获取指针了，就要增加引用计数器
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr); //这个是链式的

      /* Update tcp window size to be advertized : should be called when received
      data (with the amount plen) has been processed by the application layer */
      tcp_recved(tpcb, plen);
   } else if(wr_err == ERR_MEM) {
      /* we are low on memory, try later / harder, defer to poll */
     cs->p = ptr;
   } else {
     printf("ERROR:tcp_client_send unknown error!\n");
   }
  }
}

static void tcp_client_error(void *arg, err_t err)
{

  struct tcp_client_struct *cs;

  LWIP_UNUSED_ARG(err);

  cs = (struct tcp_client_struct *)arg;
  if (cs != NULL) {
    /*  free es structure */
    mem_free(cs);
  }
}

void tcp_client_init(void){
	err_t errt = ERR_OK;
	ip4_addr_t addr;
	
	tcp_client_pcb = tcp_new();
	
	if(tcp_client_pcb != NULL){
		IP4_ADDR(&addr,192,168,3,3);
		errt = tcp_connect(tcp_client_pcb,&addr,1000,tcp_client_connected);
		tcp_err(tcp_client_pcb,tcp_client_error);
	
	}

}




uint8_t tcp_client_send_message(void * msg, uint32_t len){
	uint8_t count = 0;
	struct pbuf *p;
	if(cs0->state !=CS_CONNECTED){
		return 1;
	}
	if(cs0->p == NULL){
		cs0->p = pbuf_alloc(PBUF_TRANSPORT,len,PBUF_RAM);
		if(cs0->p == NULL){
			printf("ERROR:tcp send message pbuf alloc failed!\n");
		}
		pbuf_take(cs0->p,(char*)msg,len);
	}
	tcp_client_send(cs0->pcb,cs0);
	
	return 0;
	
}
	
