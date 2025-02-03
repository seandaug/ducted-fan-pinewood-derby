/*
This code is for an Adafruit Trinket M0 controller,
an M4 Radio Frequency (RF) control (or a generic 4-button RF control in momentary mode) such as QIACHIP RX480E,
an EDF such as X-Fly Galaxy X4,
an ESC compatible with the EDF,
a 4S LiPo battery

When powered on, the fan will beep 4 times (because of a 4S battery), then a long beep and then be ready to accept inputs from the remote.
Button A turns it on in "staged mode" with a lower power fan speed ready to go in the starting block.
When in staged mode:
  Button A turns it off
  Button B, C, or D turns on the fan for racing at a low, medium, or high speed (repsectively) for the specified race duration, then turns off
*/

#include <Adafruit_SoftServo.h>  // Install "Adafruit SoftServo by Adafruit 1.0.4" from Arduino library
#include <Adafruit_ZeroTimer.h>  // Install "Adafruit ZeroTimer Library by Adafruit 2.2.4" from Arduino library

#define SERVO_PIN  0                         // Servo control output pin
#define A_BUTTON_PIN 1                       // Remote A input pin
#define B_BUTTON_PIN 2                       // Remote B input pin
#define C_BUTTON_PIN 3                       // Remote C input pin
#define D_BUTTON_PIN 4                       // Remote D input pin

#define OFF_SPEED    0                       // Lowest ESC speed (range 0-180 as per attach method)
#define STAGED_SPEED 20                      // ESC speed when staged before race start
#define LOW_SPEED    40
#define MED_SPEED    70
#define HIGH_SPEED   100

#define MIN_ON_OFF_DURATION 1000          // Minimum duration before you can push A again to turn it on/off
#define RACE_DURATION 1100                // Duration of racing speed in milliseconds before turning off

unsigned long onOffTime = 0;
unsigned long racingTime = 0;

Adafruit_SoftServo esc;

// Choose which hardware timer to use:
// SAMD21 has TCC0, TCC1, TCC2, TC3, TC4, TC5 ...
// Many people pick TC3 to avoid conflicts. So do we.
Adafruit_ZeroTimer myTimer = Adafruit_ZeroTimer(3);

// Define the interrupt handler
void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

enum raceState {
  OFF,
  STAGED,
  L,      // Low speed race
  M,      // Medium speed race
  H       // High speed race
};
raceState currentState = OFF;

// Adafruit_SoftServo says "For best results, use with a timer0 interrupt to refresh() all your servos once every 20 milliseconds!"
// This function gets called every 20 ms.
volatile bool togglepin = false;
void myTimerCallback() {
  esc.refresh();
  // Make the LED on the Trinket flash on and off to show the speed of the timer and know that it's running
  digitalWrite(LED_BUILTIN, togglepin);
  togglepin = !togglepin;
}

void setup() {
  // ESC
  esc.attach(SERVO_PIN);                     // Attach the pin to the software servo library
  // LED
  pinMode(LED_BUILTIN, OUTPUT);              // Set Trinket pin as an output for LED
  // Remote
  pinMode(A_BUTTON_PIN, INPUT);              // Set up the remote's A button as input
  pinMode(B_BUTTON_PIN, INPUT);              // Set up the remote's B button as input
  pinMode(C_BUTTON_PIN, INPUT);              // Set up the remote's C button as input
  pinMode(D_BUTTON_PIN, INPUT);              // Set up the remote's D button as input

  // Configure the timer:
  myTimer.enable(false);
  //--------------------------------------------------------------
  // 1) Configure the timer prescaler, size, and wave generation
  //
  //    We'll use:
  //      - Prescaler:  /64 => 48MHz / 64 = 750kHz timer tick
  //      - 16-bit mode
  //      - MATCH_FREQ wave generation => the timer resets when
  //        it reaches the "period" value.
  //      - Up-counting
  //--------------------------------------------------------------
  myTimer.configure(
    TC_CLOCK_PRESCALER_DIV64,     // prescaler
    TC_COUNTER_SIZE_16BIT,        // 16-bit timer
    TC_WAVE_GENERATION_MATCH_FREQ // reset when match is reached
  );

  //--------------------------------------------------------------
  // 2) Set the period to get a 20ms rate = 50 Hz
  //
  //    20ms * 750kHz = 15,000 counts
  //    We'll set period to 15000 so the timer runs from 0..14999
  //    If you prefer an exact 0..14999 cycle, use (14999) for period,
  //    but 15000 is generally fine for a ~20ms rate.
  //--------------------------------------------------------------
  uint32_t period = 15000;  // yields 50 Hz
  uint8_t  chann  = 0;      // use CC channel 0 as the "top" register
  myTimer.setCompare(chann, period);

  //--------------------------------------------------------------
  // 3) Attach a callback to compare channel 0
  //    So whenever the timer hits the period (resets),
  //    it will trigger this callback.
  //--------------------------------------------------------------
  myTimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, myTimerCallback);

  // 4) Enable the timer (starts counting & firing interrupts)
  myTimer.enable(true);

  // The ESC must send the off speed to initialize before it can send a higher speed or else the EDF will error beep
  // So if you change the logic to start off at a higher speed, uncomment the line below and possibly the delay too
  // esc.write(OFF_SPEED);
  // delay(2000);
}

void loop() {
  switch (currentState) {
    case OFF:
      // if button A was pressed and it's been long enough since it was pressed last
      if ((digitalRead(A_BUTTON_PIN) == HIGH) && ((millis() - onOffTime) > MIN_ON_OFF_DURATION)) {
        onOffTime = millis();
        // switch the fan to the staged speed
        currentState = STAGED;
      }
      break;
    
    case STAGED:
      // if button A was pressed and it's been long enough since it was pressed last
      if ((digitalRead(A_BUTTON_PIN) == HIGH) && ((millis() - onOffTime) > MIN_ON_OFF_DURATION)) {
        onOffTime = millis();
        // switch the fan to the off state
        currentState = OFF;
      }
      // if button B was pressed, switch the fan to low speed (L)
      else if (digitalRead(B_BUTTON_PIN) == HIGH) {
        racingTime = millis();
        currentState = L;
      }
      // if button C was pressed, switch the fan to medium speed (M)
      else if (digitalRead(C_BUTTON_PIN) == HIGH) {
        racingTime = millis();
        currentState = M;
      }
      // if button D was pressed, switch the fan to high speed (H)
      else if (digitalRead(D_BUTTON_PIN) == HIGH) {
        racingTime = millis();
        currentState = H;
      }
      break;

    // If racing in low, medium, or high speed, stop automatically after the racing duration has elapsed.
    // You may want to alter this code to run the different speed for different lengths of time
    case L:
    case M:
    case H:
      if ((millis() - racingTime) > RACE_DURATION) {
        currentState = OFF;
      }
      break;
  }

  // Set the ESC speed depending on the current state
  switch(currentState) {
    case OFF:
      esc.write(OFF_SPEED);
      break;
    case STAGED:
      esc.write(STAGED_SPEED);
      break;
    case L:
      esc.write(LOW_SPEED);
      break;
    case M:
      esc.write(MED_SPEED);
      break;
    case H:
      esc.write(HIGH_SPEED);
      break;
  }

  // Wait 1ms before running the loop again
  delay(1);
}
