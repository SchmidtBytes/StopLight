
/*
  Build status stop light code
  V 1.0
*/

// LED Pin definitions
#define LED_RD 9
#define LED_YE 10
#define LED_GN 11

// Serial speed definition
#define SERIAL_SPEED 38400

// stop light state definitions
#define STATE_INIT       0 // code: i; Initialization animation
#define STATE_OFF        1 // code: o; Turn off all of the LEDs
#define STATE_BUILDING   2 // code: b; Flash the yellow LED, but dim the transitions
#define STATE_SUCCESS    3 // code: s; Light up only the green LED
#define STATE_UNSTABLE   4 // code: u; Light up only the yellow LED
#define STATE_FAILED     5 // code: f; Light up only the red LED
#define STATE_PREPARE    6 // code: p; Light up both yellow and red
#define STATE_NIGHT      7 // code: n; flash the yellow light, but with no dimming
#define STATE_CYCLE     98 // code: c; run a normal stop light animation
#define STATE_PARTY     99 // code: $; flash the LEDs at random times and brightness levels

// Timing
#define CYCLE_TIME          10 // [ms] Time to wait in between cycles
                               // WARNING: this will affect all the animated states.
                               //          You might need to adjust the timings to fit again
#define TURN_OFF_TIME   (3600/*[s/h]*/ * 1000/*[ms/s]*/ / CYCLE_TIME/*[ms/cycle]*/)
                        // Number of cycles before turning the stop light off
                        // (default: Turn off after 1 hour)

int incomingByte = 0;                   // for incoming serial data
int state = 0;                          // memory for the currently active state
uint32_t sleep_timer = TURN_OFF_TIME;   // counter to enable sleep mode
int state_var = 0;                      // variable that can be used by the currently active state to save some information
                                        // This is mostly used to time the flash patterns for repeating states
uint32_t change_time_r = TURN_OFF_TIME; // Variable to save when to change the red LED; only used in some states
uint32_t change_time_y = TURN_OFF_TIME; // Variable to save when to change the yellow LED; only used in some states
uint32_t change_time_g = TURN_OFF_TIME; // Variable to save when to change the green LED; only used in some states

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize the serial interface
  Serial.begin(SERIAL_SPEED);

  // setup the pins of the leds
  pinMode(LED_RD, OUTPUT);
  pinMode(LED_YE, OUTPUT);
  pinMode(LED_GN, OUTPUT);

  // read from an unconnected analog input to initialize the random function
  randomSeed(analogRead(0));
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available() > 0) {
    // read a byte from the serial interface
    incomingByte = Serial.read();
    // convert to lower case
    if (incomingByte >= 'A' && incomingByte <= 'Z')
        incomingByte += 'a' - 'A';

    bool byte_accepted = true;
    // Check which state was requested and set it up if necessary
    if(incomingByte == 'i' && state != STATE_INIT)
      state = STATE_INIT;
    else if(incomingByte == 'o' && state != STATE_OFF)
      state = STATE_OFF;
    else if(incomingByte == 'b' && state != STATE_BUILDING)
      state = STATE_BUILDING;
    else if(incomingByte == 's' && state != STATE_SUCCESS)
      state = STATE_SUCCESS;
    else if(incomingByte == 'u' && state != STATE_UNSTABLE)
      state = STATE_UNSTABLE;
    else if(incomingByte == 'f' && state != STATE_FAILED)
      state = STATE_FAILED;
    else if(incomingByte == 'p' && state != STATE_PREPARE)
      state = STATE_PREPARE;
    else if(incomingByte == 'n' && state != STATE_NIGHT)
      state = STATE_NIGHT;
    else if(incomingByte == 'c' && state != STATE_CYCLE)
      state = STATE_CYCLE;
    else if(incomingByte == '$' && state != STATE_PARTY)
    {
      state = STATE_PARTY;
      change_time_r = TURN_OFF_TIME;
      change_time_y = TURN_OFF_TIME;
      change_time_g = TURN_OFF_TIME;
    }
    else
      // detect if requested state is already active or state code was invalid
      byte_accepted = false;

    if(byte_accepted)
        // Reset the state var for the new state
        state_var=0;
    else if(state == STATE_PARTY)
    {
        uint32_t diff_time = TURN_OFF_TIME - sleep_timer;
        change_time_r += diff_time;
        change_time_y += diff_time;
        change_time_g += diff_time;
    }

    // reset the sleep timer
    sleep_timer = TURN_OFF_TIME;
  }

  // State machine for the various states
  switch(state){
    case STATE_INIT:
      if (state_var < 64) {
        analogWrite(LED_RD, state_var*4);
        digitalWrite(LED_YE, LOW);
        digitalWrite(LED_GN, LOW);
      }
      else if(state_var < 128) {
        digitalWrite(LED_RD, HIGH);
        analogWrite(LED_YE, (state_var-64)*4);
        digitalWrite(LED_GN, LOW);
      }
      else if(state_var < 192) {
        digitalWrite(LED_RD, HIGH);
        digitalWrite(LED_YE, HIGH);
        analogWrite(LED_GN, (state_var-128)*4);
      }
      else if(state_var < 384) {
        digitalWrite(LED_RD, HIGH);
        digitalWrite(LED_YE, HIGH);
        digitalWrite(LED_GN, HIGH);
      }
      else if(state_var < 448) {
        analogWrite(LED_RD, (447 - state_var)*4);
        digitalWrite(LED_YE, HIGH);
        digitalWrite(LED_GN, HIGH);
      }
      else if(state_var < 512) {
        digitalWrite(LED_RD, LOW);
        analogWrite(LED_YE, (511 - state_var)*4);
        digitalWrite(LED_GN, HIGH);
      }
      else if(state_var < 576) {
        digitalWrite(LED_RD, LOW);
        digitalWrite(LED_YE, LOW);
        analogWrite(LED_GN, (575 - state_var)*4);
      }
      else {
        // Automatically switch to off state after finishing the initialization animation
        state = STATE_OFF;
        sleep_timer = 0;
      }
      // increase the state var to run through the animation
      state_var ++;
      break;

    case STATE_OFF:
      digitalWrite(LED_RD, LOW);
      digitalWrite(LED_YE, LOW);
      digitalWrite(LED_GN, LOW);
      break;

    case STATE_BUILDING:
      digitalWrite(LED_RD, LOW);
      if(state_var < 64)
        analogWrite(LED_YE, state_var*4);
      else if(state_var < 96)
        digitalWrite(LED_YE, HIGH);
      else if(state_var < 160)
        analogWrite(LED_YE, (159-state_var)*4);
      else
        digitalWrite(LED_YE, LOW);
      digitalWrite(LED_GN, LOW);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % 192;
      break;

    case STATE_SUCCESS:
      digitalWrite(LED_RD, LOW);
      digitalWrite(LED_YE, LOW);
      digitalWrite(LED_GN, HIGH);
      break;

    case STATE_UNSTABLE:
      digitalWrite(LED_RD, LOW);
      digitalWrite(LED_YE, HIGH);
      digitalWrite(LED_GN, LOW);
      break;

    case STATE_FAILED:
      digitalWrite(LED_RD, HIGH);
      digitalWrite(LED_YE, LOW);
      digitalWrite(LED_GN, LOW);
      break;

    case STATE_PREPARE:
      digitalWrite(LED_RD, HIGH);
      digitalWrite(LED_YE, HIGH);
      digitalWrite(LED_GN, LOW);
      break;

    case STATE_NIGHT:
      digitalWrite(LED_RD, LOW);
      digitalWrite(LED_YE, state_var<128?HIGH:LOW);
      digitalWrite(LED_GN, LOW);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % 256;
      break;

    case STATE_CYCLE:
      if(state_var < 1000) {
        digitalWrite(LED_RD, HIGH);
        digitalWrite(LED_YE, LOW);
        digitalWrite(LED_GN, LOW);
      }
      else if(state_var < 1100)
        digitalWrite(LED_YE, HIGH);
      else if(state_var < 2100) {
        digitalWrite(LED_RD, LOW);
        digitalWrite(LED_YE, LOW);
        digitalWrite(LED_GN, HIGH);
      }
      else {
        digitalWrite(LED_YE, HIGH);
        digitalWrite(LED_GN, LOW);
      }
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % 2300;
      break;

    case STATE_PARTY:
      // randomize the brightness levels and change times for all LEDs independently
      if (change_time_r >= sleep_timer)
      {
        analogWrite(LED_RD, random(6)* 51);
        change_time_r = sleep_timer - random(25, 150);
      }
      if (change_time_y >= sleep_timer)
      {
        analogWrite(LED_YE, random(6)* 51);
        change_time_y = sleep_timer - random(25, 150);
      }
      if (change_time_g >= sleep_timer)
      {
        analogWrite(LED_GN, random(6)* 51);
        change_time_g = sleep_timer - random(25, 150);
      }
      break;

    default:
      // reinitialize when the current state is invalid
      state = STATE_INIT;
      break;
  }
  if(sleep_timer > 0)
    // count down the time when to turn the stop light off
    sleep_timer--;
  if(sleep_timer == 0)
    // switch to state off after the sleep_timer ran out
    state = STATE_OFF;

  // Wait some time before starting the next cycle
  delay(CYCLE_TIME);
}
