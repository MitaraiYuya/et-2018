/**
 ******************************************************************************
 ** ファイル名 : grayJudgment.c
 **
 ** 概要 : 灰色検知用プログラム
 **
 ** 注記 :　
 ******************************************************************************
 **/

#include "ev3api.h"
#include "grayJudgment.h"
#include "devices.h"
#include <stdlib.h>
#define TOPMAX 1000

int threshiold;
int gray_valu;
int turn_frequency;

int light_log[TOPMAX] = {};
int motor_ang_l_log[TOPMAX] = {};
int motor_ang_r_log[TOPMAX] = {};
int log_top = 0;
int motor_ang_l_old = 0;
int motor_ang_r_old = 0;

double motor_ang_average_l_log[TOPMAX] = {};
double motor_ang_average_r_log[TOPMAX] = {};

int start_count = 0, dabu = 0,kari_gray_judg=0;

bool_t gray_judg,bt_chec;

bool_t GrayJudgment(int light_valu, int motor_ang_l, int motor_ang_r) {
	bt_chec = true;
	gray_judg = false;

	light_log[log_top] = light_valu - threshiold;

	//logに前の回転数からどれだけ変化したか記録する
	motor_ang_l_log[log_top] = motor_ang_l - motor_ang_l_old;
	motor_ang_r_log[log_top] = motor_ang_r - motor_ang_r_old;
	//oldに今回の回転数を記録する
	motor_ang_l_old = motor_ang_l;
	motor_ang_r_old = motor_ang_r;

	double motor_ang_l_count = 0, motor_ang_r_count = 0;
	int l_renzoku_count = 0, light_count = 0, light_up_count = 0, light_doun_count = 0;
	double l_bic_valu = 0, r_bic_valu = 0;
	int l_bic_count = 0, r_bic_count = 0;
	for (int i = 0; i < 50; i++) {
		int number = (log_top - i) >= 0 ? (log_top - i) : (TOPMAX - i + log_top);
		motor_ang_l_count += motor_ang_l_log[number];
		motor_ang_r_count += motor_ang_r_log[number];

		if (motor_ang_average_r_log[number] > motor_ang_average_l_log[number]) {
			r_bic_valu += motor_ang_average_r_log[number] -
				motor_ang_average_l_log[number];
			r_bic_count++;
		}//右側の動きが大きかった時の回転すう、とその回数

		if (light_log[number] <= -4)
			light_doun_count++;
		number = (log_top - i - 40) >= 0 ? (log_top - i - 40) : (TOPMAX - i - 40 + log_top);
		if (light_log[number] >= 4)
			light_up_count++;

		number = (log_top - i - 20) >= 0 ? (log_top - i - 20) : (TOPMAX - i - 20 + log_top);
		if (motor_ang_average_l_log[number] > motor_ang_average_r_log[number] ) {
			l_bic_valu += motor_ang_average_l_log[number] -
							motor_ang_average_r_log[number];
			l_bic_count++;
		}//左側の動きが大きかった時の回転すう、とその回数

		int number2 = (log_top - 1) >= 0 ? (log_top - 1) : (TOPMAX - 1 + log_top);
		if (motor_ang_average_l_log[number] - motor_ang_average_r_log[number] <
			motor_ang_average_r_log[number2] - motor_ang_average_l_log[number2]) {
			l_renzoku_count++;
		}


	}
	motor_ang_l_count /= 50.0;//平均値の算出
	motor_ang_r_count /= 50.0;
	//light_count /= 50;

	motor_ang_average_l_log[log_top] = motor_ang_l_count;//ログの記録
	motor_ang_average_r_log[log_top] = motor_ang_r_count;
	/*
	int l_renzoku_count = 0;
	for (int i = 0; i < 200; i++) {
		int number = (log_top - i - 150) >= 0 ? (log_top - i - 150) : (TOPMAX - i - 150 + log_top);
		if (motor_ang_average_l_log[number] > motor_ang_average_r_log[number]) {
			l_renzoku_count++;
		}
		else {
			l_renzoku_count = 0;
		}
	}
	*/
	if (1.2 >= motor_ang_r_count - motor_ang_l_count  && motor_ang_r_count - motor_ang_l_count >= 0.35) {
		//右モーターが左モーターより回転したら
		if (light_count > 20 && dabu >= 350 && l_renzoku_count > 25) {
			//光が暗くなっている　＆　過去350以内に判定が無い　
			gray_judg = true;
			dabu = 0;
		}
	}

	if (light_doun_count > 10 && 
		light_up_count > 6 && 
		dabu >= 350)
		kari_gray_judg = 200;

	if (kari_gray_judg > 0 &&
		l_bic_count > 0 &&
		r_bic_count > 0 &&
		dabu >= 350) {
		l_bic_valu = l_bic_valu / (double)l_bic_count;
		r_bic_valu = r_bic_valu / (double)r_bic_count;
		if (l_bic_valu > r_bic_valu) {
			//光が暗くなっている　＆　過去350以内に判定が無い　
			gray_judg = true;
			dabu = 0;
		}
	}

	kari_gray_judg <= 0 ? kari_gray_judg = 0 : kari_gray_judg--;//仮判定からの回数

	dabu >= 350 ? dabu = 350 : dabu++;//前の判定からの回数
	start_count >= 200 ? start_count = 200 : start_count++;//スタートからの回数

	if (++log_top == TOPMAX)
		log_top = 0;
	if (gray_judg == true && start_count >= 200)
		return true;
	return false;
}

void GrayJudgment_makeThreshiold(int come_threshiold, int come_turn_frequency, int come_gray_valu) {
	threshiold = come_threshiold;
	gray_valu = come_gray_valu;
	turn_frequency = come_turn_frequency;
}
/*
bool_t GrayJudgment_log_send(int* light_value, int* motor_ang_l, int* motor_ang_r) {
	if (bt_chec == true) {
		bt_chec = false;
		*light_value = light_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		*motor_ang_l = motor_ang_l_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		*motor_ang_r = motor_ang_r_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		return true;
	}
	return false;
}*/
bool_t GrayJudgment_log_send_2(int* light_value, int* motor_ang_l, int* motor_ang_r,int* gray_area,int *gray_juge_result) {
	if (bt_chec == true) {
		bt_chec = false;
		*light_value = light_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		*motor_ang_l = motor_ang_l_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		*motor_ang_r = motor_ang_r_log[(log_top - 1) >= 0 ? (log_top - 1) : TOPMAX - 1];
		*gray_area = (Devices_getDeviceValue(SENSOR_SONER_) < 20) ? 1 : 0;
		*gray_juge_result = gray_judg == true ? 1 : 0;
		//*gray_juge_result = iranai_count;
		return true;
	}
	return false;
}