/***************************************************************************//**
	@file		PID.c
	@brief		PID 制御モジュール
	@author		$Author: toyo_chun $
	@version	$Revision: 386 $
	@date		$Date:: 2012-02-19 01:17:54 +0900 #$
*//****************************************************************************/

/*******************************************************************************
	include
*******************************************************************************/
#include "PID.h"



/*******************************************************************************
	macro
*******************************************************************************/
#define PID_TURN_LIMIT		100

/* PID 係数 */
// P = 比例
// I = 積分
// D = 微分

// forward = 80で安定走行
// #define PID_TURN_P_FACTOR	( 1.14514)
// #define PID_TURN_I_FACTOR	( 0.00004545)
// #define PID_TURN_D_FACTOR	( 8.101919)

// forward = 100で挑戦
#define PID_TURN_P_FACTOR	( 0.43814)
#define PID_TURN_I_FACTOR	( 0.000010)
#define PID_TURN_D_FACTOR	( 6.901919)



/*******************************************************************************
	prototype
*******************************************************************************/
static int	PID_calculateTurn(
			PID *this,
			float theP_Factor, float theI_Factor, float theD_Factor,
			int theBrightnessDeviation, int theEdgeFactor
		);



/***************************************************************************//**
	@brief		初期化する
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID 構造体へのポインタ
	@param		theBrightnessDeviation	明るさの偏差
	@return		なし
*//****************************************************************************/
void	PID_initialize(PID *this, int theBrightnessDeviation)
{
	this->brightness_P	= theBrightnessDeviation;
	this->brightness_I	= 0;
}



/***************************************************************************//**
	@brief		turn 値を取得する
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID 構造体へのポインタ
	@param		theBrightnessDeviation	明るさの偏差
	@param		theEdge			走行エッジ係数 (-1：右，1：左)
<<<<<<< HEAD
	@return		turn 値 : -100 (左旋回) ～ 100 (右旋回)
=======
	@return		turn 値 : -100 (左旋回) 〜 100 (右旋回)
>>>>>>> 文字コード変更
*//****************************************************************************/
int	PID_getTurn(PID *this, int theBrightnessDeviation, int theEdge)
{
	int	turn;

	turn =	PID_calculateTurn(
			this,
			PID_TURN_P_FACTOR,
			PID_TURN_I_FACTOR,
			PID_TURN_D_FACTOR,
			theBrightnessDeviation,
			theEdge
			//エッジは右が-1で左が1
		);

	return turn;
}



/***************************************************************************//**
	@brief		PID 制御用の turn 値を取得する
	@author		takigawa
	@date		2012/05/17

	@param		*this			PID 構造体へのポインタ
	@param		theP_Factor		P 成分の係数
	@param		theI_Factor		I 成分の係数
	@param		theD_Factor		D 成分の係数
	@param		theBrightnessDeviation	明るさの偏差
	@param		theEdgeFactor		走行エッジ係数 (-1：右，1：左)
<<<<<<< HEAD
	@return		turn 値 : -100 (左旋回) ～ 100 (右旋回)
=======
	@return		turn 値 : -100 (左旋回) 〜 100 (右旋回)
>>>>>>> 文字コード変更
*//****************************************************************************/
int	PID_calculateTurn(
		PID *this,
		float theP_Factor, float theI_Factor, float theD_Factor,
		int theBrightnessDeviation, int theEdgeFactor
	)
{
// <<<<<<< HEAD
	int	turn;			/* 旋回命令: -100 (左旋回) ～ 100 (右旋回) */
// ======
//	int	turn;			/* 旋回命令: -100 (左旋回) 〜 100 (右旋回) */
// >>>>>>> 文字コード変更
	int	turn_P;			/* 旋回命令の比例成分 */
	int	turn_I;			/* 旋回命令の積分成分 */
	int	turn_D;			/* 旋回命令の微分成分 */
	int	brightness_P;		/* 明るさの P 成分 */
	int	brightness_I;		/* 明るさの I 成分 */
	int	brightness_D;		/* 明るさの D 成分 */

	brightness_P		= theBrightnessDeviation;
	brightness_I		= this->brightness_I + theBrightnessDeviation;
	brightness_D		= theBrightnessDeviation - this->brightness_P;

	this->brightness_P	= theBrightnessDeviation;
	this->brightness_I	= brightness_I;

	/*
		P 成分から turn を求める
	*/
	turn_P = (int)(theP_Factor * (float)brightness_P);
	turn_P *= theEdgeFactor;

	/*
		I 成分から turn を求める
	*/
	turn_I = (int)(theI_Factor * (float)brightness_I);
	turn_I *= theEdgeFactor;

	/*
		D 成分から turn を求める
	*/
	turn_D = (int)(theD_Factor * (float)brightness_D);
	turn_D *= theEdgeFactor;

	/*
		turn を決定する
	*/
	turn = turn_P + turn_I + turn_D;
	if(turn >= PID_TURN_LIMIT) {
		turn = PID_TURN_LIMIT;
	} else if (turn <= (-1 * PID_TURN_LIMIT)) {
		turn = -1 * PID_TURN_LIMIT;
	}

	return turn;
}

