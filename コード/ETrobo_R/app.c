/**
 ******************************************************************************
 ** ファイル名 : app.c
 **
 ** 概要 : 2輪倒立振子ライントレースロボットのTOPPERS/HRP2用Cサンプルプログラム
 **
 ** 注記 : sample_c4 (sample_c3にBluetooth通信リモートスタート機能を追加)
 ******************************************************************************
 **/

#include "ev3api.h"
#include "app.h"
#include "balancer.h"

#include "devices.h"

#include "lineTraceRun.h"
#include "runControl.h"

//#include "grayJudgment.h"//PCへlogを送信するため
//#include "runControl.h"	//PCへlogを送信するため

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif



static int      bt_cmd = 0;     /* Bluetoothコマンド 1:リモートスタート */
static FILE     *bt = NULL;     /* Bluetoothファイルハンドル */
//static int bt_log_flag = 1;		//ログを送信する時のフラグ

static memfile_t *dededon = NULL;

/* 下記のマクロは個体/環境に合わせて変更する必要があります */

/* sample_c1マクロ */
//#define GYRO_OFFSET  0          /* ジャイロセンサオフセット値(角速度0[deg/sec]時) */
//#define LIGHT_WHITE  40         /* 白色の光センサ値 */
//#define LIGHT_BLACK  0          /* 黒色の光センサ値 */


/* sample_c2マクロ */
// #define SONAR_ALERT_DISTANCE 30 /* 超音波センサによる障害物検知距離[cm] */


/* sample_c3マクロ */
//#define TAIL_ANGLE_STAND_UP  93 /* 完全停止時の角度[度] */
//#define TAIL_ANGLE_DRIVE      3 /* バランス走行時の角度[度] */
//#define P_GAIN             2.5F /* 完全停止用モータ制御比例係数 */
//#define PWM_ABS_MAX          60 /* 完全停止用モータ制御PWM絶対最大値 */


/* sample_c4マクロ */
//#define DEVICE_NAME     "ET0"  /* Bluetooth名 hrp2/target/ev3.h BLUETOOTH_LOCAL_NAMEで設定 */
//#define PASS_KEY        "1234" /* パスキー    hrp2/target/ev3.h BLUETOOTH_PIN_CODEで設定 */
#define CMD_START         '1'    /* リモートスタートコマンド */

/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

/* 関数プロトタイプ宣言 */
static int sonar_alert(void);


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




void init() {
	/* LCD画面表示 */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);



	
	/* 
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor);
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	ev3_motor_reset_counts(tail_motor);
	*/
	/*
	ev3_sensor_config(EV3_PORT_1, TOUCH_SENSOR);

	while (1) {
		if (Devices_getDeviceValue(SENSOR_TOUCH_) == 1) {
			ev3_lcd_draw_string("         SENSOR_TOUCH is TRUE", 0, CALIB_FONT_HEIGHT * 6);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH is FALSE", 0, CALIB_FONT_HEIGHT * 6);
		}

		if (ev3_touch_sensor_is_pressed(EV3_PORT_1)) {
			ev3_lcd_draw_string("         SENSOR_TOUCH2 is TRUE", 0, CALIB_FONT_HEIGHT * 8);
		}
		else {
			ev3_lcd_draw_string("         SENSOR_TOUCH2 is FALSE", 0, CALIB_FONT_HEIGHT * 8);
		}
	}*/

	Devices_calibrate();
	Devices_controlDevice(MONITOR_, -1);
	RunControl_makeThreshiold();
}


/* メインタスク */
void main_task(intptr_t unused)
{
    
	/* 初期化 */
	init();

    /* Open Bluetooth file */
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    //assert(bt != NULL);

    /* Bluetooth通信タスクの起動 */
    act_tsk(BT_TASK);

	ev3_speaker_set_volume(50);
    ev3_led_set_color(LED_ORANGE); /* 初期化完了通知 */



    /* スタート待機 */
    while(1)
    {
		
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */

        if (bt_cmd == 1){
            break; /* リモートスタート */
        }

		if(Devices_getDeviceValue(SENSOR_TOUCH_)==1){
            break; /* タッチセンサが押された */
        }

        tslp_tsk(1); /* 1msecウェイト */
    }


	/* 走行モーターエンコーダーリセット */
	ev3_motor_reset_counts(left_motor);
	ev3_motor_reset_counts(right_motor);

    ev3_led_set_color(LED_GREEN); /* スタート通知 */

	//Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_STAND_UP); /* バランス走行用角度に制御 */
	while (Devices_getDeviceValue(MOTOR_TAIL_) < TAIL_ANGLE_START) {
		Devices_controlDevice(MOTOR_TAIL_, TAIL_ANGLE_START); /* バランス走行用角度に制御 */
	}

    /**
    * Main loop for the self-balance control algorithm
    */
    while(1){
//     	bt_log_flag = 1;
		RunControl_run();
        tslp_tsk(4); /* 4msec周期起動 */
    }
    ev3_motor_stop(left_motor, false);
    ev3_motor_stop(right_motor, false);

    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// 関数名 : sonar_alert
// 引数 : 無し
// 返り値 : 1(障害物あり)/0(障害物無し)
// 概要 : 超音波センサによる障害物検知
//*****************************************************************************
// static int sonar_alert(void)
// {
//     static unsigned int counter = 0;
//     static int alert = 0;
// 
//     signed int distance;
// 
//     if (++counter == 40/4) /* 約40msec周期毎に障害物検知  */
//     {
//         /*
//          * 超音波センサによる距離測定周期は、超音波の減衰特性に依存します。
//          * NXTの場合は、40msec周期程度が経験上の最短測定周期です。
//          * EV3の場合は、要確認
//          */
//         distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
//         if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
//         {
//             alert = 1; /* 障害物を検知 */
//         }
//         else
//         {
//             alert = 0; /* 障害物無し */
//         }
//         counter = 0;
//     }
// 
//     return alert;
// }



//*****************************************************************************
// 関数名 : bt_task
// 引数 : unused
// 返り値 : なし
// 概要 : Bluetooth通信によるリモートスタート。 Tera Termなどのターミナルソフトから、
//       ASCIIコードで1を送信すると、リモートスタートする。
//*****************************************************************************
void bt_task(intptr_t unused)
{
    while(1)
    {
		if (bt_cmd != 1) {
			uint8_t c = fgetc(bt); /* 受信 */
			switch (c)
			{
			case '1':
				bt_cmd = 1;
				break;
			default:
				break;
			}
			fputc(c, bt); /* エコーバック */
// 			fprintf(bt, "\r\n\r\nLight,\r\n");
		}
		
		
		//結果をPCに送信
		// if(bt_log_flag == 1){
		// 	// fprintfの第三引数以降に送信したいデータを記入
		// 	// メインタスクの無限ループ内のbt_log_flag = 1;をコメントインすること
		// 	// ログ送信によって動作がおかしくなる場所もあります（原因、再現性は不明）
		// 	// 必要最低限の送信に使ってください
		// 	fprintf(bt, "%d,\r\n", Devices_getDeviceValue(SENSOR_COLOR_));
		// 	bt_log_flag = 0;
		// }
		
    }
}
