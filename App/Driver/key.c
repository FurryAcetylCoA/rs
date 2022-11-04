//
// Created by 11096 on 2022/10/30.
//

#include "key.h"
#include "main.h"


/* 按键排列
 * ┌────┐
 * │RST │
 * └────┘
 * ┌────┐
 * │KEY2│
 * ├────┼────┐
 * │KEY1│KEY3│
 * ├────┼────┘
 * │KEY0│
 * └────┘
 */

Key_data keys_old;
static Key_data read_key();

static Key_data read_key(){
    Key_data keys;
    keys.key0 = !HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin);
    keys.key1 = !HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin);
    keys.key2 = !HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin);
    keys.key3 = HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin);
    return  keys;
}


Key_data key_reader(){
    Key_data keys_accepted,keys_pressed;
    keys_pressed = read_key();
    keys_accepted.U = keys_pressed.U & (keys_old.U ^ keys_pressed.U);//按键必须是：1)现在按下的 & 2)上轮没按下的
    keys_old = keys_accepted;//change to pressed to avoid combo

    return keys_accepted;
}