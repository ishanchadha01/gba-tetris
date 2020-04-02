#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  extern GBAState state;
  extern int speed;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial game state
  state = START;

  while (1) {
    currentButtons = BUTTONS;  // Load the current state of the buttons
    waitForVBlank();
    switch (state) {
      case START:
        reset();
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) start();
        break;
      case PLAY:
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) state = START;
        if (KEY_JUST_PRESSED(BUTTON_LEFT, currentButtons, previousButtons)) movePiece(-1, 0);
        if (KEY_JUST_PRESSED(BUTTON_RIGHT, currentButtons, previousButtons)) movePiece(1, 0);
        if (KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) movePiece(0, 1);
        if (KEY_JUST_PRESSED(BUTTON_L, currentButtons, previousButtons)) rotatePiece(1);
        if (KEY_JUST_PRESSED(BUTTON_R, currentButtons, previousButtons)) rotatePiece(0);
        if (vBlankCounter % (speed) == 0) {
          movePiece(0, 1);
        }
        break;
      case WIN:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) state = START;
        else if (KEY_DOWN(BUTTON_START, currentButtons)) state = START;
        break;
      case LOSE:
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) state = START;
        else if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) state = START;
        break;
    }
    previousButtons = currentButtons;  // Store the current state of the buttons
  }

  return 0;
}
