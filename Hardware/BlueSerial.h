#ifndef __BLUESERIAL_H
#define __BLUESERIAL_H


#include "stdio.h"
#include "public.h"

extern uint8_t Serial_RxFlag;
extern char Serial_RxPacket[];
extern uint8_t Key1;
extern uint8_t pid_adjust_flag;//某个pid被修改了的标志位

extern uint8_t test_open_flag;

extern uint8_t turn_flag;

extern float omega_ref_test;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(char *String);
uint32_t Serial_Pow(uint32_t X,uint32_t Y);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_Printf(char *format,...);
uint8_t Serial_GetRxFlag(void);
void Serial_SendPacket(void);
void BlueSeiral_Proc(void);



#endif
