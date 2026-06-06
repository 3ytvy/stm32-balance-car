#include "Delay.h"

#define PERIODIC(T) \
static unit32_t nxt=0; \
if(GetTick_ms()<nxt) return; \
nxt += (T);

#define PERIODIC_START(NAME,T) \
static uint32_t NAME##_nxt=0; \
if(GetTick_ms() >= NAME##_nxt) {\
NAME##_nxt += (T);
	
#define PERIODIC_END}
