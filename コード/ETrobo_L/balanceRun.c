#include "lineTraceRun.h"

int threshiold;
int gyroOffset;
int flag = 0;

int BalanceRun_run() {
	signed char forward=0;      /* 前後進命令 */
	signed char turn = -1;     /* 旋回命令 */

	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	//if (ev3_button_is_pressed(BACK_BUTTON)) break;

	Devices_controlDevice(MOTOR_TAIL_,
		Devices_getDeviceValue(MOTOR_TAIL_) < TAIL_ANGLE_DRIVE ? TAIL_ANGLE_DRIVE : Devices_getDeviceValue(MOTOR_TAIL_) - 5);
	/* バランス走行用角度に徐々にしていく制御 */

	if (forward < 10) forward++;

	/* 倒立振子制御API に渡すパラメータを取得する */
	motor_ang_l = Devices_getDeviceValue(MOTOR_LEFT_);
	motor_ang_r = Devices_getDeviceValue(MOTOR_RIGHT_);
	gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	volt = Devices_getDeviceValue(BATTERY_);

	/* 倒立振子制御APIを呼び出し、倒立走行するための */
	/* 左右モータ出力値を得る */
	balance_control(
		(float)forward,
		(float)turn,
		(float)gyro,
		(float)gyroOffset,
		(float)motor_ang_l,
		(float)motor_ang_r,
		(float)volt,
		(signed char*)&pwm_L,
		(signed char*)&pwm_R);

	/* EV3ではモーター停止時のブレーキ設定が事前にできないため */
	/* 出力0時に、その都度設定する */
	Devices_controlDevice(MOTOR_LEFT_, pwm_L);
	Devices_controlDevice(MOTOR_RIGHT_, pwm_R);

	if (Devices_getDeviceValue(SENSOR_COLOR_) < threshiold ) {
		return (int)LINE_TRACE_RUN;
		//return (int)LIMBO_RUN;

	}
	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
		return (int)STOP;
	}

	return (int)BALANCE_RUN;

}


void BalanceRun_makeThreshiold() {
	threshiold = (Devices_getCalibrateValue(BLACK_CALIBRATE) + Devices_getCalibrateValue(WHITE_CALIBRATE)) / 2;
	gyroOffset = Devices_getCalibrateValue(SENSOR_GYRO_);
}