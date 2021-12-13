// ControlPanel

#define FASTLED_INTERNAL
#include <FastLED.h>
#include <CMRI.h>
#include "Button.h"

#define LED_ON      true
#define LED_OFF     false
#define LED_PIN     5
#define BRIGHTNESS  32
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_LEDS    36
CRGB leds[NUM_LEDS];

CMRI cmri(0, 48, 48); // 48 inputs, 48 outputs configure in JMRI: CMRI connection with node 0, USIC_SUSIC, 24 bits per card, two input cards, two output cards

#define TURNOUT_UNKNOWN 0
#define TURNOUT_CLOSE  1
#define TURNOUT_THROW  2

#define NUM_BUTTONS               15
#define NUM_BUTTON_STATES          6
#define NUM_BUTTON_STATE_TURNOUTS  6
#define NUM_BUTTON_STATE_LEDS      6
#define BUTTON_RATE               50
#define BUTTON_46_ALT_MODE         1

typedef struct {
  byte led; // 1-based as numbered in JMRI
  bool on;
} LedState;

typedef struct {
  byte turnout; // 1-based as numbered in JMRI
  byte state;
} TurnoutState;

typedef struct {
  byte numTurnouts;
  TurnoutState turnouts[NUM_BUTTON_STATE_TURNOUTS];
  byte numLeds;
  LedState leds[NUM_BUTTON_STATE_LEDS];
} SwitchState;

typedef struct {
  Button *button;

  byte numStates;
  SwitchState states[NUM_BUTTON_STATES];
  short state;
  short lastState;
} SwitchButton;

SwitchButton buttons[NUM_BUTTONS] = {
  {
    new Button(48, BUTTON_RATE), 2,
    {
      {
        1, { { 3, TURNOUT_CLOSE } },
        2, { { 3, LED_OFF }, { 4, LED_ON } }
      },
      {
        1, { { 3, TURNOUT_THROW } },
        2, { { 3, LED_ON }, { 4, LED_OFF } }
      }
    }
  },
  {
    new Button(44, BUTTON_RATE), 2,
    {
      {
        1, { { 21, TURNOUT_CLOSE } },
        2, { { 1, LED_OFF }, { 2, LED_ON } }
      },
      {
        1, { { 21, TURNOUT_THROW } },
        2, { { 1, LED_ON }, { 2, LED_OFF } }
      }
    }
  },
  {
    new Button(46, BUTTON_RATE), 3,
    {
      {
        2, { { 9, TURNOUT_CLOSE }, { 10, TURNOUT_CLOSE } },
        3, { { 5, LED_OFF }, { 6, LED_ON }, { 7, LED_OFF } }
      },
      {
        2, { { 9, TURNOUT_CLOSE }, { 10, TURNOUT_THROW } },
        3, { { 5, LED_OFF }, { 6, LED_OFF }, { 7, LED_ON } }
      },
      {
        1, { { 9, TURNOUT_THROW } },
        3, { { 5, LED_ON }, { 6, LED_OFF }, { 7, LED_OFF } }
      }
    }
  },
  {
    new Button(50, BUTTON_RATE), 2,
    {
      {
        1, { { 11, TURNOUT_THROW } },
        1, { { 8, LED_OFF } }
      },
      { // this state causes button 46 to skip state 1
        1, { { 11, TURNOUT_CLOSE } },
        1, { { 8, LED_ON } }
      },
    }
  },
  {
    new Button(42, BUTTON_RATE), 3,
    {
      {
        2, { { 12, TURNOUT_CLOSE }, { 13, TURNOUT_CLOSE } },
        3, { { 10, LED_ON }, { 11, LED_OFF }, { 12, LED_OFF } }
      },
      {
        2, { { 12, TURNOUT_CLOSE }, { 13, TURNOUT_THROW } },
        3, { { 10, LED_OFF }, { 11, LED_ON }, { 12, LED_OFF } }
      },
      {
        1, { { 12, TURNOUT_THROW } },
        3, { { 10, LED_OFF }, { 11, LED_OFF }, { 12, LED_ON } }
      },
    }
  },
  {
    new Button(38, BUTTON_RATE), 2,
    {
      {
        1, { { 4, TURNOUT_CLOSE } },
        2, { { 13, LED_OFF }, { 14, LED_ON } }
      },
      {
        1, { { 4, TURNOUT_THROW } },
        2, { { 13, LED_ON }, { 14, LED_OFF } }
      }
    }
  },
  {
    new Button(40, BUTTON_RATE), 2,
    {
      {
        1, { { 5, TURNOUT_CLOSE } },
        2, { { 17, LED_ON }, { 18, LED_OFF } }
      },
      {
        1, { { 5, TURNOUT_THROW } },
        2, { { 17, LED_OFF }, { 18, LED_ON } }
      }
    }
  },
  {
    new Button(30, BUTTON_RATE), 2,
    {
      {
        1, { { 2, TURNOUT_CLOSE } },
        2, { { 35, LED_OFF }, { 36, LED_ON } }
      },
      {
        1, { { 2, TURNOUT_THROW } },
        2, { { 35, LED_ON }, { 36, LED_OFF } }
      }
    }
  },
  {
    new Button(26, BUTTON_RATE), 2,
    {
      {
        1, { { 8, TURNOUT_CLOSE } },
        2, { { 33, LED_OFF }, { 34, LED_ON } }
      },
      {
        1, { { 8, TURNOUT_THROW } },
        2, { { 33, LED_ON }, { 34, LED_OFF } }
      }
    }
  },
  {
    new Button(28, BUTTON_RATE), 2,
    {
      {
        1, { { 20, TURNOUT_CLOSE } },
        2, { { 31, LED_OFF }, { 32, LED_ON } }
      },
      {
        1, { { 20, TURNOUT_THROW } },
        2, { { 31, LED_ON }, { 32, LED_OFF } }
      }
    }
  },
  {
    new Button(32, BUTTON_RATE), 2,
    {
      {
        1, { { 14, TURNOUT_CLOSE } },
        2, { { 15, LED_ON }, { 16, LED_OFF } }
      },
      {
        1, { { 14, TURNOUT_THROW } },
        2, { { 15, LED_OFF }, { 16, LED_ON } }
      }
    }
  },
  {
    new Button(36, BUTTON_RATE), 6,
    {
      {
        1, { { 15, TURNOUT_CLOSE } },
        6, { { 20, LED_ON }, { 21, LED_OFF }, { 22, LED_OFF }, { 23, LED_OFF }, { 24, LED_OFF }, { 25, LED_OFF } }
      },
      {
        2, { { 15, TURNOUT_THROW }, { 16, TURNOUT_THROW } },
        6, { { 20, LED_OFF }, { 21, LED_ON }, { 22, LED_OFF }, { 23, LED_OFF }, { 24, LED_OFF }, { 25, LED_OFF } }
      },
      {
        3, { { 15, TURNOUT_THROW }, { 16, TURNOUT_CLOSE }, { 17, TURNOUT_THROW } },
        6, { { 20, LED_OFF }, { 21, LED_OFF }, { 22, LED_ON }, { 23, LED_OFF }, { 24, LED_OFF }, { 25, LED_OFF } }
      },
      {
        4, { { 15, TURNOUT_THROW }, { 16, TURNOUT_CLOSE }, { 17, TURNOUT_CLOSE }, { 18, TURNOUT_THROW } },
        6, { { 20, LED_OFF }, { 21, LED_OFF }, { 22, LED_OFF }, { 23, LED_ON }, { 24, LED_OFF }, { 25, LED_OFF } }
      },
      {
        5, { { 15, TURNOUT_THROW }, { 16, TURNOUT_CLOSE }, { 17, TURNOUT_CLOSE }, { 18, TURNOUT_CLOSE }, { 19, TURNOUT_CLOSE } },
        6, { { 20, LED_OFF }, { 21, LED_OFF }, { 22, LED_OFF }, { 23, LED_OFF }, { 24, LED_ON }, { 25, LED_OFF } }
      },
      {
        5, { { 15, TURNOUT_THROW }, { 16, TURNOUT_CLOSE }, { 17, TURNOUT_CLOSE }, { 18, TURNOUT_CLOSE }, { 19, TURNOUT_THROW } },
        6, { { 20, LED_OFF }, { 21, LED_OFF }, { 22, LED_OFF }, { 23, LED_OFF }, { 24, LED_OFF }, { 25, LED_ON } }
      },
    }
  },
  {
    new Button(24, BUTTON_RATE), 2,
    {
      {
        1, { { 7, TURNOUT_CLOSE } },
        2, { { 29, LED_ON }, { 30, LED_OFF } }
      },
      {
        1, { { 7, TURNOUT_THROW } },
        2, { { 29, LED_OFF }, { 30, LED_ON } }
      }
    }
  },
  {
    new Button(34, BUTTON_RATE)
  },
  {
    new Button(22, BUTTON_RATE), 2,
    {
      {
        2, { { 1, TURNOUT_CLOSE }, { 6, TURNOUT_CLOSE } },
        3, { { 26, LED_ON }, { 27, LED_OFF }, { 28, LED_ON } }
      },
      {
        2, { { 1, TURNOUT_THROW }, { 6, TURNOUT_THROW } },
        3, { { 26, LED_OFF }, { 27, LED_ON }, { 28, LED_OFF } }
      }
    }
  }
};

#define BLINK_RATE 100
bool blink = true;

#define NUM_TURNOUTS      22
#define TURNOUT_SYNC_RATE 1000
int turnoutSyncTime = 0;
byte turnoutInputs[NUM_TURNOUTS];
byte turnoutOutputs[NUM_TURNOUTS];

#define MODE_CMRI 0
#define MODE_STUB_CMRI 1
#define MODE_TEST_LIGHTS 2
#define MODE_SYNC_TURNOUTS 3
int mode = MODE_CMRI;

void setupSwitchButton(SwitchButton *button) {
  button->button->setup();
}

SwitchButton *getSwitchButtonWithPin(byte pin) {
  for (byte i = 0; i < NUM_BUTTONS; ++i) {
    if (buttons[i].button->pin() == pin) {
      return &buttons[i];
    }
  }

  return NULL;
}

SwitchState *getSwitchState(SwitchButton *button) {
  if (button->state >= 0) {
    return &button->states[button->state];
  } else {
    return NULL;
  }
}

SwitchState *setNextSwitchState(SwitchButton *button) {
  button->lastState = button->state;
  button->state = (button->state + 1) % button->numStates;
  return &button->states[button->state];
}

void writeTurnouts() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    SwitchButton *button = &buttons[i];
    writeTurnoutsForSwitchButton(button);
  }
}

void writeTurnoutsForSwitchButton(SwitchButton *button) {
  SwitchState *state = getSwitchState(button);
  if (state != NULL) {
    for (byte i = 0; i < state->numTurnouts; ++i) {
      TurnoutState *turnoutState = &state->turnouts[i];

      turnoutOutputs[turnoutState->turnout - 1] = turnoutState->state;
    }
  }
}

void readTurnouts() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    SwitchButton *button = &buttons[i];
    readTurnoutsForSwitchButton(button);
  }
}

void readTurnoutsForSwitchButton(SwitchButton *button) {
  if (button->state == button->lastState) {
    short bestMatchedState = -1;
    for(byte i = 0; i < button->numStates; i++) {
      if (button->button->pin() == 46 && i == 1 && getSwitchButtonWithPin(50)->state == 1) {
        continue;
      }

      SwitchState *state = &button->states[i];
      if (turnoutStateMatches(state) && (bestMatchedState == -1 || state->numTurnouts > button->states[bestMatchedState].numTurnouts)) {
        bestMatchedState = i;
      }
    }
    button->lastState = bestMatchedState;
    button->state = bestMatchedState;
  } else {
    SwitchState *state = getSwitchState(button);
    if (state != NULL) {
      if (turnoutStateMatches(state)) {
        button->lastState = button->state;
      }
    }
  }
}

void writeLeds() {
  setColorForAllLeds(CRGB::Black);
  EVERY_N_MILLIS(BLINK_RATE) {
    blink = !blink;
  }
  if (mode == MODE_CMRI) {
    for(byte i = 0; i < NUM_BUTTONS; ++i) {
      SwitchButton *button = &buttons[i];
      writeLedsForSwitchButton(button);
    }
  }
  if (mode == MODE_TEST_LIGHTS) {
    if (blink) {
      setColorForAllLeds(CRGB::Red);
    }
  }
  FastLED.show();
}

void writeLedsForSwitchButton(SwitchButton *button) {
  SwitchState *state = getSwitchState(button);
  if (state != NULL) {
    for (byte i = 0; i < state->numLeds; ++i) {
      LedState *ledState = &state->leds[i];

      if (button->state == button->lastState) {
        leds[ledState->led - 1] = ledState->on ? CRGB::Green : CRGB::Black;
      } else {
        leds[ledState->led - 1] = ledState->on ? CRGB::Yellow : CRGB::Black;
      }
    }
  }
}

bool isTurnoutState(byte turnout, byte state) {
  return turnoutInputs[turnout - 1] == state;
}

bool turnoutStateMatches(SwitchState *state) {
  for (byte i = 0; i < state->numTurnouts; ++i) {
    TurnoutState *turnoutState = &state->turnouts[i];
    if (!isTurnoutState(turnoutState->turnout, turnoutState->state)) {
      return false;
    }
  }

  return true;
}

void processSwitchButtonPresses() {
  // int button53Down = !digitalRead(53);
  // int button51Down = !digitalRead(51);
  // int button49Down = !digitalRead(49);
  // int button47Down = !digitalRead(47);
  // for(byte i = 0; i < NUM_BUTTONS; ++i) {
  //   readSwitchButtonPin(&buttons[i]);
  // }

  // if (button53Down && mode == MODE_CMRI) {
  //   mode = MODE_TEST_LIGHTS;
  // }
  // if (!button53Down && mode == MODE_TEST_LIGHTS) {
  //   mode = MODE_CMRI;
  // }
  // if (button47Down && mode == MODE_CMRI) {
  //   mode = MODE_SYNC_TURNOUTS;
  // }

  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    SwitchButton *button = &buttons[i];
    if (button->button->pressed()) {
      Serial.print("button pressed: ");
      Serial.println(button->button->pin());
      setNextSwitchState(button);

      if (button->button->pin() == 36) {
        if (button->state >= 3) {
          button->state = 0;
        }
      }

      if (button->button->pin() == 34) {
        SwitchButton *peerSwitchButton = getSwitchButtonWithPin(36);
        setNextSwitchState(peerSwitchButton);
        if (peerSwitchButton->state < 3) {
          peerSwitchButton->state = 3;
        }
      }

      if (button->button->pin() == 46) {
        SwitchButton *peerSwitchButton = getSwitchButtonWithPin(50);
        if (peerSwitchButton->state == BUTTON_46_ALT_MODE) {
          if (button->state == 1) {
            setNextSwitchState(button);
          }
        }
      }

      if (button->button->pin() == 50) {
        if (button->state == BUTTON_46_ALT_MODE) {
          SwitchButton *peerSwitchButton = getSwitchButtonWithPin(46);
          if (peerSwitchButton->state == 1) {
            setNextSwitchState(peerSwitchButton);
          }
        }
      }
    }
  }
}

void setupSwitchButtons() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    setupSwitchButton(&buttons[i]);
  }
}

void setColorForAllLeds(CRGB color) {
  for(byte i = 0; i < NUM_LEDS; ++i) {
    leds[i] = color;
  }
}

void syncTurnouts() {
  if (mode == MODE_STUB_CMRI) {
    EVERY_N_MILLIS(TURNOUT_SYNC_RATE) {
      for (byte i = 0; i < NUM_TURNOUTS; ++i) {
        turnoutInputs[i] = turnoutOutputs[i];
      }
    }
  } else {

    // if (mode == MODE_SYNC_TURNOUTS) {
    //   for (byte i = 0; i < NUM_TURNOUTS; ++i) {
    //     byte cmriOutput = i * 2;
    //     if (turnoutOutputs[i] == TURNOUT_CLOSE) {
    //       cmri.set_bit(cmriOutput, false);
    //       cmri.set_bit(cmriOutput + 1, true);
    //     } else if (turnoutOutputs[i] == TURNOUT_THROW) {
    //       cmri.set_bit(cmriOutput, true);
    //       cmri.set_bit(cmriOutput + 1, false);
    //     }
    //   }
    //   cmri.process();
    //   delay(100);
    //   mode = MODE_CMRI;
    // }

    cmri.process();

    for (byte i = 0; i < NUM_TURNOUTS; ++i) {
      byte cmriOutput = i * 2;
      if (turnoutOutputs[i] == TURNOUT_CLOSE) {
        cmri.set_bit(cmriOutput, true);
        cmri.set_bit(cmriOutput + 1, false);
      } else if (turnoutOutputs[i] == TURNOUT_THROW) {
        cmri.set_bit(cmriOutput, false);
        cmri.set_bit(cmriOutput + 1, true);
      } else {
        cmri.set_bit(cmriOutput, false);
        cmri.set_bit(cmriOutput + 1, false);
      }
//      turnoutOutputs[i] = TURNOUT_UNKNOWN;
    }

    for (byte i = 0; i < NUM_TURNOUTS; ++i) {
      byte cmriInput = i * 2;
      if (cmri.get_bit(cmriInput) && !cmri.get_bit(cmriInput + 1)) {
        turnoutInputs[i] = TURNOUT_CLOSE;
      } else if (!cmri.get_bit(cmriInput) && cmri.get_bit(cmriInput + 1)) {
        turnoutInputs[i] = TURNOUT_THROW;
      } else {
        turnoutInputs[i] = TURNOUT_UNKNOWN;
      }
    }
  }
}

void setup() {
  delay(2000); // power-up safety delay

  Serial.begin(9600, SERIAL_8N2); // SERIAL_8N2 to match what JMRI expects CMRI hardware to use
  Serial.println("hello");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  setColorForAllLeds(CRGB::Gray);
  FastLED.show();
  delay(500);

  setupSwitchButtons();

  // pinMode(47, INPUT_PULLUP);
  // pinMode(49, INPUT_PULLUP);
  // pinMode(51, INPUT_PULLUP);
  // pinMode(53, INPUT_PULLUP);
  writeTurnouts();
}

void loop() {
  processSwitchButtonPresses();

  writeTurnouts();
  syncTurnouts();
  readTurnouts();

  writeLeds();
}
