#include <AccelStepper.h>
#include <MultiStepper.h>

#define STEPS_PER_REV 2048
#define RPM 3

#define NB_TURNS_BY_DAY 1600
#define MINUTES_TO_WAIT 20

#define PIN_BUTTON_1 4
#define PIN_BUTTON_2 3
#define PIN_BUTTON_3 2

AccelStepper motor1(AccelStepper::FULL4WIRE, 12, 10, 11, 9);
AccelStepper motor2(AccelStepper::FULL4WIRE, 8, 6, 7, 5);
AccelStepper motor3(AccelStepper::FULL4WIRE, 19, 17, 18, 16);

const long secondsToMakeATurn = 60 / RPM;
const long microsecondsToMakeATurn = secondsToMakeATurn * 1000.0 * 1000.0;
const long secondsToMakeAllTurn = NB_TURNS_BY_DAY * secondsToMakeATurn;
const long secondsToWaitByDay = 24.0 * 3600.0 - secondsToMakeAllTurn;
const int nbWaitingTime = secondsToWaitByDay / (MINUTES_TO_WAIT * 60.0);
const int nbRotationByTurn = NB_TURNS_BY_DAY / nbWaitingTime;
const long long waitingTimeInMicroseconds = MINUTES_TO_WAIT * 60.0 * 1000.0 * 1000.0;

enum State {
  switchOff = 0,
  turn = 1,
  wait = 2
};

// Init global values
State state1 = switchOff;
State state2 = switchOff;
State state3 = switchOff;


bool button1Triggered = false;
bool button2Triggered = false;
bool button3Triggered = false;

int nbRotationToMakeMotor1 = 0;
int nbRotationToMakeMotor2 = 0;
int nbRotationToMakeMotor3 = 0;

long long microsecondsToWaitMotor1 = 0;
long long microsecondsToWaitMotor2 = 0;
long long microsecondsToWaitMotor3 = 0;

int sequenceRotationDirection[] = {1, 1, -1, -1};
int pointerOnSequenceRotation = 0;

void setup()
{
  Serial.begin(9600);

  float stepPerSecond = STEPS_PER_REV / secondsToMakeATurn;

  motor1.disableOutputs();
  motor2.disableOutputs();
  motor3.disableOutputs();

  motor1.setMaxSpeed(stepPerSecond);
  motor2.setMaxSpeed(stepPerSecond);
  motor3.setMaxSpeed(stepPerSecond);

  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  pinMode(PIN_BUTTON_3, INPUT_PULLUP);

}

void loop()
{
  const long startLoopTime = micros();

  checkStateOfButtons();

  moveMotors();
  decountWaitingTime(startLoopTime);

  checkNextState();

  // Increase pointer on sequence rotation
  pointerOnSequenceRotation++;
  pointerOnSequenceRotation = pointerOnSequenceRotation % 4;
}

void checkStateOfButtons()
{
  // Reverse value read cause by INPUT_PULLUP
  int stateButton1 = !digitalRead(PIN_BUTTON_1);
  int stateButton2 = !digitalRead(PIN_BUTTON_2);
  int stateButton3 = !digitalRead(PIN_BUTTON_3);

  if (stateButton1 != button1Triggered)
  {
    button1Triggered = stateButton1;
    resetStateMotor1();
  }

  if (stateButton2 != button2Triggered)
  {
    button2Triggered = stateButton2;
    resetStateMotor2();
  }

  if (stateButton3 != button3Triggered)
  {
    button3Triggered = stateButton3;
    resetStateMotor3();
  }
}



/* Motor 1
    Functions
*/
void toStateSwitchOff1()
{
  nbRotationToMakeMotor1 = 0;
  microsecondsToWaitMotor1 = 0;
  motor1.disableOutputs();

  state1 = switchOff;

}

void toStateTurn1()
{
  nbRotationToMakeMotor1 = nbRotationByTurn;
  microsecondsToWaitMotor1 = waitingTimeInMicroseconds;

  state1 = turn;

  motor1.enableOutputs();
}

void toStateWait1()
{
  nbRotationToMakeMotor1 = 0;

  state1 = wait;

  motor1.disableOutputs();
}

void resetStateMotor1()
{
  if (button1Triggered)
  {
    toStateTurn1();
  }
  else
  {
    toStateSwitchOff1();
  }
}

/* Motor 2
    Functions
*/
void toStateSwitchOff2()
{
  nbRotationToMakeMotor2 = 0;
  microsecondsToWaitMotor2 = 0;

  state2 = switchOff;

  motor2.disableOutputs();
}

void toStateTurn2()
{
  nbRotationToMakeMotor2 = nbRotationByTurn;
  microsecondsToWaitMotor2 = waitingTimeInMicroseconds;

  state2 = turn;

  motor2.enableOutputs();
}

void toStateWait2()
{
  nbRotationToMakeMotor2 = 0;

  state2 = wait;

  motor2.disableOutputs();
}

void resetStateMotor2()
{
  if (button2Triggered)
  {
    toStateTurn2();
  }
  else
  {
    toStateSwitchOff2();
  }
}

/* Motor 3
    Functions
*/
void toStateSwitchOff3()
{
  nbRotationToMakeMotor3 = 0;
  microsecondsToWaitMotor3 = 0;

  state3 = switchOff;

  motor3.disableOutputs();
}

void toStateTurn3()
{
  nbRotationToMakeMotor3 = nbRotationByTurn;
  microsecondsToWaitMotor3 = waitingTimeInMicroseconds;

  state3 = turn;

  motor3.enableOutputs();
}

void toStateWait3()
{
  nbRotationToMakeMotor3 = 0;

  state3 = wait;

  motor3.disableOutputs();
}

void resetStateMotor3()
{
  if (button3Triggered)
  {
    toStateTurn3();
  }
  else
  {
    toStateSwitchOff3();
  }
}

void moveMotors()
{
  MultiStepper motors;

  int nbMotorsAdded = 0;

  if (state1 == turn)
  {
    motors.addStepper(motor1);
    ++nbMotorsAdded;
    nbRotationToMakeMotor1 = nbRotationToMakeMotor1 - 1;
  }

  if (state2 == turn)
  {
    motors.addStepper(motor2);
    ++nbMotorsAdded;
    nbRotationToMakeMotor2 = nbRotationToMakeMotor2 - 1;
  }

  if (state3 == turn)
  {
    motors.addStepper(motor3);
    ++nbMotorsAdded;
    nbRotationToMakeMotor3 = nbRotationToMakeMotor3 - 1;
  }

  long positions[nbMotorsAdded];

  for (int i = 0; i < nbMotorsAdded; ++i) {
    positions[i] = STEPS_PER_REV * sequenceRotationDirection[pointerOnSequenceRotation];
  }

  motors.moveTo(positions);
  motors.runSpeedToPosition();

  motor1.setCurrentPosition(0);
  motor2.setCurrentPosition(0);
  motor3.setCurrentPosition(0);
}

void decountWaitingTime(const long& startLoopTime)
{
  const long elapsedTimeInLoop = micros() - startLoopTime;

  if (elapsedTimeInLoop > 0)
  {
    if (state1 == wait)
    {
      microsecondsToWaitMotor1 = microsecondsToWaitMotor1 - elapsedTimeInLoop;
    }

    if (state2 == wait)
    {
      microsecondsToWaitMotor2 = microsecondsToWaitMotor2 - elapsedTimeInLoop;
    }

    if (state3 == wait)
    {
      microsecondsToWaitMotor3 = microsecondsToWaitMotor3 - elapsedTimeInLoop;
    }
  }
}

void checkNextState()
{
  if (state1 == turn)
  {
    if (nbRotationToMakeMotor1 <= 0)
    {
      toStateWait1();
    }
  }
  else if (state1 == wait)
  {
    if (microsecondsToWaitMotor1 <= 0)
    {
      toStateTurn1();
    }
  }

  if (state2 == turn)
  {
    if (nbRotationToMakeMotor2 <= 0)
    {
      toStateWait2();
    }
  }
  else if (state2 == wait)
  {
    if (microsecondsToWaitMotor2 <= 0)
    {
      toStateTurn2();
    }
  }

  if (state3 == turn)
  {
    if (nbRotationToMakeMotor3 <= 0)
    {
      toStateWait3();
    }
  }
  else if (state3 == wait)
  {
    if (microsecondsToWaitMotor3 <= 0)
    {
      toStateTurn3();
    }
  }
}
