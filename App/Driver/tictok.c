//
// Created by 11096 on 2022/10/28.
//

#include "tictok.h"

static void      tick   (void);
static void      tock   (void);
static uint32_t  Add    (void(*)(uint32_t),uint32_t,_Bool);
static void      Remove (uint32_t);
static void Init(void);
#define Task_Empty   (-2)

typedef struct{

    uint32_t  ID;
    void      (*Payload)(uint32_t ID);
    uint32_t  Period;    //不变
    uint32_t  Counter;   //每轮改变
    uint32_t  Tock;      //标志位，当其为1时，会在tock周期中被执行
    _Bool     OneShoot;  //只执行一次

}Task;

TicTok tictok={
        .tick   =   tick,
        .tock   =   tock,
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
//@note:关于tick和tock函数
//    本来是只有tick函数的，在tick函数中，如果Counter到达Period，就会执行对应的PayLoad (及one_shot检查)
//  但这样有个问题，就是tick是由SysTick_Handler()调用的。即，tick执行时是handler mode，
//  本着“中断时不要执行太多逻辑”的思想(尤其是SysTick是1ms一次，更何况还有重入问题)。我将其改成只在tick中设置标志位。
//  然后在主循环内通过调用tock函数来执行实际的PayLoad

static void tick  (void){//遍历任务表
    Task *p;
    for(int i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID!=Task_Empty){
            if(++p->Counter == p->Period){
                p->Counter = 0;
                p->Tock    = 1;
            }
        }
    }
}

static void tock  (void){
    Task *p;
    for(int i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID != Task_Empty){
            if(p->Tock == 1){
                p->Payload(p->ID);
                if(p->OneShoot){
                    p->ID=Task_Empty;
                }
            }
        }
    }
}

uint32_t Add (void(*Payload)(uint32_t),uint32_t time,_Bool OneShoot){ //时间单位是10ms
    int i;
    Task *p;
    for(i=0;i<sizeof(task_list)/sizeof(task_list[0]);i++){
        p=&task_list[i];
        if(p->ID==Task_Empty){
            p->ID=i;
            p->Counter=0;
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
