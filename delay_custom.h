#include <stdio.h>
#define oneMs 128


#ifndef delay
#define delay
void delay_ms(unsigned int x);
void delay_seconds(unsigned int x);
void delay_us(void);
void delay_onems();
void delay_oneus();
#endif // delay
