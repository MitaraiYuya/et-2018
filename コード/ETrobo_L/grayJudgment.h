#ifndef _grayJudgment
#define _grayJudgment
bool_t GrayJudgment(int light_valu,int motor_ang_l, int motor_ang_r);
void GrayJudgment_makeThreshiold(int come_threshiold, int come_turn_frequency,int come_gray_valu);
//bool_t GrayJudgment_log_send(int* light_value, int* motor_ang_l, int* motor_ang_r);//PCへlog送信用
bool_t GrayJudgment_log_send_2(int* light_value, int* motor_ang_l, int* motor_ang_r, int* gray_area, int *gray_juge_result);
#endif