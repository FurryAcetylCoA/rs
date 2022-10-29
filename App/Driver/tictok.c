//
// Created by 11096 on 2022/10/28.
//

#include "tictok.h"

static void      tick   (void);
static uint32_t  Add    (void(*)(void),uint32_t,_Bool);
static void      Remove (uint32_t);
static void Init(void);
#define Task_Empty   -2

typedef struct{

    uint32_t  ID;
    void      (*Payload)(void);
    uint32_t  Period;     //不变
    uint32_t  NextTime;   //每轮改变
    _Bool     OneShoot;

}Task;

TicTok tictok={
        .tick   =   tick,
        .Add    =   Add,
        .Remove =   Remove,
        .Init   =   Init
};

static Task task_list[10];

static void Init(void){

    Task *p;
    for(int i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        p->ID=Task_Empty;
    }

}
static void tick  (void){//遍历任务表
    Task *p;
    for(int i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID!=Task_Empty){
            if(++p->NextTime==p->Period){
                p->NextTime=0;
                p->Payload();
                if(p->OneShoot){
                    p->ID=Task_Empty;
                }
            }
        }
    }

}
uint32_t Add (void(*Payload)(void),uint32_t time,_Bool OneShoot){ //时间单位是10ms
    int i;
    Task *p;
    for(i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID==Task_Empty){
            p->ID=i;
            p->NextTime=0;
            p->Period=time;
            p->Payload=Payload;
            p->OneShoot=OneShoot;
            return i;
        }
    }
    return -1;
}

static void  Remove(uint32_t ID){
    int i;
    Task *p;
    for(i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID==ID){
            p->ID=Task_Empty;
            break;
        }
    }
}
