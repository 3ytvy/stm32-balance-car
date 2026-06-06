#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"                  // Device header
#include "public.h"

void PID_LimitConfig(PID_TypeDef *PID,float Upper,float Lower);
void PID_Reset(PID_TypeDef *p);
void PID_Init(PID_TypeDef *PID);
void PID_ChangeTarget(PID_TypeDef *PID,float Target);
void PID_Update(PID_TypeDef *p);


#endif
