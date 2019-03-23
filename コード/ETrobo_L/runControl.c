#include "runControl.h"


#include "stop.h"
#include "balanceRun.h"
#include "lineTraceRun.h"
#include "limbo.h"

#include "stack.h"

#define MAIN_STACK_SIZE 32

Stack mainStack;

void RunControl_run() {
	int runMode;

	StackPop(&mainStack, &runMode);

	switch (runMode) {
		case STOP:
			StackPush(&mainStack, Stop_run());
			Devices_controlDevice(MONITOR_, (int)STOP);
			ev3_led_set_color(LED_GREEN);
			break;

		case BALANCE_RUN:
			StackPush(&mainStack, BalanceRun_run());
			Devices_controlDevice(MONITOR_, (int)BALANCE_RUN);
			ev3_led_set_color(LED_ORANGE);
			break;

		case LINE_TRACE_RUN:
			StackPush(&mainStack, LineTraceRun_run());
			Devices_controlDevice(MONITOR_, (int)LINE_TRACE_RUN);
			ev3_led_set_color(LED_RED);
			break;

		case LIMBO_RUN:
			StackPush(&mainStack, Limbo_run());
			Devices_controlDevice(MONITOR_, (int)LIMBO_RUN);
			ev3_led_set_color(LED_GREEN);
			break;
	}
}


void RunControl_makeThreshiold() {
	StackAlloc(&mainStack, MAIN_STACK_SIZE);
	StackPush(&mainStack, (int)BALANCE_RUN);
	BalanceRun_makeThreshiold();
	LineTraceRun_makeThreshiold();
	Limbo_makeThreshiold();
}
