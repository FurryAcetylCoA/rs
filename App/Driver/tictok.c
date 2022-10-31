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
    uint32_t  Period;    //����
    uint32_t  Counter;   //ÿ�ָı�
    uint32_t  Tock;      //��־λ������Ϊ1ʱ������tock�����б�ִ��
    _Bool     OneShoot;  //ִֻ��һ��

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
//@note:����tick��tock����
//    ������ֻ��tick�����ģ���tick�����У����Counter����Period���ͻ�ִ�ж�Ӧ��PayLoad (��one_shot���)
//  �������и����⣬����tick����SysTick_Handler()���õġ�����tickִ��ʱ��handler mode��
//  ���š��ж�ʱ��Ҫִ��̫���߼�����˼��(������SysTick��1msһ�Σ����ο�������������)���ҽ���ĳ�ֻ��tick�����ñ�־λ��
//  Ȼ������ѭ����ͨ������tock������ִ��ʵ�ʵ�PayLoad

static void tick  (void){//���������
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

uint32_t Add (void(*Payload)(uint32_t),uint32_t time,_Bool OneShoot){ //ʱ�䵥λ��10ms
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
