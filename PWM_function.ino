#define PIN_LED 7
#define PWM_PERIOD_US 10000

int pwm_period;
int pwm_duty;
unsigned long triangle_start;

void set_period(int period) {
  if(period < 100){
    period = 100;
  }else if(period > 10000){
    period = 10000;
  }
  
  pwm_period = period;
}

void set_duty(int duty) {
  if(duty < 0){
    duty = 0;
  }else if(duty > 100){
    duty = 100;
  }

  pwm_duty = duty;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  set_period(PWM_PERIOD_US);
  set_duty(0);
  triangle_start = micros();
}

void loop() {
  unsigned long elapsed = (micros() - triangle_start) % 1000000UL;
  int duty;
  
  if (elapsed < 500000UL) {
    duty = elapsed / 5000UL;
  } else {
    unsigned long remaining = 1000000UL - elapsed;
    duty = remaining / 5000UL;
  }
  
  set_duty(duty);

  unsigned long on_time = (unsigned long)pwm_period * pwm_duty / 100;
  unsigned long off_time = pwm_period - on_time;

  if (on_time > 0) {
    digitalWrite(PIN_LED, LOW);
    delayMicroseconds(on_time);
  }
  if (off_time > 0) {
    digitalWrite(PIN_LED, HIGH);
    delayMicroseconds(off_time);
  }
}
