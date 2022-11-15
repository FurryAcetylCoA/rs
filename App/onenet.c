//
// Created by 11096 on 2022/11/9.
//

#include "onenet.h"
#include "http_client.h"

static uint8_t current_poll;

static void httpc_result_fn_impl_onenet(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);

void onenet_update(uint8_t n);

void onenet_update_one(){
    //上推某个传感器的数据// 这玩意的速率要不要跟随传感器数量而变化？
    if(current_poll <= This.config.dev_count){
        onenet_update(current_poll);
        current_poll++;
    }else{
        current_poll = 0;
    }
}

void onenet_update(uint8_t n){
    httpc_connection_t httpc_settings= { HTTPC_METHOD_POST ,NULL,0 ,httpc_result_fn_impl_onenet, NULL };
    httpc_state_t *phttpc_state = NULL;
    ip4_addr_t addr;
    IP4_ADDR(&addr,192,168,3,5);
    uint8_t Postbuf[128];
    App_dev_desc   *ndev = &This.devs[n];
    Dev_desc const *ddev = &devDesc[n];
    if(ndev->sens_desc.data2.exist){
        /*httpc_settings.body_len = snprintf((char *) Postbuf, lenof(Postbuf), \
                        "{\"datastreans\": [{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%f\"}]},{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%f\"}]}]}",\
                         ddev->name,ddev->data1_display_name,ndev->data1,ddev->name,ddev->data2_display_name,ndev->data2);
    */
        httpc_settings.body_len = snprintf((char*)Postbuf,lenof(Postbuf),\
                                            "abcd");
    }else {
        httpc_settings.body_len = snprintf((char *) Postbuf, lenof(Postbuf), \
                        "{\"datastreans\": [{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%f\"}]}]}",\
                        ddev->name,ddev->data1_display_name,ndev->data1);
    }
    httpc_settings.post_body =  &(Postbuf);
    httpc_request_file(&addr,80,"/file.php",&httpc_settings,NULL,phttpc_state,&phttpc_state);

}

static void httpc_result_fn_impl_onenet(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err){
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(httpc_result);
    LWIP_UNUSED_ARG(rx_content_len);
    LWIP_UNUSED_ARG(srv_res);
    LWIP_UNUSED_ARG(err);
    printf("httpc_result_fn_impl \n");
    _TRAP;
}