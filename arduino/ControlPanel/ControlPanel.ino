// ControlPanel

#define FASTLED_INTERNAL
#include <FastLED.h>
#include <CMRI.h>
#include "Button.h"
#include "DCCSensor.h"

#define LED_ON      true
#define LED_OFF     false
#define LED_PIN     5
#define BRIGHTNESS  32
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MIN_BRIGHTNESS 8
#define MAX_BRIGHTNESS 64
#define NUM_LEDS    36
CRGB leds[NUM_LEDS];

CMRI cmri(0, 48, 48); // 48 inputs, 48 outputs configure in JMRI: CMRI connection with node 0, USIC_SUSIC, 24 bits per card, two input cards, two output cards

#define TURNOUT_UNKNOWN 0
#define TURNOUT_CLOSE  1
#define TURNOUT_THROW  2

#define NUM_ROUTE_CONTROLS               15
#define NUM_ROUTE_CONTROL_STATES          6
#define NUM_ROUTE_CONTROL_STATE_TURNOUTS  6
#define NUM_ROUTE_CONTROL_STATE_LEDS      6
#define BUTTON_DEBOUNCE_MS               50
#define BUTTON_46_ALT_MODE                1

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
  TurnoutState turnouts[NUM_ROUTE_CONTROL_STATE_TURNOUTS];
  byte numLeds;
  LedState leds[NUM_ROUTE_CONTROL_STATE_LEDS];
} RouteControlState;

typedef struct {
  Button *button;

  byte numStates;
  RouteControlState states[NUM_ROUTE_CONTROL_STATES];
  short state;
  short lastState;
} RouteControl;

RouteControl routeControls[NUM_ROUTE_CONTROLS] = {
  {
    new Button(48, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(44, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(46, BUTTON_DEBOUNCE_MS), 3,
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
    new Button(50, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(42, BUTTON_DEBOUNCE_MS), 3,
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
    new Button(38, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(40, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(30, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(26, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(28, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(32, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(36, BUTTON_DEBOUNCE_MS), 6,
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
    new Button(24, BUTTON_DEBOUNCE_MS), 2,
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
    new Button(34, BUTTON_DEBOUNCE_MS)
  },
  {
    new Button(22, BUTTON_DEBOUNCE_MS), 2,
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

Button testLEDsButton(53, BUTTON_DEBOUNCE_MS);
Button syncTurnoutsButton(47, BUTTON_DEBOUNCE_MS);
DCCSensor dccSensor(A0, 500);

#define NUM_TURNOUTS      22
byte turnoutInputs[NUM_TURNOUTS];
byte turnoutOutputs[NUM_TURNOUTS];

void setupRouteControl(RouteControl *button) {
  button->button->setup();
}

RouteControl *getRouteControlWithPin(byte pin) {
  for (byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    if (routeControls[i].button->pin() == pin) {
      return &routeControls[i];
    }
  }

  return NULL;
}

RouteControlState *getRouteControlState(RouteControl *button) {
  if (button->state >= 0) {
    return &button->states[button->state];
  } else {
    return NULL;
  }
}

RouteControlState *setNextRouteControlState(RouteControl *button) {
  button->lastState = button->state;
  button->state = (button->state + 1) % button->numStates;
  return &button->states[button->state];
}

void writeTurnoutsForRouteControls() {
  for(byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    RouteControl *button = &routeControls[i];
    writeTurnoutsForRouteControl(button);
  }
}

void writeTurnoutsForRouteControl(RouteControl *button) {
  RouteControlState *state = getRouteControlState(button);
  if (state != NULL) {
    for (byte i = 0; i < state->numTurnouts; ++i) {
      TurnoutState *turnoutState = &state->turnouts[i];

      turnoutOutputs[turnoutState->turnout - 1] = turnoutState->state;
    }
  }
}

void readTurnouts() {
  for(byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    RouteControl *button = &routeControls[i];
    readTurnoutsForRouteControl(button);
  }
}

void readTurnoutsForRouteControl(RouteControl *button) {
  if (button->state == button->lastState) {
    short bestMatchedState = -1;
    for(byte i = 0; i < button->numStates; i++) {
      if (button->button->pin() == 46 && i == 1 && getRouteControlWithPin(50)->state == 1) {
        continue;
      }

      RouteControlState *state = &button->states[i];
      if (turnoutStateMatches(state) && (bestMatchedState == -1 || state->numTurnouts > button->states[bestMatchedState].numTurnouts)) {
        bestMatchedState = i;
      }
    }
    button->lastState = bestMatchedState;
    button->state = bestMatchedState;
  } else {
    RouteControlState *state = getRouteControlState(button);
    if (state != NULL) {
      if (turnoutStateMatches(state)) {
        button->lastState = button->state;
      }
    }
  }
}

#define BREATH_RATE 5
int breathHue = 0;
int breathDivisor = 30;

void setLedsToBreath() {
  EVERY_N_MILLIS(BREATH_RATE) {
    float breath = (exp(sin(millis() / 5000.0 * PI)) - 0.36787944) * 108.0;
    breath = map(breath, 0, 255, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    FastLED.setBrightness(breath);
    fill_rainbow(leds, NUM_LEDS, breathHue++ / breathDivisor);
    if (breathHue == (255 * breathDivisor)) {
      breathHue = 0;
    }
  }
}

void setLedsForRouteControls() {
  for(byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    RouteControl *button = &routeControls[i];
    setLedsForRouteControl(button);
  }
}

void writeLeds() {
  setLedsToColor(CRGB::Black);

  if (testLEDsButton.read() == Button::PRESSED) {
    setLedsToColor(CRGB::Red);
  } else if (dccSensor.read() == DCCSensor::STOPPED) {
    setLedsToBreath();
  } else {
    setLedsForRouteControls();
  }

  FastLED.show();
}

void setLedsForRouteControl(RouteControl *button) {
  RouteControlState *state = getRouteControlState(button);
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

bool turnoutStateMatches(RouteControlState *state) {
  for (byte i = 0; i < state->numTurnouts; ++i) {
    TurnoutState *turnoutState = &state->turnouts[i];
    if (!isTurnoutState(turnoutState->turnout, turnoutState->state)) {
      return false;
    }
  }

  return true;
}

void processRouteControls() {
  for(byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    RouteControl *button = &routeControls[i];
    if (button->button->pressed()) {
      Serial.print("button pressed: ");
      Serial.println(button->button->pin());
      setNextRouteControlState(button);

      if (button->button->pin() == 36) {
        if (button->state >= 3) {
          button->state = 0;
        }
      }

      if (button->button->pin() == 34) {
        RouteControl *peerRouteControl = getRouteControlWithPin(36);
        setNextRouteControlState(peerRouteControl);
        if (peerRouteControl->state < 3) {
          peerRouteControl->state = 3;
        }
      }

      if (button->button->pin() == 46) {
        RouteControl *peerRouteControl = getRouteControlWithPin(50);
        if (peerRouteControl->state == BUTTON_46_ALT_MODE) {
          if (button->state == 1) {
            setNextRouteControlState(button);
          }
        }
      }

      if (button->button->pin() == 50) {
        if (button->state == BUTTON_46_ALT_MODE) {
          RouteControl *peerRouteControl = getRouteControlWithPin(46);
          if (peerRouteControl->state == 1) {
            setNextRouteControlState(peerRouteControl);
          }
        }
      }
    }
  }
}

void setupRouteControls() {
  for(byte i = 0; i < NUM_ROUTE_CONTROLS; ++i) {
    setupRouteControl(&routeControls[i]);
  }
}

void setLedsToColor(CRGB color) {
  for(byte i = 0; i < NUM_LEDS; ++i) {
    leds[i] = color;
  }
}

void syncTurnouts() {
  if (syncTurnoutsButton.pressed() || dccSensor.started()) {
    for (byte i = 0; i < NUM_TURNOUTS; ++i) {
      byte cmriOutput = i * 2;
      if (turnoutOutputs[i] == TURNOUT_CLOSE) {
        cmri.set_bit(cmriOutput, false);
        cmri.set_bit(cmriOutput + 1, true);
      } else if (turnoutOutputs[i] == TURNOUT_THROW) {
        cmri.set_bit(cmriOutput, true);
        cmri.set_bit(cmriOutput + 1, false);
      }
    }

    delay(25);
    cmri.process();
    delay(25);
  }

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
  }

  cmri.process();

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

void setup() {
  delay(2000); // power-up safety delay

  Serial.begin(9600, SERIAL_8N2); // SERIAL_8N2 to match what JMRI expects CMRI hardware to use
  Serial.println("hello");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  setLedsToColor(CRGB::Gray);
  FastLED.show();
  delay(500);

  setupRouteControls();

  testLEDsButton.setup();
  syncTurnoutsButton.setup();

  writeTurnoutsForRouteControls();
}

void loop() {
  if (dccSensor.stopped()) {
    breathHue = 0;
  }

  if (dccSensor.read() == DCCSensor::STARTED) {
    processRouteControls();

    writeTurnoutsForRouteControls();
    syncTurnouts();
    readTurnouts();
  }

  writeLeds();
}
