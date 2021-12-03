#define FASTLED_INTERNAL
#include <FastLED.h>
#include <CMRI.h>

#define STUB_CMRI   true
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
#define BUTTON_RATE               500
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
} ButtonState;

typedef struct {
  byte pin; // on the arduino
  byte numStates;
  ButtonState states[NUM_BUTTON_STATES];

  bool isDown;
  bool wasDown;

  short state;
  short lastState;
} Button;

Button buttons[NUM_BUTTONS] = {
  {
    48, 2,
    {
      {
        1, { { 3, TURNOUT_CLOSE } },
        2, { { 3, LED_ON }, { 4, LED_OFF } }
      },
      {
        1, { { 3, TURNOUT_THROW } },
        2, { { 3, LED_OFF }, { 4, LED_ON } }
      }
    }
  },
  {
    44, 2,
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
    46, 3,
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
    50, 2,
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
    42, 3,
    {
      {
        2, { { 12, TURNOUT_THROW }, { 13, TURNOUT_CLOSE } },
        3, { { 10, LED_OFF }, { 11, LED_ON }, { 12, LED_OFF } }
      },
      {
        2, { { 12, TURNOUT_THROW }, { 13, TURNOUT_THROW } },
        3, { { 10, LED_OFF }, { 11, LED_OFF }, { 12, LED_ON } }
      },
      {
        1, { { 12, TURNOUT_CLOSE } },
        3, { { 10, LED_ON }, { 11, LED_OFF }, { 12, LED_OFF } }
      },
    }
  },
  {
    38, 2,
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
    40, 2,
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
    30, 2,
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
    26, 2,
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
    28, 2,
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
    32, 2,
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
    36, 6,
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
    24, 2,
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
    34
  },
  {
    22, 2,
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

void setupButton(Button *button) {
  pinMode(button->pin, INPUT_PULLUP);
}

void readButtonPin(Button *button) {
  button->wasDown = button->isDown;
  button->isDown = !digitalRead(button->pin);
}

bool isButtonReleased(Button *button) {
  return !button->isDown && button->wasDown;
}

bool isButtonUp(Button *button) {
  return !button->isDown;
}

bool isButtonPressed(Button *button) {
  return button->isDown && !button->wasDown;
}

bool isButtonDown(Button *button) {
  return button->isDown;
}

Button *getButtonWithPin(byte pin) {
  for (byte i = 0; i < NUM_BUTTONS; ++i) {
    if (buttons[i].pin == pin) {
      return &buttons[i];
    }
  }

  return NULL;
}

ButtonState *getButtonState(Button *button) {
  if (button->state >= 0) {
    return &button->states[button->state];
  } else {
    return NULL;
  }
}

ButtonState *setButtonState(Button *button, byte state = 0) {
  button->lastState = button->state;
  button->state = state;
  return &button->states[button->state];
}

ButtonState *setNextButtonState(Button *button) {
  button->lastState = button->state;
  button->state = (button->state + 1) % button->numStates;
  return &button->states[button->state];
}

void writeTurnouts() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    Button *button = &buttons[i];
    writeTurnoutsForButton(button);
  }
}

void writeTurnoutsForButton(Button *button) {
  ButtonState *state = getButtonState(button);
  if (state != NULL) {
    for (byte i = 0; i < state->numTurnouts; ++i) {
      TurnoutState *turnoutState = &state->turnouts[i];

      turnoutOutputs[turnoutState->turnout - 1] = turnoutState->state;
    }
  }
}

void readTurnouts() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    Button *button = &buttons[i];
    readTurnoutsForButton(button);
  }
}

void readTurnoutsForButton(Button *button) {
  if (button->state == button->lastState) {
    short bestMatchedState = -1;
    for(byte i = 0; i < button->numStates; i++) {
      if (button->pin == 46 && i == 1 && getButtonWithPin(50)->state == 1) {
        continue;
      }
      
      ButtonState *state = &button->states[i];
      if (turnoutStateMatches(state) && (bestMatchedState == -1 || state->numTurnouts > button->states[bestMatchedState].numTurnouts)) {
        bestMatchedState = i;
      }
    }
    button->lastState = bestMatchedState;
    button->state = bestMatchedState;
  } else {
    ButtonState *state = getButtonState(button);
    if (state != NULL) {
      if (turnoutStateMatches(state)) {
        button->lastState = button->state;
      }
    }
  }
}

void writeLeds() {
  EVERY_N_MILLIS(BLINK_RATE) {
    blink = !blink;
  }

  setColorForAllLeds(CRGB::Black);
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    Button *button = &buttons[i];
    writeLedsForButton(button);
  }
  FastLED.show();
}

void writeLedsForButton(Button *button) {
  ButtonState *state = getButtonState(button);
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

bool turnoutStateMatches(ButtonState *state) {
  for (byte i = 0; i < state->numTurnouts; ++i) {
    TurnoutState *turnoutState = &state->turnouts[i];
    if (!isTurnoutState(turnoutState->turnout, turnoutState->state)) {
      return false;
    }
  }

  return true;
}

void processButtonPresses() {
  EVERY_N_MILLIS(BUTTON_RATE) {
    Serial.print(digitalRead(53));
    Serial.print(digitalRead(51));
    Serial.print(digitalRead(49));
    Serial.print(digitalRead(47));
    Serial.println();
    for(byte i = 0; i < NUM_BUTTONS; ++i) {
      readButtonPin(&buttons[i]);
    }
    for(byte i = 0; i < NUM_BUTTONS; ++i) {
      Button *button = &buttons[i];
      if (isButtonPressed(button)) {
        Serial.print("button pressed: ");
        Serial.println(button->pin);
        setNextButtonState(button);

        if (button->pin == 36) {
          if (button->state >= 3) {
            button->state = 0;
          }
        }

        if (button->pin == 34) {
          Button *peerButton = getButtonWithPin(36);
          setNextButtonState(peerButton);
          if (peerButton->state < 3) {
            peerButton->state = 3;
          }
        }

        if (button->pin == 46) {
          Button *peerButton = getButtonWithPin(50);
          if (peerButton->state == BUTTON_46_ALT_MODE) {
            if (button->state == 1) {
              setNextButtonState(button);
            }
          }
        }

        if (button->pin == 50) {
          if (button->state == BUTTON_46_ALT_MODE) {
            Button *peerButton = getButtonWithPin(46);
            if (peerButton->state == 1) {
              setNextButtonState(peerButton);
            }
          }
        }
      }
    }
  }
}

void setupButtons() {
  for(byte i = 0; i < NUM_BUTTONS; ++i) {
    setupButton(&buttons[i]);
  }
}

void setColorForAllLeds(CRGB color) {
  for(byte i = 0; i < NUM_LEDS; ++i) {
    leds[i] = color;
  }
}

void syncTurnouts() {
  if (STUB_CMRI) {
    EVERY_N_MILLIS(TURNOUT_SYNC_RATE) {
      for (byte i = 0; i < NUM_TURNOUTS; ++i) {
        turnoutInputs[i] = turnoutOutputs[i];
      }
    }
  } else {
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

  setupButtons();
  pinMode(47, INPUT_PULLUP);
  pinMode(49, INPUT_PULLUP);
  pinMode(51, INPUT_PULLUP);
  pinMode(53, INPUT_PULLUP);
  writeTurnouts();
}

void loop() {
  processButtonPresses();

  writeTurnouts();
  syncTurnouts();
  readTurnouts();

  writeLeds();
}
