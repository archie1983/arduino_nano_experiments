/*

*/
int led = 6;//LED_BUILTIN;
int btn = 12;
int forward_pin = 11;
int back_pin = 10;
int button_pressed = 0;

int soundInput = A7;    // select the input pin for the microphone
int soundLoudness = 0;  // variable to store the value coming from the microphone
#define SOUND_THRESHOLD 7
#define NOMINAL_LEVEL 130

char response[12]; // response to send to USB serial to monitor mic

enum motor_state {
  FORWARD1,
  FORWARD2,
  FORWARD3,
  BACK1,
  BACK2,
  BACK3,
  IDLE
};

motor_state msMotor = IDLE;

void setup() {
    pinMode(led, OUTPUT);
    pinMode(forward_pin, OUTPUT);
    pinMode(back_pin, OUTPUT);
    pinMode(btn, INPUT);

    Serial.begin(9600);
}

void loop() {
  int motorPower = 0;
  int soundDeviation = 0;
  soundLoudness = analogRead(soundInput);
  
  soundLoudness = map(soundLoudness, 0, 1023, 0, 255);
  soundDeviation = abs(soundLoudness - NOMINAL_LEVEL);
  motorPower = soundDeviation * 20;
  
  if (motorPower > 240) {
    motorPower = 240;
  }
  
  //if (Serial.available()) {      // If anything comes in Serial (USB),
  itoa(soundDeviation, response, 10);
  //Serial.write(response);
  //Serial.write("\n");

  //# if sound level reached the threshold then we start doing things
  if (soundDeviation >= SOUND_THRESHOLD) {
    //# first advancing the state machine
    switch (msMotor) {
      case IDLE:
        msMotor = FORWARD1;
        break;
      case FORWARD1:
        msMotor = FORWARD2;
        break;
      case FORWARD2:
        msMotor = FORWARD3;
        break;
      case FORWARD3:
        msMotor = BACK1;
        break;
      case BACK1:
        msMotor = BACK2;
        break;
      case BACK2:
        msMotor = BACK3;
        break;
      case BACK3:
        msMotor = IDLE;
        break;
    }
    
    //# now running the motor
    switch (msMotor) {
      case IDLE:
        makeMotorIdle();
        break;
      case FORWARD1:
      case FORWARD2:
      case FORWARD3:
      case BACK1:
      case BACK2:
      case BACK3:
        //runMotorWithSoftStart(msMotor, motorPower);
        runMotorWithHardStart(msMotor, motorPower);
        break;
    }

    //# now letting the motor run for a short time before listening to the microphone again
    delay(2000);
    makeMotorIdle();
  } else {
    //delay(30);
  }
}

void runMotorWithHardStart(motor_state msMotorState, int motorPower) {
  Serial.write("DIR=");
  
  if (msMotorState == FORWARD1 ||
      msMotorState == FORWARD2 ||
      msMotorState == FORWARD3) {
    digitalWrite(back_pin, LOW);
    digitalWrite(forward_pin, HIGH);
    Serial.write("FORWARD");
  } else if (msMotorState == BACK1 ||
      msMotorState == BACK2 ||
      msMotorState == BACK3) {
    digitalWrite(forward_pin, LOW);
    digitalWrite(back_pin, HIGH);
    Serial.write("BACK");
  }
  
  Serial.write("\n");
  analogWrite(led, motorPower);
}

void runMotorWithSoftStart(motor_state msMotorState, int motorPower) {

  if (msMotorState == FORWARD1 ||
      msMotorState == FORWARD2 ||
      msMotorState == FORWARD3) {
    digitalWrite(back_pin, LOW);
    motorSoftStart(forward_pin, motorPower);
  } else if (msMotorState == BACK1 ||
      msMotorState == BACK2 ||
      msMotorState == BACK3) {
    digitalWrite(forward_pin, LOW);
    motorSoftStart(back_pin, motorPower);
  }

  analogWrite(led, motorPower);
}

void motorSoftStart(int intPin, int intMotorPower) {
  div_t divresult;

  int intIncrement = 20;
  int intNumOfSteps = 0;
  int intCounter = 0;

  divresult = div(intMotorPower, intIncrement);
  intNumOfSteps = divresult.quot;
  
  for (intCounter = 1; intCounter <= intNumOfSteps; intCounter++) {
    analogWrite(intPin, intCounter * intIncrement);
    delay(250);
  }
}

void makeMotorIdle() {
  digitalWrite(led, LOW);
  digitalWrite(forward_pin, LOW);
  digitalWrite(back_pin, LOW);
}
