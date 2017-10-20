#ifndef __PUBLIC_H
#define __PUBLIC_H

#define MOTOR_1   0
#define MOTOR_2   1
#define MOTOR_3   2
#define MOTOR_4   3
#define MOTOR_5   4
#define MOTOR_6   5
#define MOTOR_7   6
#define MOTOR_8   7
#define MOTOR_9   8
#define MOTOR_10  9
#define MOTOR_11  10


#define RUN_MODE_RESET 1
#define RUN_MODE_RUN   2

#define MOTOR_STATUS_RUN   1
#define MOTOR_STATUS_STOP  0

//电机运行模式：环型，条型
#define MOTOR_RUN_MODE_CIRCLE 1 
#define MOTOR_RUN_MODE_LINE   2

typedef struct tag_Motor_Info {
	unsigned char motor_type;   //电机序号,因为多个电机共用定时器
	unsigned char dir;   		//运行方向
	unsigned int steps;  	 	//运行的步数
	unsigned int max_steps;  	//运行最大的步数
	unsigned char qk_count;     //  缺口数量
	unsigned char run_mode;     //运行模式：正常运行、复位
	unsigned char motor_run_mode; //电机运行模式：环型，条型
	unsigned char use_go;         //是否 使用光耦标志
	unsigned char status_1,status_2,temp_count;
}MOTOR_INFO;


#endif
