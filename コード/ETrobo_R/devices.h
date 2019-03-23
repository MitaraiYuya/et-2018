#ifndef _devices
#define _devices


#include "ev3api.h"
#include "app.h"
#include "balancer.h"


#define TAIL_ANGLE_MINIMUM  360  /*尻尾を一番上にする特殊数値*/
#define TAIL_ANGLE_STAND_UP  95 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_START     107 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE      3  /* バランス走行時の角度[度] */
#define TAIL_ANGLE_INIT    -180  /*	初期化時の角度[度] */
#define P_GAIN             2.5F  /* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX          100  /* 完全停止用モータ制御PWM絶対最大値 */

/* 各オブジェクトの通し番号 */

#define SENSOR_COLOR_ 0
#define SENSOR_SONER_ 1
#define SENSOR_TOUCH_ 2
#define SENSOR_GYRO_ 3
#define MOTOR_RIGHT_ 4
#define MOTOR_LEFT_ 5
#define MOTOR_TAIL_ 6
#define BATTERY_ 7
#define MONITOR_ 8
#define MOTOR_POWER_RIGHT_ 99
#define MOTOR_POWER_LEFT_ 98

#define WHITE_CALIBRATE 10
#define BLACK_CALIBRATE 11
#define GYRO_CALIBRATE 12
#define GRAY_CALIBRATE 13
#define LIMBO_WHITE_CALIBRATE 14
#define LIMBO_BLACK_CALIBRATE 15
#define TAIL_WHITE_CALIBRATE 16
#define TAIL_BLACK_CALIBRATE 17

#define SONER_AREAT_DISTANCE_ 1
#define SONER_GIRIGIRI_DISTANCE_ 1

void Devices_calibrate();
void Devices_controlDevice(int deviceNo, int value);
void Devices_setGyroDeviceValue();
int  Devices_getCalibrateValue(int deviceNo);
int  Devices_getDeviceValue(int deviceNo);
void Devices_playBeep();                            //ビープ音がなります
void Devices_playTone(int pitch, int duration);     //pitchで指定した周波数の音が、durationで指定した時間なります


#endif 