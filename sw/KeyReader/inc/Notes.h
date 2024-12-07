#ifndef NOTES_H
#define NOTES_H
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "stdint.h"
#include "stdbool.h"
#include "SensorIn.h"
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
#define NUMBER_OF_KEYS 24
#define START_THRESHOLD 0
#define PRESS_THRESHOLD 0
#define RELEASE_THRESHOLD 0

#define SENSOR_MAX 1200 //PLACEHOLDER
#define SENSOR_MIN 0 //PLACEHOLDER
#define NOTE_MIN_THRESH 1100 //PLACEHOLDER
#define NOTE_MAX_THRESH 100  //PLACEHOLDER

enum NotesIndexes {
    G_SHARP3 = 0, // G#3 / Ab3
    A3,           // A3
    A_SHARP3,     // A#3 / Bb3
    B3,           // B3

    C4,           // Middle C
    C_SHARP4,     // C#4 / Db4
    D4,           // D4
    D_SHARP4,     // D#4 / Eb4
    E4,           // E4
    F4,           // F4
    F_SHARP4,     // F#4 / Gb4
    G4,           // G4
    G_SHARP4,     // G#4 / Ab4
    A4,           // A4 (440 Hz)
    A_SHARP4,     // A#4 / Bb4
    B4,           // B4

    C3,           // Middle C
    C_SHARP3,     // C#4 / Db4
    D3,           // D4
    D_SHARP3,     // D#4 / Eb4
    E3,           // E4
    F3,           // F4
    F_SHARP3,     // F#4 / Gb4
    G3,           //
};


enum KeyState{
    idle,
    start,
    press
};

typedef struct {
    enum KeyState state;           // To hold the current state of the key (Idle, Pressing, etc.)
    uint32_t currentTime_start;    // To hold the start time of the current state
    uint32_t elapsedTime;          // To hold the elapsed time since the state started
    bool pressMessageSent;         // Flag to check if a "press" message has been sent
} Key;

//can be changed
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void Notes_Init(void);
#endif 

