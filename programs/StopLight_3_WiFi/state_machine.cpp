#include "HardwareSerial.h"
#include <ctime>
#include "state_enum.h"
#include <state_machine.h>
#include <Arduino.h>
#include "pins.h"

StopLightState active_state = StopLightState::STATE_INIT; // memory for the currently active state
bool sleep_active = true;               // Holds the current state if sleep mode is active
uint32_t sleep_timer = TURN_OFF_TIME;   // counter to enable sleep mode
unsigned long next_cycle = CYCLE_TIME;  // time when to start with the next cycle
int state_var = 0;                      // variable that can be used by the currently active state to save some information
unsigned long time_mili = 0;            // Current system time in miliseconds
                                        // This is mostly used to time the flash patterns for repeating states
uint32_t change_time_r = TURN_OFF_TIME; // Variable to save when to change the red LED; only used in some states
uint32_t change_time_y = TURN_OFF_TIME; // Variable to save when to change the yellow LED; only used in some states
uint32_t change_time_g = TURN_OFF_TIME; // Variable to save when to change the green LED; only used in some states

bool set_state(StopLightState requested_state)
{
  if(requested_state == STATE_UNKNOWN)
  { // Ignore unknown states
    return false;
  }
  // reset the sleep timer
  sleep_timer = TURN_OFF_TIME;
  if(requested_state != active_state)
  { // Ipdate and initialize the state if needed
    active_state = requested_state;
    state_var = 0;
    if(requested_state == STATE_PARTY)
    {
      uint32_t diff_time = TURN_OFF_TIME - sleep_timer;
      change_time_r = TURN_OFF_TIME + diff_time;
      change_time_y = TURN_OFF_TIME + diff_time;
      change_time_g = TURN_OFF_TIME + diff_time;
    }
  }
  return true;
}

void run_state_machine() {
  // State machine for the various states
  switch(active_state){
    case STATE_INIT:
      if (state_var < (500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, (255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME));
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 0);
      }
      else if(state_var < (1000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255);
        analogWrite(LED_YE, (255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME));
        analogWrite(LED_GN, 0);
      }
      else if(state_var < (1500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255);
        analogWrite(LED_YE, 255);
        analogWrite(LED_GN, (255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME));
      }
      else if(state_var < (3000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255);
        analogWrite(LED_YE, 255);
        analogWrite(LED_GN, 255);
      }
      else if(state_var < (3500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255 - ((255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
        analogWrite(LED_YE, 255);
        analogWrite(LED_GN, 255);
      }
      else if(state_var < (4000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 255 - ((255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
        analogWrite(LED_GN, 255);
      }
      else if(state_var < (4500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 255 - ((255 * (state_var % (500/*ms*/ / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
      }
      else {
        // Automatically switch to off state after finishing the initialization animation
        active_state = STATE_OFF;
        sleep_timer = 0;
      }
      // increase the state var to run through the animation
      state_var ++;
      break;

    case STATE_OFF:
      analogWrite(LED_RD, 0);
      analogWrite(LED_YE, 0);
      analogWrite(LED_GN, 0);
      break;

    case STATE_BUILDING:
      analogWrite(LED_RD, 0);
      if(state_var < (500/*ms*/ / CYCLE_TIME))
        analogWrite(LED_YE, (255 * state_var) / (500/*ms*/ / CYCLE_TIME));
      else if(state_var < (750/*ms*/ / CYCLE_TIME))
      {
        analogWrite(LED_YE, 255);
      }
      else if(state_var < (1250/*ms*/ / CYCLE_TIME))
        analogWrite(LED_YE, 255 - ((255 * (state_var - (750 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
      else
        analogWrite(LED_YE, 0);
      analogWrite(LED_GN, 0);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (1500/*ms*/ / CYCLE_TIME);
      break;

    case STATE_SUCCESS:
      analogWrite(LED_RD, 0);
      analogWrite(LED_YE, 0);
      analogWrite(LED_GN, 255);
      break;

    case STATE_UNSTABLE:
      analogWrite(LED_RD, 0);
      analogWrite(LED_YE, 255);
      analogWrite(LED_GN, 0);
      break;

    case STATE_FAILED:
      analogWrite(LED_RD, 255);
      analogWrite(LED_YE, 0);
      analogWrite(LED_GN, 0);
      break;

    case STATE_PREPARE:
      analogWrite(LED_RD, 255);
      analogWrite(LED_YE, 255);
      analogWrite(LED_GN, 0);
      break;

    case STATE_NIGHT:
      analogWrite(LED_RD, 0);
      analogWrite(LED_YE, state_var<(1000/*ms*/ / CYCLE_TIME)?255:0);
      analogWrite(LED_GN, 0);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (2000/*ms*/ / CYCLE_TIME);
      break;

    case STATE_WAIT:
      analogWrite(LED_RD, state_var<(500/*ms*/ / CYCLE_TIME)?255:0);
      analogWrite(LED_YE, 0);
      analogWrite(LED_GN, state_var<(500/*ms*/ / CYCLE_TIME)?255:0);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (1000/*ms*/ / CYCLE_TIME);
      break;

    case STATE_ALTERNATE:
      analogWrite(LED_RD, state_var<(1000/*ms*/ / CYCLE_TIME)?255:0);
      analogWrite(LED_YE, state_var<(1000/*ms*/ / CYCLE_TIME)?0:255);
      analogWrite(LED_GN, state_var<(1000/*ms*/ / CYCLE_TIME)?255:0);
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (2000/*ms*/ / CYCLE_TIME);
      break;

    case STATE_ROLLING:
      if (state_var < (500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, (255 * state_var) / (500/*ms*/ / CYCLE_TIME));
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 0);
      }
      else if (state_var < (1000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255 - ((255 * (state_var - (500 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
        analogWrite(LED_YE, (255 * (state_var - (500 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME));
        analogWrite(LED_GN, 0);
      }
      else if (state_var < (1500/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 255 - ((255 * (state_var - (1000 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
        analogWrite(LED_GN, (255 * (state_var - (1000 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME));
      }
      else if (state_var < (2000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 255 - ((255 * (state_var - (1500 / CYCLE_TIME))) / (500/*ms*/ / CYCLE_TIME)));
      }
      else {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 0);
      }
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (3000/*ms*/ / CYCLE_TIME);
      break;

    case STATE_CYCLE:
      if(state_var < (10000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 255);
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 0);
      }
      else if(state_var < (11000/*ms*/ / CYCLE_TIME))
        analogWrite(LED_YE, 255);
      else if(state_var < (21000/*ms*/ / CYCLE_TIME)) {
        analogWrite(LED_RD, 0);
        analogWrite(LED_YE, 0);
        analogWrite(LED_GN, 255);
      }
      else {
        analogWrite(LED_YE, 255);
        analogWrite(LED_GN, 0);
      }
      // increase the state_var to run through the animation
      state_var = (state_var + 1) % (23000/*ms*/ / CYCLE_TIME);
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
      active_state = STATE_INIT;
      break;
  }

  if(sleep_timer > 0)
    // count down the time when to turn the stop light off
    sleep_timer--;
  if(sleep_timer == 0) {
    if(sleep_active)
      // switch to state off after the sleep_timer ran out and sleep is active
      active_state = STATE_OFF;
    else
      // reset the sleep timer and the rgb change times
      change_time_r = change_time_y = change_time_g = sleep_timer = TURN_OFF_TIME;
  }

  // Wait some time before starting the next cycle
  do {
    time_mili = millis();
  } while(time_mili <= next_cycle || (next_cycle > ((unsigned long)100 * CYCLE_TIME) && (time_mili < ((unsigned long) 90 * CYCLE_TIME))));
  do {
    next_cycle += CYCLE_TIME;
  }while(next_cycle < time_mili && next_cycle >= CYCLE_TIME);
}