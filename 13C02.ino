#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters for Servo
#define _DUTY_MIN 500 
#define _DUTY_NEU 1525 
#define _DUTY_MAX 2550 


#define _SERVO_SPEED 3.0       
#define _POS_START _DUTY_MIN   
#define _POS_END   _DUTY_MAX   


#define INTERVAL 20     


unsigned long last_sampling_time; 
unsigned long start_time;         

Servo myservo;

float duty_per_msec;  
int duty_curr;        

void setup() {
  myservo.attach(PIN_SERVO, _DUTY_MIN, _DUTY_MAX);

  duty_curr = _POS_START;
  myservo.writeMicroseconds(duty_curr);

  // initialize serial port
  Serial.begin(57600);

  // convert angular velocity into duty change per msec.
  duty_per_msec = (_DUTY_MAX - _DUTY_MIN) / 180.0 * _SERVO_SPEED / 1000.0;

  delay(3000);  // hold at start position before moving

  start_time = last_sampling_time = millis();
}

void loop() {
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  last_sampling_time += INTERVAL;

  unsigned long elapsed = millis() - start_time;
  float duty = _POS_START + duty_per_msec * elapsed;
  if (duty > _POS_END)
    duty = _POS_END;
  duty_curr = duty + 0.5;  

  myservo.writeMicroseconds(duty_curr);

  Serial.print("time:");   Serial.print(elapsed / 1000.0);
  Serial.print(",angle:"); Serial.print((duty - _DUTY_MIN) * 180.0 / (_DUTY_MAX - _DUTY_MIN));
  Serial.print(",duty:");  Serial.println(duty_curr);
}
