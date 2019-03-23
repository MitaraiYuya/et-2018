#include "devices.h"
/*
#include <string.h>
#include <stdlib.h>
*/

#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)


typedef struct {
	int white;
	int black;
	int gyro;
	int gray;
	int limboWhite;
	int limboBlack;
	int tailWhite;
	int tailBlack;
}Calibrate;

Calibrate calibrateValue;
int isCalibrate = 0;

/**
* センサー、モーターの接続を定義します
*/
static const sensor_port_t
touch_sensor = EV3_PORT_1,
sonar_sensor = EV3_PORT_2,
color_sensor = EV3_PORT_3,
gyro_sensor = EV3_PORT_4;

static const motor_port_t
left_motor = EV3_PORT_C,
right_motor = EV3_PORT_B,
tail_motor = EV3_PORT_A;




void Devices_calibrate() {
	int time;
	isCalibrate = 1;
	//char buf[256];

	/* センサー入力ポートの設定 */
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor); /* 反射率モード */
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	/* モーター出力ポートの設定 */
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	/* ジャイロセンサーリセット */
	ev3_gyro_sensor_reset(gyro_sensor);
	/* 倒立振子API初期化 */
	balance_init();

	do {
		ev3_motor_reset_counts(tail_motor);//尻尾の角度センサー値をリセットする
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_INIT);//尻尾を上にあげる
		tslp_tsk(50);
	} while (Devices_getDeviceValue(MOTOR_TAIL_) != 0);//体につく(変化がなくなる)まで
													   /* 尻尾センサリセット */
	ev3_motor_reset_counts(tail_motor);
	char str[50];
	sprintf(str, "EV3way-ET battery = %d", ev3_battery_voltage_mV());
	ev3_lcd_draw_string(str, 0, CALIB_FONT_HEIGHT * 1);
	ev3_lcd_draw_string("         GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	ev3_lcd_draw_string("         WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
	ev3_lcd_draw_string("         BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
	ev3_lcd_draw_string("         GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
	ev3_lcd_draw_string("         LimboWhite Calibrate", 0, CALIB_FONT_HEIGHT * 6);
	ev3_lcd_draw_string("         LimboBlack Calibrate", 0, CALIB_FONT_HEIGHT * 7);
	ev3_lcd_draw_string("         TailWhite Calibrate", 0, CALIB_FONT_HEIGHT * 8);
	ev3_lcd_draw_string("         TailBlack Calibrate", 0, CALIB_FONT_HEIGHT * 9);
	tslp_tsk(1000);
	

	/*
	char mozi[50] = {};
	int a;
	strcpy(mozi, "wwwww");
	while (1) {
		if (Devices_getDeviceValue(SENSOR_TOUCH_)==false) {
			ev3_lcd_draw_string("         SENSOR_TOUCH is TRUE", 0, CALIB_FONT_HEIGHT * 6);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH is FALSE", 0, CALIB_FONT_HEIGHT * 6);
		}

		if (ev3_ultrasonic_sensor_get_distance(sonar_sensor) < 15) {
			ev3_lcd_draw_string("         SONAR is TRUE", 0, CALIB_FONT_HEIGHT * 8);
		}
		else {
			ev3_lcd_draw_string("         SONAR is FALSE", 0, CALIB_FONT_HEIGHT * 8);
		}if (ev3_touch_sensor_is_pressed(touch_sensor) == true) {
			a = 1;
		}
		else a = 2;
		sprintf(mozi, "sensor_touch is %d", a );
		ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 10);
	}
	*/

	// ジャイロ
	while (1) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_START); // 完全停止用角度に制御
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
			Devices_playTone(800, 250);
			// ジャイロセンサ初期化
			// ev3_gyro_sensor_get_angle(gyro_sensor);
			break; // タッチセンサが押された
		}
	}
	calibrateValue.gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	ev3_lcd_draw_string("Complete GYRO Calibrate", 0, CALIB_FONT_HEIGHT * 2);
	//sprintf(buf, "gyro  = %03d", calibrateValue.gyro);

	//Limbo作業の時短の為に直接代入してます
	if (0) {
		calibrateValue.white = 35;
		calibrateValue.black = 2;
		calibrateValue.gray = 8;
		calibrateValue.limboWhite = 10;
		calibrateValue.limboBlack = 5;
	} else {
		// 白色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.white = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete WHITE Calibrate", 0, CALIB_FONT_HEIGHT * 3);
		//sprintf(buf, "white = %03d", calibrateValue.white);

		// 黒色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.black = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete BLACK Calibrate", 0, CALIB_FONT_HEIGHT * 4);
		//sprintf(buf, "black = %03d", calibrateValue.black);

		// 灰色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.gray = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete GRAY Calibrate", 0, CALIB_FONT_HEIGHT * 5);
		//sprintf(buf, "gray  = %03d", calibrateValue.gray);

		//Limbo白色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, 75);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.limboWhite = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete LimboWhite Calibrate", 0, CALIB_FONT_HEIGHT * 6);
		//sprintf(buf, "limboWhite = %03d", calibrateValue.limboWhite);

		//Limbo黒色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, 75);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.limboBlack = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete LimboBlack Calibrate", 0, CALIB_FONT_HEIGHT * 7);
		//sprintf(buf, "limboBlack = %03d", calibrateValue.limboBlack);
		
		//tail白色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, 90);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.tailWhite = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete TailWhite Calibrate", 0, CALIB_FONT_HEIGHT * 8);
		//sprintf(buf, "tailWhite = %03d", calibrateValue.tailWhite);

		//tail黒色
		while (1) {
			Devices_controlDevice(MOTOR_TAIL_, 90);
			if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
				Devices_playTone(800, 250);
				break;
			}
		}
		calibrateValue.tailBlack = Devices_getDeviceValue(SENSOR_COLOR_);
		ev3_lcd_draw_string("Complete TailBlack Calibrate", 0, CALIB_FONT_HEIGHT * 9);
		//sprintf(buf, "tailBlack = %03d", calibrateValue.tailBlack);

		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP);	//TAIL角度を発進用に元に戻す
	}



	char mozi[50] = {};
// 	sprintf(mozi, "          GYRO = %.4lf      ", Devices_getCalibrateValue(GYRO_CALIBRATE));
// 	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 1);
	
	sprintf(mozi, "         WHITE = %4d        ", Devices_getCalibrateValue(WHITE_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 2);

	sprintf(mozi, "         BLACK = %4d        ", Devices_getCalibrateValue(BLACK_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 3);

	sprintf(mozi, "    threshiold = %4d        ", (Devices_getCalibrateValue(BLACK_CALIBRATE)+
										Devices_getCalibrateValue(WHITE_CALIBRATE)) /2 );
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 4);

	sprintf(mozi, "          GRAY = %4d        ", Devices_getCalibrateValue(GRAY_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 5);

	sprintf(mozi, "    LimboWhite = %4d        ", Devices_getCalibrateValue(LIMBO_WHITE_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 6);

	sprintf(mozi, "    LimboBlack = %4d        ", Devices_getCalibrateValue(LIMBO_BLACK_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 7);

	sprintf(mozi, "    tailWhite = %4d        ", Devices_getCalibrateValue(TAIL_WHITE_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 8);

	sprintf(mozi, "    tailBlack = %4d        ", Devices_getCalibrateValue(TAIL_BLACK_CALIBRATE));
	ev3_lcd_draw_string(mozi, 0, CALIB_FONT_HEIGHT * 9);
}


void Devices_controlDevice( int deviceNo, int value) {
	float pwm;
	switch (deviceNo) {
		case MOTOR_RIGHT_:

			if (value == 0) {
				ev3_motor_stop(right_motor, true);
			}
			else{
				ev3_motor_set_power(right_motor, (int)value);
			}

			break;


		case MOTOR_LEFT_:

			if (value == 0)	{
				ev3_motor_stop(left_motor, true);
			}
			else{
				ev3_motor_set_power(left_motor, (int)value);
			}

			break;


		case MOTOR_TAIL_:

			pwm = (float)(value - Devices_getDeviceValue(MOTOR_TAIL_))*P_GAIN;
			if (pwm > PWM_ABS_MAX){
				pwm = PWM_ABS_MAX;
			}
			else if (pwm < -PWM_ABS_MAX){
				pwm = -PWM_ABS_MAX;
			}

			if (pwm == 0){
				ev3_motor_stop(tail_motor, true);
			}
			else{
				ev3_motor_set_power(tail_motor, (signed char)pwm);
			}

			break;

		case MONITOR_:
			switch (value){
				case -1:
					ev3_lcd_draw_string("Waiting Start AND press touch sensor", 0, CALIB_FONT_HEIGHT * 10);
					break;

				case 0:
					ev3_lcd_draw_string("Running STOP mode                   ", 0, CALIB_FONT_HEIGHT * 10);
					break;

				case 1:
					ev3_lcd_draw_string("Running BALANCE mode                ", 0, CALIB_FONT_HEIGHT * 10);
					break;

				case 2:
					ev3_lcd_draw_string("Running LINE_TRACE mode             ", 0, CALIB_FONT_HEIGHT * 10);
					break;
			}
			
	}
	return;
}

void Devices_setGyroDeviceValue() {
	calibrateValue.gyro = Devices_getDeviceValue(SENSOR_GYRO_);
}

int  Devices_getCalibrateValue(int deviceNo) {
	if (isCalibrate == 0)Devices_calibrate();
	
	switch (deviceNo) {
		case GYRO_SENSOR:
			return calibrateValue.gyro;

		case BLACK_CALIBRATE:
			return calibrateValue.black;

		case WHITE_CALIBRATE:
			return calibrateValue.white;
		
		case GRAY_CALIBRATE:
			return calibrateValue.gray;
		case LIMBO_WHITE_CALIBRATE:
			return calibrateValue.limboWhite;
		case LIMBO_BLACK_CALIBRATE:
			return calibrateValue.limboBlack;
		case TAIL_WHITE_CALIBRATE:
			return calibrateValue.tailWhite;
		case TAIL_BLACK_CALIBRATE:
			return calibrateValue.tailBlack;
	}

	return -1;
}


int  Devices_getDeviceValue(int deviceNo) {
	switch (deviceNo) {
		case SENSOR_COLOR_:
			return ev3_color_sensor_get_reflect(color_sensor);

		case SENSOR_SONER_:
			return ev3_ultrasonic_sensor_get_distance(sonar_sensor);

		case SENSOR_TOUCH_:
			if (ev3_touch_sensor_is_pressed(touch_sensor)) {
				while (ev3_touch_sensor_is_pressed(touch_sensor));
				return  1;
			}
			return 0;
			
		case SENSOR_GYRO_:
			return ev3_gyro_sensor_get_rate(gyro_sensor);

		case MOTOR_RIGHT_:
			return ev3_motor_get_counts(right_motor);

		case MOTOR_LEFT_:
			return ev3_motor_get_counts(left_motor);

		case MOTOR_TAIL_:
			return ev3_motor_get_counts(tail_motor);

		case BATTERY_:
			return ev3_battery_voltage_mV();
		
		case MOTOR_POWER_RIGHT_:
			return ev3_motor_get_power(right_motor);
		
		case MOTOR_POWER_LEFT_:
			return ev3_motor_get_power(left_motor);
	}

	return -1;
}

void Devices_playBeep() {
	ev3_speaker_set_volume(20);
	ev3_speaker_play_tone(800, 250);
}

void Devices_playTone(int pitch, int duration){
	ev3_speaker_set_volume(1);
	ev3_speaker_play_tone(pitch, duration);
}
