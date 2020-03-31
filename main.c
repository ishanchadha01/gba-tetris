#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  extern GBAState state;

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
        if (KEY_DOWN(BUTTON_START, BUTTONS)) state = PLAY;
        break;
      case PLAY:
        if (KEY_DOWN(BUTTON_SELECT, BUTTONS)) state = START;
        else if (KEY_DOWN(BUTTON_LEFT, BUTTONS)) movePiece(-1, 0);
        else if (KEY_DOWN(BUTTON_DOWN, BUTTONS)) movePiece(1, 0);
        else if (KEY_DOWN(BUTTON_DOWN, BUTTONS)) movePiece(0, 1);
        else if (KEY_DOWN(BUTTON_L, BUTTONS)) rotatePiece(1);
        else if (KEY_DOWN(BUTTON_R, BUTTONS)) rotatePiece(0);
        break;
      case WIN:
        if (KEY_DOWN(BUTTON_SELECT, BUTTONS)) state = START;
        else if (KEY_DOWN(BUTTON_START, BUTTONS)) state = START;
        break;
      case LOSE:
        if (KEY_DOWN(BUTTON_SELECT, BUTTONS)) state = START;
        else if (KEY_DOWN(BUTTON_START, BUTTONS)) state = START;
        break;
    }

    previousButtons = currentButtons;  // Store the current state of the buttons
  }

  UNUSED(previousButtons);  // You can remove this once previousButtons is used

  return 0;
}
