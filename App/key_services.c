//
// Created by 11096 on 2022/10/30.
//

#include "key_services.h"

void static key_services_golden_key();
void static key_services_silver_key();

void key_services(){
    switch (This.state) {
        case ST_Golden_Key:
            key_services_golden_key();
            break;
        case ST_Silver_Key:
            key_services_silver_key();
            break;
    }

}

void static key_services_golden_key(){
    if(This.keys.U != 0){ //»Œ“‚º¸
        This.state_go(ST_Empyrean);
    }
}
void static key_services_silver_key(){
    if(This.keys.U != 0){
        This.state_go(ST_Earth);
    }
}