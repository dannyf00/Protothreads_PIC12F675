#include "config.h"						//configuration words
#include "gpio.h"                           //we use gpio functions
#include "delay.h"                          //we use software delays
#include "tmr0.h"						//use tmr0 as systick
#include "pt.h"								//we use protothreads

//hardware configuration
#define LED_PORT			GPIO
#define LED_DDR				TRISIO
#define LED1				(1<<0)
#define LED1_DLY			(100ul)
#define LED2				(1<<1)
#define LED2_DLY			(200ul)
#define LED3				(1<<2)
#define LED3_DLY			(100ul)
#define LED4				(1<<4)
//end hardware configuration

//global defines
#define ticks()				tick		//use look counter
#define systicks()			TMR0		//use tmr0 / systick

//global variables
struct pt pt1, pt2, pt3;
uint32_t tick=0;


//task1
static char task1(struct pt *pt) {
	static uint32_t cnt=0;
	
	PT_BEGIN(pt);						//start the protothread
	while (1) {
		PT_WAIT_UNTIL(pt, ticks() - cnt > LED1_DLY);	//wait for sufficient time passage
		cnt += LED1_DLY;				//update counter
		IO_FLP(LED_PORT, LED1);			//task1 - flip led1
	}
	PT_END(pt);							//end protothread
}

//task2
//option 1: use look counter
//option 2: use tmr0. will reset flag
static char task2(struct pt *pt) {
	static uint32_t cnt=0;
	
	PT_BEGIN(pt);						//start the protothread
	while (1) {
		//option 1
		//PT_WAIT_UNTIL(pt, ticks() - cnt > LED2_DLY);		//wait for sufficient time passage
		//cnt += LED2_DLY;				//update counter
		//option 2: hardware based timing
		PT_WAIT_UNTIL(pt, T0IF);		//wait for sufficient time passage = tmr0 overflows
		T0IF = 0;						//reset timer0 flag
		IO_FLP(LED_PORT, LED2);			//task2 - flip led2
	}
	PT_END(pt);							//end protoghread
}

//task3
static char task3(struct pt *pt) {
	static uint8_t cnt=0;
	
	PT_BEGIN(pt);						//start the protothread
	while (1) {
		PT_WAIT_UNTIL(pt, (uint8_t) (systicks() - cnt) > LED3_DLY);	//wait for sufficient time passage
		cnt += LED3_DLY;				//update counter
		IO_FLP(LED_PORT, LED3);			//task2 - flip led2
	}
	PT_END(pt);							//end protoghread
}

int main(void) {
	
	mcu_init();							//initialize the mcu
	tmr0_init(TMR0_PS_256x);				//set up timer
	IO_OUT(LED_DDR, LED1 | LED2 | LED3 | LED4);		//led 1+2+3 as output
	PT_INIT(&pt1);						//initialize the protothreads
	PT_INIT(&pt2);
	while (1) {
		tick+=1;						//update the loop counter
		//always run tasks
		IO_FLP(LED_PORT, LED4);			//flip led4
		
		//ocassionally run tasks
		task1(&pt1);					//task 1
		task2(&pt2);					//task 2
		task3(&pt3);					//task 3
	}
}

