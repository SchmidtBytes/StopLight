#ifndef STOP_LIGHT_STATE_MACHINE_H
#define STOP_LIGHT_STATE_MACHINE_H

#include <stdint.h>
#include "state_enum.h"

// Timing
#define CYCLE_TIME      10 // [ms] Time to wait in between cycles
#define TURN_OFF_TIME   ((uint32_t) 3600/*[s/h]*/ * 1000/*[ms/s]*/ / CYCLE_TIME/*[ms/cycle]*/)
                        // Number of cycles before turning the stop light off
                        // (default: Turn off after 1 hour)

extern StopLightState active_state; // memory for the currently active state
extern bool sleep_active;           // Holds the current state if sleep mode is active
extern uint32_t sleep_timer;        // counter to enable sleep mode

// Set the state of the stop light
bool set_state(StopLightState requested_state);
// run the stop light state machine
void run_state_machine();

#endif // STOP_LIGHT_STATE_MACHINE