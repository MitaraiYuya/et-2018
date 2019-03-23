#include "limbo.h"

#define TURN_TIME_LIMIT	500	/* 2sec */
#define TURN_TIME_SEARCH	300 	//尻尾設置後ライン復帰用の回転時間
#define LIMBO_SPEED 50
#define HOW_LONG 2300		//この数値を変えることによってゲート後に移動する量が変化(左右の回転角の和)
// 大体1500で53cm（2017ver）
#define RUN_LONG_LIMIT_FIRST 750	//１回目リンボーをくぐる距離
#define RUN_LONG_LIMIT 1500			//２回目以降リンボーをくぐる距離


int limboThresiold;
int nomalThresiold;
int tailThresiold;
int time = 0;
int state = 0;
int tailAngle;
int speed = 50;
int runMode;
int gyroOffset;
int motorAngle;
int runLong = RUN_LONG_LIMIT_FIRST;

typedef enum LD_State{
	TAIL_INIT,		//尻尾初期化=尻尾の角度を尻尾倒立用に設定
	SSTOP,			//小停止
	TAIL_GND,		//尻尾接地
	TAIL_LINE_SEARCH,	//尻尾設置をした後にラインに復帰する
	TAIL_BAR_SEARCH,	//尻尾が設置した状態でゲートギリギリまで近づく
	TAIL_TILT,		//尻尾を傾ける
	TAIL_RUN,		//尻尾走行=ゲート中
	TAIL_BACK,		//バック走行
	TAIL_TURN,		//超信地旋回
	TAIL_UP,		//起き上がり前進
	TAIL_UP_BACK,	//起き上がり逆走
	TAIL_END,		//車体を車庫まで運ぶ
	PSTOP,			//終了
} LD_STATE;

LD_STATE ld_state = TAIL_INIT;


typedef enum COLOR {
	BLACK,
	WHITE,
}COLOR;

const int stateNum = 26;
const int stateArray[] = {
	TAIL_INIT,
	SSTOP,
	TAIL_BACK,
	SSTOP,
	TAIL_GND,
	SSTOP,
	TAIL_LINE_SEARCH,
	SSTOP,
	TAIL_BAR_SEARCH,
	SSTOP,
	TAIL_TILT,
	SSTOP,
	TAIL_RUN,
	SSTOP,
	TAIL_TURN,
	SSTOP,
	TAIL_RUN,
	SSTOP,
	TAIL_TURN,
	SSTOP,
	TAIL_RUN,
	SSTOP,
	TAIL_UP,
	SSTOP,
	TAIL_UP_BACK,
	SSTOP,
	TAIL_END,
	PSTOP
};

void BalanceRun(int speed,int turn);
void Turn(int speed);
void nonBalanceRun(int speed,int turn);
void WakeUp(int speed);

int Limbo_run() {
	int thresiold;
	int turn;
	COLOR color;


	ld_state = stateArray[state];


	switch (ld_state){
		case TAIL_INIT:
			tailAngle = 90;
			speed = 0;
			runMode = 0;
			if (time >= 250 && speed == 0){
				//Devices_playTone(523, 250);
				state++;
				time = 0;
			}
			break;

		case SSTOP:
			if (speed > 0)speed = speed - 5;
			else if (speed < -5) speed = speed +5;
			else speed = 0;
			// runMode あえて弄らない
			if (time >= 375){	//1.5sec
				//Devices_playTone(262, 50);
				//Devices_playTone(294, 50);
				//Devices_playTone(330, 50);
				state++;
				time = 0;
				motorAngle = Devices_getDeviceValue(MOTOR_LEFT_) + Devices_getDeviceValue(MOTOR_RIGHT_);

			}
			break;

		case TAIL_BACK:
			speed = -45;
			runMode = 0;
			if (time > 300) {
				//Devices_playTone(554, 250);
				state++;
				time = 0;
			}
			break;

		case TAIL_GND:
			speed = 40;
			runMode = 1;
			if (time >= 50){
				//Devices_playTone(587, 250);
				state++;
				time = 0;
			}
			break;
			
		case TAIL_LINE_SEARCH:
			if(time < TURN_TIME_SEARCH){
				//Devices_playTone(200, 25);
				speed = -30;
				runMode = 2;
				time++;
			}
			else{
				//Devices_playTone(2000, 25);
				speed = 30;
				runMode = 2;
				if(Devices_getDeviceValue(SENSOR_COLOR_) < tailThresiold){
					//Devices_playTone(8000, 250);
					state++;
					time = 0;
				}
			}
			break;
		
		case TAIL_BAR_SEARCH:
			speed = 45;
			runMode = 5;
			//検知距離
			if(Devices_getDeviceValue(SENSOR_SONER_) <= SONER_GIRIGIRI_DISTANCE_){
				//Devices_playTone(262, 250);
				state++;
				time = 0;
			}
			break;

		case TAIL_TILT:
			speed = 0;
			runMode = 1;
			if (time > 150){
				if (tailAngle <= 75){
					//Devices_playTone(659, 250);
					time = 0;
					state++;
				}else{
					tailAngle -= 1;
					time = 0;
				}
			}
			break;

		case TAIL_RUN:
			speed = 45;
			runMode = 1;
			if (Devices_getDeviceValue(MOTOR_LEFT_) + Devices_getDeviceValue(MOTOR_RIGHT_) >= motorAngle + runLong){
				//Devices_playTone(698, 250);
				state++;
				time = 0;
			}
			break;

		case TAIL_TURN:
			speed = -30;
			runMode = 2;
			if (time >= TURN_TIME_LIMIT){
				runLong = RUN_LONG_LIMIT;
				//Devices_playTone(784, 250);
				state++;
				time = 0;
			}
			break;

		case TAIL_UP:
			runMode = 3;
			
			if (Devices_getDeviceValue(MOTOR_LEFT_) + Devices_getDeviceValue(MOTOR_RIGHT_) < motorAngle + 600){
				speed = 10;
			} else {
				//Devices_playTone(880, 250);
				time = 0;
				state++;
			}
			break;

		case TAIL_UP_BACK:
			runMode = 3;
			
			if (Devices_getDeviceValue(MOTOR_LEFT_) + Devices_getDeviceValue(MOTOR_RIGHT_) > motorAngle - 700){
				speed = -10;
				if (time % 80 == 0 && tailAngle < 90) tailAngle += 2;
			} else {
				//Devices_playTone(988, 250);
				tailAngle = 90;
				time = 0;
				state++;
				runMode = 4;
			}
			break;

		case TAIL_END:
			runMode = 5;
			tailAngle = 90;
			speed = 10;
			if (Devices_getDeviceValue(MOTOR_LEFT_) + Devices_getDeviceValue(MOTOR_RIGHT_) > motorAngle + HOW_LONG) {
				//Devices_playTone(1047, 250);
				time = 0;
				state++;
			}
			break;

		case PSTOP:
			return STOP;
	}
	time++;

	Devices_controlDevice(MOTOR_TAIL_, tailAngle);

	if (runMode == 0 || runMode == 4){
		thresiold = nomalThresiold;
	}
	else if(runMode == 5){
		thresiold = tailThresiold;
	}
	else {
		thresiold = limboThresiold;
	}

	
	if (Devices_getDeviceValue(SENSOR_COLOR_) > thresiold ){
		color = WHITE;
	}else {
		color = BLACK;
	}


	if (color == WHITE)turn = -1;
	else if (color == BLACK) turn = 1;
	else turn = 0;


	if (runMode == 0){
		BalanceRun(speed, turn);
	} else if (runMode == 1 || runMode == 4 || runMode == 5){
		nonBalanceRun(speed, turn);
	} else if (runMode == 2){
		Turn(speed);
	} else if (runMode == 3){
		WakeUp(speed);
	}

	if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1){
		return STOP;
	}

	return LIMBO_RUN;
}


void Limbo_makeThreshiold() {
	limboThresiold = (Devices_getCalibrateValue(LIMBO_BLACK_CALIBRATE) + Devices_getCalibrateValue(LIMBO_WHITE_CALIBRATE)) / 2;
	nomalThresiold = (Devices_getCalibrateValue(BLACK_CALIBRATE) + Devices_getCalibrateValue(WHITE_CALIBRATE)) / 2;
	gyroOffset = Devices_getCalibrateValue(GYRO_CALIBRATE);
	tailThresiold = (Devices_getCalibrateValue(TAIL_BLACK_CALIBRATE) + Devices_getCalibrateValue(TAIL_WHITE_CALIBRATE)) / 2;
}

void BalanceRun(int speed,int turn) {
	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */

	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;


	/* 倒立振子制御API に渡すパラメータを取得する */
	motor_ang_l = Devices_getDeviceValue(MOTOR_LEFT_);
	motor_ang_r = Devices_getDeviceValue(MOTOR_RIGHT_);
	gyro = Devices_getDeviceValue(SENSOR_GYRO_);
	volt = Devices_getDeviceValue(BATTERY_);

	/* 倒立振子制御APIを呼び出し、倒立走行するための */
	/* 左右モータ出力値を得る */
	balance_control(
		(float)speed,
		(float)turn,
		(float)gyro,
		(float)gyroOffset,
		(float)motor_ang_l,
		(float)motor_ang_r,
		(float)volt,
		(signed char*)&pwm_L,
		(signed char*)&pwm_R);

	Devices_controlDevice(MOTOR_LEFT_, pwm_L);
	Devices_controlDevice(MOTOR_RIGHT_, pwm_R);
}

void Turn(int speed) {
	Devices_controlDevice(MOTOR_LEFT_, -speed/2);
	Devices_controlDevice(MOTOR_RIGHT_, speed/2);
}

void nonBalanceRun(int speed, int turn) {
	if (turn < 0) {
		Devices_controlDevice(MOTOR_LEFT_, speed/8);
		Devices_controlDevice(MOTOR_RIGHT_, speed/2);
	} else if (turn > 0) {
		Devices_controlDevice(MOTOR_LEFT_, speed/2);
		Devices_controlDevice(MOTOR_RIGHT_, speed/8);
	} else {
		Devices_controlDevice(MOTOR_LEFT_, speed);
		Devices_controlDevice(MOTOR_RIGHT_, speed);
	}
}

void WakeUp(int speed) {
	Devices_controlDevice(MOTOR_LEFT_, speed);
	Devices_controlDevice(MOTOR_RIGHT_, speed);
}
