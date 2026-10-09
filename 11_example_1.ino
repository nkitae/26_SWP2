#include <Servo.h>



// Arduino pin assignment

#define PIN_LED   9   // LED active-low

#define PIN_TRIG  12  // sonar sensor TRIGGER

#define PIN_ECHO  13  // sonar sensor ECHO

#define PIN_SERVO 10  // servo motor



// configurable parameters for sonar

#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)

#define INTERVAL 25      // sampling interval (unit: msec)

#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)

#define _DIST_MIN 180.0   // minimum distance to be measured (unit: mm)

#define _DIST_MAX 360.0   // maximum distance to be measured (unit: mm)



#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)

#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance



#define _EMA_ALPHA 0.2    // EMA weight of new sample (range: 0 to 1)

                          // Setting EMA to 1 effectively disables EMA filter.




// duty duration for myservo.writeMicroseconds()

// NEEDS TUNING (servo by servo)

 

#define _DUTY_MIN 500 // servo full clockwise position (0 degree)

#define _DUTY_NEU 1525 // servo neutral position (90 degree)

#define _DUTY_MAX 2550 // servo full counterclockwise position (180 degree)



// global variables

float  dist_ema, dist_prev = _DIST_MIN; // unit: mm

unsigned long last_sampling_time = 0;       // unit: ms



Servo myservo;



void setup() {

  // initialize GPIO pins

  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_TRIG, OUTPUT);    // sonar TRIGGER

  pinMode(PIN_ECHO, INPUT);     // sonar ECHO

  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 



  myservo.attach(PIN_SERVO); 

  myservo.writeMicroseconds(_DUTY_NEU);



  // initialize USS related variables

  dist_prev = _DIST_MIN; // raw distance output from USS (unit: mm)



  // initialize serial port

  Serial.begin(57600);

}



void loop() {

  float  dist_raw, dist_filtered;

  

  // wait until next sampling time.

  // millis() returns the number of milliseconds since the program started. 

  // will overflow after 50 days.

  if (millis() < last_sampling_time + INTERVAL)

    return;



  // get a distance reading from the USS

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);



  // the range filter

  bool in_range =
    (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX);

  if (dist_raw == 0.0 || !in_range) {
    dist_filtered = dist_prev;
  }
  else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }



  // Modify the below line to implement the EMA equation

  dist_ema = _EMA_ALPHA * dist_filtered + (1.0 - _EMA_ALPHA) * dist_ema;



  // adjust servo position according to the USS read value

  // add your code here!
  float angle;

  if (dist_ema <= _DIST_MIN) {
    angle = 0.0;
  }
  else if (dist_ema >= _DIST_MAX) {
    angle = 180.0;
  }
  else {
    angle = (dist_ema - _DIST_MIN) * 180.0
          / (_DIST_MAX - _DIST_MIN);
  }

  int duty = _DUTY_MIN
           + (angle / 180.0)
           * (_DUTY_MAX - _DUTY_MIN);

  myservo.writeMicroseconds(duty);


  if (in_range) {
    digitalWrite(PIN_LED, LOW);
  }
  else {
    digitalWrite(PIN_LED, HIGH);
  }







  // output the distance to the serial port

  Serial.print("Min:");    Serial.print(_DIST_MIN);

  Serial.print(",dist:");  Serial.print(dist_raw);

  Serial.print(",ema:");Serial.print(dist_ema);

  Serial.print(",Servo:"); Serial.print(myservo.read());  

  Serial.print(",Max:");   Serial.print(_DIST_MAX);

  Serial.println("");

 

  // update last sampling time

  last_sampling_time += INTERVAL;

}



// get a distance reading from USS. return value is in millimeter.

float USS_measure(int TRIG, int ECHO)

{

  digitalWrite(TRIG, HIGH);

  delayMicroseconds(PULSE_DURATION);

  digitalWrite(TRIG, LOW);

  

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

}
