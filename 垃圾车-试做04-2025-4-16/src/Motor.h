#ifndef Motor_h
#define Motor_h
#include <Arduino.h>

// #define CONFIG_MOTORA_PWM_PIN       13
// #define CONFIG_MOTORA_PWM_CHN       0
// #define CONFIG_MOTORA_INA_PIN		12
// #define CONFIG_MOTORA_INB_PIN       14

// #define CONFIG_MOTORB_PWM_PIN       27
// #define CONFIG_MOTORB_PWM_CHN       1
// #define CONFIG_MOTORB_INA_PIN		26
// #define CONFIG_MOTORB_INB_PIN       25

// #define CONFIG_MOTORC_PWM_PIN       5
// #define CONFIG_MOTORC_PWM_CHN       2
// #define CONFIG_MOTORC_INA_PIN		18
// #define CONFIG_MOTORC_INB_PIN       19

// #define CONFIG_MOTORD_PWM_PIN       4
// #define CONFIG_MOTORD_PWM_CHN       3
// #define CONFIG_MOTORD_INA_PIN		16
// #define CONFIG_MOTORD_INB_PIN       17

#define CONFIG_MOTORA_PWM_PIN       13
#define CONFIG_MOTORA_PWM_CHN       0
#define CONFIG_MOTORA_INA_PIN		14
#define CONFIG_MOTORA_INB_PIN       21

#define CONFIG_MOTORB_PWM_PIN       47
#define CONFIG_MOTORB_PWM_CHN       1
#define CONFIG_MOTORB_INA_PIN		0
#define CONFIG_MOTORB_INB_PIN       45

#define CONFIG_MOTORC_PWM_PIN       35
#define CONFIG_MOTORC_PWM_CHN       2
#define CONFIG_MOTORC_INA_PIN		36
#define CONFIG_MOTORC_INB_PIN       37

#define CONFIG_MOTORD_PWM_PIN       38
#define CONFIG_MOTORD_PWM_CHN       3
#define CONFIG_MOTORD_INA_PIN		39
#define CONFIG_MOTORD_INB_PIN       40

#define CONFIG_MOTOR_PROPULSION1      41
#define CONFIG_MOTOR4_PWM_CHN       6

#define CONFIG_MOTOR_PROPULSION2      42
#define CONFIG_MOTOR5_PWM_CHN       7

#define CONFIG_MOTOR_CONVEYOR       4
#define CONFIG_MOTOR6_PWM_CHN       5

enum  motor_t
{
    MotorA,
    MotorB,
    MotorC,
    MotorD,

};

void motor_init();
void motor_duty_set(motor_t motor,uint32_t duty);
void motor_dir_set(motor_t motor,uint8_t dir);
void motor_function(uint8_t func_num,uint8_t mode_num);

#endif