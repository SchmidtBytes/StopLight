#ifndef STOP_LIGHT_STATE_ENUM_H
#define STOP_LIGHT_STATE_ENUM_H

#include <stdint.h>
#include <map>
#include <SPIFFS.h>

// Enum for the stop light state
enum StopLightState : uint8_t {
    STATE_INIT      =   0, // code: i; Initialization animation
    STATE_OFF       =   1, // code: o; Turn off all of the LEDs
    STATE_BUILDING  =   2, // code: b; Flash the yellow LED, but dim the transitions
    STATE_SUCCESS   =   3, // code: s; Light up only the green LED
    STATE_UNSTABLE  =   4, // code: u; Light up only the yellow LED
    STATE_FAILED    =   5, // code: f; Light up only the red LED
    STATE_PREPARE   =   6, // code: p; Light up both yellow and red
    STATE_NIGHT     =   7, // code: n; Flash the yellow light, but with no dimming
    STATE_WAIT      =   8, // code: w; Flash the red and green LEDs at the same time
    STATE_ALTERNATE =   9, // code: a; Alternate between yellow and the other LEDs
    STATE_ROLLING   =  10, // code: r; Rolling animation from red to green
    STATE_CYCLE     =  98, // code: c; Run a normal stop light animation
    STATE_PARTY     =  99, // code: $; Flash the LEDs at random times and brightness levels

    STATE_UNKNOWN   = 255  // State used for invalid states.
};

// Map to convert the StopLightState into the command chars
const static std::map<StopLightState,char> STATE_CHAR_MAP = {
    {STATE_INIT,      'i'},
    {STATE_OFF,       'o'},
    {STATE_BUILDING,  'b'},
    {STATE_SUCCESS,   's'},
    {STATE_UNSTABLE,  'u'},
    {STATE_FAILED,    'f'},
    {STATE_PREPARE,   'p'},
    {STATE_NIGHT,     'n'},
    {STATE_WAIT,      'w'},
    {STATE_ALTERNATE, 'a'},
    {STATE_ROLLING,   'r'},
    {STATE_CYCLE,     'c'},
    {STATE_PARTY,     '$'},
    {STATE_UNKNOWN,   'X'}
};

// Map to convert the command chars into a StopLightState
const static std::map<char,StopLightState> CHAR_STATE_MAP = {
    {'i', STATE_INIT},
    {'o', STATE_OFF},
    {'b', STATE_BUILDING},
    {'s', STATE_SUCCESS},
    {'u', STATE_UNSTABLE},
    {'f', STATE_FAILED},
    {'p', STATE_PREPARE},
    {'n', STATE_NIGHT},
    {'w', STATE_WAIT},
    {'a', STATE_ALTERNATE},
    {'r', STATE_ROLLING},
    {'c', STATE_CYCLE},
    {'$', STATE_PARTY}
};

// Map to convert the StopLightState into a String
const static std::map<StopLightState,String> STATE_STRING_MAP = {
    {STATE_INIT,      "Init"},
    {STATE_OFF,       "OFF"},
    {STATE_BUILDING,  "Dimmed Mode"},
    {STATE_SUCCESS,   "Go"},
    {STATE_UNSTABLE,  "Hold"},
    {STATE_FAILED,    "Stop"},
    {STATE_PREPARE,   "Prepare"},
    {STATE_NIGHT,     "Night mode"},
    {STATE_WAIT,      "Wait"},
    {STATE_ALTERNATE, "Alternating"},
    {STATE_ROLLING,   "Rolling"},
    {STATE_CYCLE,     "Cycle"},
    {STATE_PARTY,     "Party"},
    {STATE_UNKNOWN,   "UNKNOWN_STATE"}
};

#endif // STOP_LIGHT_STATE_ENUM_H