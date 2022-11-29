//
// Created by 11096 on 2022/11/9.
//

#include "onenet.h"
#include "http_client_x.h"
#include "lcd_gxct.h"
#include "App_Config.h"

static uint8_t current_poll;

void httpc_result_fn_impl_onenet(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);
void onenet_update_one();
void onenet_update(uint8_t n);
void onenet_update_all();

Onenet onenet={
        .Poll = onenet_update_one,
        .Pollall = onenet_update_all,
        .PollOne = onenet_update
};

void onenet_update_all(){
    for (int i = 0; i < This.config.dev_count; ++i) {
        if (This.devs[i].onenetDesc.sending == 0){
            This.devs[i].onenetDesc.sending = 1;
            onenet_update(i);
        }

    }
}
void onenet_update_one(){
    //上推某个传感器的数据// 这玩意的速率要不要跟随传感器数量而变化？
    if(current_poll < This.config.dev_count){
        onenet_update(current_poll);
        current_poll++;
    }else{
        current_poll = 0;
    }
}

void onenet_update(uint8_t n){
    //httpc_connetion_t 的生命周期一定要能涵盖到httpc_result_fn_impl_onenet结束为止
    //为了函数的可重入性，干脆让每一个dev都自带一个httpc_settings好了
    This.devs[n].onenetDesc.httpc_settings.method = HTTPC_METHOD_POST;
    This.devs[n].onenetDesc.httpc_settings.result_fn = httpc_result_fn_impl_onenet;
    This.devs[n].onenetDesc.httpc_settings.headers_done_fn = NULL;


    httpc_state_t *phttpc_state = NULL;
    /*ip4_addr_t addr;
    IP4_ADDR(&addr,192,168,17,5);*/
    uint8_t Postbuf[1024];
    App_dev_desc   *ndev = &This.devs[n];
    Dev_desc const *ddev = &devDesc[ndev->sens_desc.name_index];
    if(ndev->sens_desc.data2.exist){
        This.devs[n].onenetDesc.httpc_settings.body_len = \
               snprintf((char *) Postbuf, lenof(Postbuf), \
                        "{\"datastreams\": [{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%.1f\"}]},{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%.1f\"}]}]}",\
                         ddev->name,ddev->data1_name,ndev->data1,ddev->name,ddev->data2_name,ndev->data2);
     }else {
        This.devs[n].onenetDesc.httpc_settings.body_len = \
               snprintf((char *) Postbuf, lenof(Postbuf), \
                        "{\"datastreams\": [{\"id\": \"%s-%s\",\"datapoints\": [{\"value\": \"%.1f\"}]}]}",\
                        ddev->name,ddev->data1_name,ndev->data1);
    }

    This.devs[n].onenetDesc.httpc_settings.post_body =  &(Postbuf);
    httpc_post_file_dns("api.heclouds.com", 80, "/devices/"APP_ONENET_DEVICE_ID"/datapoints",
                        &This.devs[n].onenetDesc.httpc_settings, NULL, &This.devs[n].onenetDesc, &phttpc_state);
}

void httpc_result_fn_impl_onenet(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err){
    Dev_onenet_desc* ptr = (Dev_onenet_desc*)arg;
    LWIP_UNUSED_ARG(httpc_result);
    LWIP_UNUSED_ARG(rx_content_len);
    LWIP_UNUSED_ARG(srv_res);
    LWIP_UNUSED_ARG(err);
    printf("httpc_result_fn_impl \n");
    ptr->sending = 0;
    //_TRAP;
}