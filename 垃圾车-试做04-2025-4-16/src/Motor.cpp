#include "motor.h"

           
          	  

void motor_init()
{
    ledcSetup(CONFIG_MOTORA_PWM_CHN, 10000, 10); 
    ledcAttachPin(CONFIG_MOTORA_PWM_PIN, CONFIG_MOTORA_PWM_CHN); 

    pinMode(CONFIG_MOTORA_INA_PIN,OUTPUT);
    pinMode(CONFIG_MOTORA_INB_PIN,OUTPUT);
    
    digitalWrite(CONFIG_MOTORA_INA_PIN,LOW);
    digitalWrite(CONFIG_MOTORA_INB_PIN,LOW);


    ledcSetup(CONFIG_MOTORB_PWM_CHN, 10000, 10); 
    ledcAttachPin(CONFIG_MOTORB_PWM_PIN, CONFIG_MOTORB_PWM_CHN);   

    pinMode(CONFIG_MOTORB_INA_PIN,OUTPUT);
    pinMode(CONFIG_MOTORB_INB_PIN,OUTPUT);
    
    digitalWrite(CONFIG_MOTORB_INA_PIN,LOW);
    digitalWrite(CONFIG_MOTORB_INB_PIN,LOW);


    ledcSetup(CONFIG_MOTORC_PWM_CHN, 10000, 10); 
    ledcAttachPin(CONFIG_MOTORC_PWM_PIN, CONFIG_MOTORC_PWM_CHN); 
    
    pinMode(CONFIG_MOTORC_INA_PIN,OUTPUT);
    pinMode(CONFIG_MOTORC_INB_PIN,OUTPUT);
    
    digitalWrite(CONFIG_MOTORC_INA_PIN,LOW);
    digitalWrite(CONFIG_MOTORC_INB_PIN,LOW);


    ledcSetup(CONFIG_MOTORD_PWM_CHN, 10000, 10); 
    ledcAttachPin(CONFIG_MOTORD_PWM_PIN, CONFIG_MOTORD_PWM_CHN); 
    
    pinMode(CONFIG_MOTORD_INA_PIN,OUTPUT);
    pinMode(CONFIG_MOTORD_INB_PIN,OUTPUT);
    
    digitalWrite(CONFIG_MOTORD_INA_PIN,LOW);
    digitalWrite(CONFIG_MOTORD_INB_PIN,LOW);


    pinMode(CONFIG_MOTOR_PROPULSION1, OUTPUT);
    ledcSetup(CONFIG_MOTOR4_PWM_CHN, 50, 10); 
    ledcAttachPin(CONFIG_MOTOR_PROPULSION1, CONFIG_MOTOR4_PWM_CHN);
    ledcWrite(CONFIG_MOTOR4_PWM_CHN, 77); 
    

    pinMode(CONFIG_MOTOR_PROPULSION2, OUTPUT);
    ledcSetup(CONFIG_MOTOR5_PWM_CHN, 50, 10); 
    ledcAttachPin(CONFIG_MOTOR_PROPULSION2, CONFIG_MOTOR5_PWM_CHN);
    ledcWrite(CONFIG_MOTOR5_PWM_CHN, 77); 


    pinMode(CONFIG_MOTOR_CONVEYOR,OUTPUT);
    ledcSetup(CONFIG_MOTOR6_PWM_CHN, 50, 10); 
    ledcAttachPin(CONFIG_MOTOR_CONVEYOR, CONFIG_MOTOR6_PWM_CHN); 
    ledcWrite(CONFIG_MOTOR6_PWM_CHN, 77); 
    

}

void motor_function(uint8_t func_num,uint8_t mode_num)
{
    switch (func_num)
    {
    case 0:
      if (mode_num == 0) {
        delay(50);
        ledcWrite(CONFIG_MOTOR4_PWM_CHN, 77);
      } 
      else if (mode_num == 1) {
        delay(50);
        ledcWrite(CONFIG_MOTOR4_PWM_CHN, 80);
      }
      else if (mode_num == 2) {
        delay(50);
        ledcWrite(CONFIG_MOTOR4_PWM_CHN, 3800);
      }
      break;
    case 1:
      if (mode_num == 0) {
        delay(50);
        ledcWrite(CONFIG_MOTOR5_PWM_CHN, 77);
      } 
      else if (mode_num == 1) {
        delay(50);
        ledcWrite(CONFIG_MOTOR5_PWM_CHN, 80);
      }
      else if (mode_num == 2) {
        delay(50);
        ledcWrite(CONFIG_MOTOR5_PWM_CHN, 3800);
      }
      break;
    case 2:
      if (mode_num == 0) {
        delay(50);
        ledcWrite(CONFIG_MOTOR6_PWM_CHN, 77);
      } 
      else if (mode_num == 1) {
        delay(50);
        ledcWrite(CONFIG_MOTOR6_PWM_CHN, 80);
      }
      else if (mode_num == 2) {
        delay(50);
        ledcWrite(CONFIG_MOTOR6_PWM_CHN, 2700);
      }
      break;
    default: break;
    }
}

void motor_duty_set(motor_t motor,uint32_t duty)
{
    switch (motor)
    {
    case 0: ledcWrite(CONFIG_MOTORA_PWM_CHN, duty); break;
    case 1: ledcWrite(CONFIG_MOTORB_PWM_CHN, duty); break;
    case 2: ledcWrite(CONFIG_MOTORC_PWM_CHN, duty); break;
    case 3: ledcWrite(CONFIG_MOTORD_PWM_CHN, duty); break;
    // case 4: ledcWrite(CONFIG_MOTOR_PROPULSION1, duty); break;
    // case 5: ledcWrite(CONFIG_MOTOR_PROPULSION2, duty); break;
    // case 6: ledcWrite(CONFIG_MOTOR_CONVEYOR, duty); break;
    default: break;
    }
}

void motor_dir_set(motor_t motor, uint8_t dir) {
  switch (motor) {
    case MotorA:
      if (dir == 0) {
        digitalWrite(CONFIG_MOTORA_INA_PIN, LOW);
        digitalWrite(CONFIG_MOTORA_INB_PIN, HIGH);
      } else {
        digitalWrite(CONFIG_MOTORA_INA_PIN, HIGH);
        digitalWrite(CONFIG_MOTORA_INB_PIN, LOW);
      }
      break;
    case MotorB:
      if (dir == 0) {
        digitalWrite(CONFIG_MOTORB_INA_PIN, LOW);
        digitalWrite(CONFIG_MOTORB_INB_PIN, HIGH);
      } else {
        digitalWrite(CONFIG_MOTORB_INA_PIN, HIGH);
        digitalWrite(CONFIG_MOTORB_INB_PIN, LOW);
      }
      break;
    case MotorC:
      if (dir == 0) {
        digitalWrite(CONFIG_MOTORC_INA_PIN, LOW);
        digitalWrite(CONFIG_MOTORC_INB_PIN, HIGH);
      } else {
        digitalWrite(CONFIG_MOTORC_INA_PIN, HIGH);
        digitalWrite(CONFIG_MOTORC_INB_PIN, LOW);
      }
      break;
    case MotorD:
      if (dir == 0) {
        digitalWrite(CONFIG_MOTORD_INA_PIN, LOW);
        digitalWrite(CONFIG_MOTORD_INB_PIN, HIGH);
      } else {
        digitalWrite(CONFIG_MOTORD_INA_PIN, HIGH);
        digitalWrite(CONFIG_MOTORD_INB_PIN, LOW);
      }
      break;

    default:
      break;
  }
}
