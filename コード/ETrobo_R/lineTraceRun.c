#include "lineTraceRun.h"
#include "grayJudgment.h"
#include "PID.h"

PID pid;

const int MAXSPEED = 100;
const int STARTCOLOR = 10;	//どこまで直進するか

int threshiold;
int gyroOffset;

int LineTraceRun_run() {
	static signed char forward = 40;      /* 前後進命令 */
	signed int turn;         /* 旋回命令 */
	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;
	static int flag = 0;	//flag:スタートを直進する用

	//if (ev3_button_is_pressed(BACK_BUTTON)) break;
	Devices_controlDevice(MOTOR_TAIL_, 
		Devices_getDeviceValue(MOTOR_TAIL_) < TAIL_ANGLE_DRIVE ? TAIL_ANGLE_DRIVE : Devices_getDeviceValue(MOTOR_TAIL_)-5);
	/* バランス走行用角度に徐々にしていく制御 */
	
	int color = Devices_getDeviceValue(SENSOR_COLOR_);
	// デバック用に光センサの値に応じて音を鳴らす
	// Devices_playTone(color * 60, 2);

	//どこまで直進するか
	if (color < STARTCOLOR) flag = 1;
	//最初は直進
	if (flag == 0) {
		turn = 0;
		//Devices_playBeep();
	}
	else turn = PID_getTurn(&pid, color - threshiold, 1);
	
	// turn = PID_getTurn(&pid, Devices_getDeviceValue(SENSOR_COLOR_) - threshiold, -1);//最後の引数はエッジ、右：-1、左：1

	/* 前進命令 */
	//forward = 100 - (turn >= 0 ? turn : -turn);
	if (forward < MAXSPEED)
		forward++;
	
	//if (Devices_getDeviceValue(SENSOR_COLOR_) >= threshiold) {
		//turn = -15; /* 左旋回命令 */
	//}else {
		//turn = 15; /* 右旋回命令 */
	//}

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

	char mozi[50] = {};//左右の回転数表示
	sprintf(mozi, "motor_L = %4d motor_R = %4d",motor_ang_l, motor_ang_r);
	ev3_lcd_draw_string(mozi, 0, 8 * 13);
	
	//if (GrayJudgment(Devices_getDeviceValue(SENSOR_COLOR_), motor_ang_l, motor_ang_r) == true)
	//	ev3_speaker_play_tone(NOTE_FS5, 10);

	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
		return STOP;
	}
	//return STOP;

	if (Devices_getDeviceValue(SENSOR_SONER_) < SONER_AREAT_DISTANCE_){
		return LIMBO_RUN;
	}


	return LINE_TRACE_RUN;

}


void LineTraceRun_makeThreshiold() {
	threshiold = (Devices_getCalibrateValue(BLACK_CALIBRATE) + Devices_getCalibrateValue(WHITE_CALIBRATE))/2;
	gyroOffset = Devices_getCalibrateValue(SENSOR_GYRO_);
	GrayJudgment_makeThreshiold(threshiold,0, Devices_getCalibrateValue(GRAY_CALIBRATE));
	PID_initialize(&pid, 0);
}