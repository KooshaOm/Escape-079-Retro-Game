/*=======================================================================*/
/*Base Address of Input/Output Registers*/
/*=======================================================================*/
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define PS2_BASE 0xFF200100
#define LED_BASE 0xFF200000
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define TIMER_BASE 0xFF202000
#define PROJECT_4_HEIGHT 240
#define PROJECT_4_WIDTH 320
#define PLAYER_SIZE 3
#define PICKUP_SIZE 4
#define NUM_PICKUPS 8
#define playerFacingUp 1
#define playerFacingDown -1
#define playerFacingRight 2
#define playerFacingLeft -2
#define COMPUTER_WIN -1
#define PLAYER_WIN 1

/*=======================================================================*/
/*Include Statements*/
/*=======================================================================*/
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*=======================================================================*/
/*Global Variables*/
/*=======================================================================*/
typedef struct Gate {
  int startX;
  int startY;
  int endX;
  int endY;
} Gate;
struct audio_t {
  volatile unsigned int control;
  volatile unsigned char rarc;
  volatile unsigned char ralc;
  volatile unsigned char warc;
  volatile unsigned char walc;
  volatile unsigned int ldata;
  volatile unsigned int rdata;
};
struct audio_playback {
  int samples_n;
  int duration;
  bool enabled;
  int *samples;
};

/*=======================================================================*/
/*Sound Arrays Pasted Here*/
/*=======================================================================*/

struct audio_t *const audiop = ((struct audio_t *)0xff203040);
volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512];     // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

// each index corresponds to the level number and each element value represents
// the number of of gates in that level
int numGatesInLevel[3] = {21, 0, 0};
int xPrevArray[2];
int yPrevArray[2];
int orientationPrevArray[2];
int gatePrevPos[2][4] = {{207, 181, 207, 194}, {207, 181, 207, 194}};
int pickupArray[NUM_PICKUPS][2] = {{30, 130}, {300, 225}, {290, 160},
                                   {290, 82}, {30, 195},  {25, 80},
                                   {155, 85}, {130, 205}};
Gate currGate;

Gate levelOneGateArray[21] = {
    (Gate){.startX = 76, .startY = 75, .endX = 90, .endY = 75},
    (Gate){.startX = 120, .startY = 29, .endX = 120, .endY = 40},
    (Gate){.startX = 76, .startY = 94, .endX = 90, .endY = 94},
    (Gate){.startX = 149, .startY = 64, .endX = 167, .endY = 64},
    (Gate){.startX = 117, .startY = 76, .endX = 117, .endY = 93},
    (Gate){.startX = 124, .startY = 103, .endX = 141, .endY = 103},
    (Gate){.startX = 178, .startY = 103, .endX = 193, .endY = 103},
    (Gate){.startX = 201, .startY = 77, .endX = 201, .endY = 90},
    (Gate){.startX = 228, .startY = 76, .endX = 244, .endY = 76},
    (Gate){.startX = 228, .startY = 125, .endX = 244, .endY = 125},
    (Gate){.startX = 123, .startY = 125, .endX = 123, .endY = 137},
    (Gate){.startX = 245, .startY = 77, .endX = 245, .endY = 90},
    (Gate){.startX = 25, .startY = 148, .endX = 42, .endY = 148},
    (Gate){.startX = 25, .startY = 180, .endX = 42, .endY = 180},
    (Gate){.startX = 75, .startY = 160, .endX = 75, .endY = 170},
    (Gate){.startX = 75, .startY = 197, .endX = 75, .endY = 207},
    (Gate){.startX = 289, .startY = 22, .endX = 289, .endY = 30},
    (Gate){.startX = 289, .startY = 40, .endX = 289, .endY = 46},
    (Gate){.startX = 245, .startY = 155, .endX = 245, .endY = 169},
    (Gate){.startX = 245, .startY = 213, .endX = 245, .endY = 236},
    (Gate){.startX = 124, .startY = 171, .endX = 141, .endY = 171}};

// make an array that hold pointers to Gate object so that we can access each
// level's gates from an array
Gate *gates[3] = {levelOneGateArray, NULL, NULL};
Gate *currLevelGatesArray;
struct audio_playback decontOne = {94877, 24, false, decontOneSamples};
struct audio_playback blackoutSound = {16827, 4, false, blackoutSoundSamples};
struct audio_playback doorSwitch = {4398, 1, false, doorSwitchSamples};
struct audio_playback lockdownSound = {4960, 2, false, lockdownSamples};
struct audio_playback unlockSound = {2175, 2, false, unlockSamples};
struct audio_playback pickupSound = {20572, 3, false, pickupSamples};
struct audio_playback computerWinSound = {158489, 28, false,
                                          computerWinSamples};
struct audio_playback alarmSound = {60043, 14, false, alarmSamples};
struct audio_playback playerWinSound = {32348, 7, false, playerWinSamples};
struct audio_playback computerDeathSound = {14333, 2, false,
                                            computerDeathSamples};

int currMapNumber = 0, selectedGate = 0, currentPickups = 0;
int hexPattern[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 103};
int dx = 0, dy = 0, xCoord = 30, yCoord = 35, gateCoolDown = 0, blackedOut = 0,
    playerOrientation = playerFacingDown;
bool blackOutActive = false, startGame = false, lockedDoor = false,
     soundPlaying = false, showPin = false, finalGateClose = true;
volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
volatile int *key_ptr = (int *)KEY_BASE;
int status_reg;
int soundIterator = 0, winner = 0; // winner: -1 for computer, 1 for player

/*=======================================================================*/
/*Function Signatures*/
/*=======================================================================*/
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync(volatile int *pixel_ctrl_ptr);
void drawPlayerBox(int xTopLeft, int yTopLeft, short int line_color);
void drawBox(int x, int y, int startWidth, int startHeight,
             short int line_color);
void drawKeyCard(int x, int y);
void swap(int *a, int *b);
void draw_line(int x0, int y0, int x1, int y1, short int colour);
bool checkCollision(int x, int y);
bool playerOpensGate(int pin);
bool playerNearFinalGate();
bool playerEscapes();
void generateMap();
void drawDigit(int x, int y, int digit);
void displayGameTime(int Gametime);
void clearPreviousDrawings();
void drawPickUps();
void getKeyboardInput(int slack, volatile int *PS2_ptr, char *byte1,
                      char *byte2, char *byte3);
void computerAbilities();
void updateDrawingsPrevPositions();
void updatePlayerPosition();
void displayPickUps();
void drawTimeLabel(int x, int y);
void audio_playback_mono(int *samples, int n);
void drawPlayer(int x, int y, int clear, int orientation);
void drawPlayerFacingUp(int x, int y, int clear);
void drawPlayerFacingDown(int x, int y, int clear);
void drawPlayerFacingRight(int x, int y, int clear);
void drawPlayerFacingLeft(int x, int y, int clear);
bool touchedPickup(int x, int y, int i);
bool touchedWall(int x, int y);
void drawLightBulb();
void drawLockIcon();
void drawEndGate();
int generateRandomBinaryDigit();
int generateRandomPin();
void drawKeyIcon();
void drawPin(int pin);

/*=======================================================================*/
/*Map Arrays Pasted Here*/
/*=======================================================================*/

/*=======================================================================*/
/*Main Program*/
/*=======================================================================*/
int main(void) {
  /* Declare volatile pointers to I/O registers (volatile means that IO load
  and store instructions will be used to access these pointer locations,
  instead of regular memory loads and stores) */
  int slack = 0, gameTime = 50000000;
  bool CloseGate = false, everyOtherHalfSecond = false;
  volatile int *led_ptr = (int *)LED_BASE;
  volatile int *PS2_ptr = (int *)PS2_BASE;
  volatile int *timer_ptr = (int *)TIMER_BASE;
  char byte1 = 0, byte2 = 0, byte3 = 0, keyBoardKeyPressed = 0;

  // Seed the random function to always gurantee random pin
  srand(13844);

  // Generate random 10-bit pin for the match
  int pin = generateRandomPin();

  // alarm sound needs to have its volume lowered to be clearer
  for (int i = 0; i < alarmSound.samples_n; i++)
    alarmSound.samples[i] = alarmSound.samples[i] >> 8;

  // PS/2 mouse needs to be reset (must be already plugged in)
  *(PS2_ptr) = 0xFF; // reset

  *(key_ptr + 3) = 0x0000000F; // reset edge capture

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) =
      (int)&Buffer1; // first store the address in the back buffer

  *pixel_ctrl_ptr = 1; // wait for vsync

  wait_for_vsync(pixel_ctrl_ptr);

  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  generateMap(startScreen);

  while (1) {
    getKeyboardInput(slack, PS2_ptr, &byte1, &byte2, &byte3);
    if (byte3 == 0x29) {
      break;
    }
  }

  generateMap(level1); // pixel_buffer_start points to the pixel buffer
  drawEndGate();
  /* now, swap the front/back buffers, to set the front buffer location */
  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  generateMap(level1); // pixel_buffer_start points to the pixel buffer
  drawEndGate();

  // setup timer
  //  Store low-order 16 bits
  *(timer_ptr + 2) = 0x0000FFFF & gameTime;
  // Shift the upper 16 bits down
  gameTime = gameTime >> 16;
  // Store high-order 16 bits
  *(timer_ptr + 3) = 0x0000FFFF & gameTime;

  gameTime = 120;

  // start timer
  *(timer_ptr + 1) = 6;

  while (1) {
    slack++;
    if (*(timer_ptr)&0x00000001 == 1) {
      if (everyOtherHalfSecond) {
        everyOtherHalfSecond = false;
        if (gameTime > 0) {
          gameTime--;
        } else if (gameTime <= 0) {
          winner = COMPUTER_WIN;
          break;
        }

        if (gameTime == 67) {
          if (!soundPlaying) {
            decontOne.enabled = true;
            soundPlaying = true;
          }
        } else if (gameTime == 10) {
          if (!soundPlaying) {
            alarmSound.enabled = true;
            soundPlaying = true;
          }
        }

        if (gateCoolDown > 0)
          gateCoolDown--;
        else
          CloseGate = false;

        if (blackedOut > 0)
          blackedOut--;
        if (blackedOut <= 7)
          blackOutActive = false;
      } else
        everyOtherHalfSecond = true;

      // sound iterating
      if (decontOne.enabled) {
        decontOne.duration--;
        if (decontOne.duration <= 0) {
          decontOne.enabled = false;
          decontOne.duration = 0; // if you want playback multiple times, reset
                                  // to original value, otherwise 0
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (blackoutSound.enabled) {
        blackoutSound.duration--;
        if (blackoutSound.duration <= 0) {
          blackoutSound.enabled = false;
          blackoutSound.duration = 4;
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (doorSwitch.enabled) {
        doorSwitch.duration--;
        if (doorSwitch.duration <= 0) {
          doorSwitch.enabled = false;
          doorSwitch.duration = 1;
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (lockdownSound.enabled) {
        lockdownSound.duration--;
        if (lockdownSound.duration <= 0) {
          lockdownSound.enabled = false;
          lockdownSound.duration = 2;
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (unlockSound.enabled) {
        unlockSound.duration--;
        if (unlockSound.duration <= 0) {
          unlockSound.enabled = false;
          unlockSound.duration = 2;
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (pickupSound.enabled) {
        pickupSound.duration--;
        if (pickupSound.duration <= 0) {
          pickupSound.enabled = false;
          pickupSound.duration = 4;
          soundIterator = 0;
          soundPlaying = false;
        }
      }
      if (alarmSound.enabled) {
        alarmSound.duration--;
        if (alarmSound.duration <= 0) {
          alarmSound.enabled = false;
          alarmSound.duration = 14;
          soundIterator = 0;
          soundPlaying = false;
        }
      }

      *timer_ptr = 0;
    }

    // sound rendering
    if (decontOne.enabled) {
      audio_playback_mono(decontOne.samples, decontOne.samples_n);
    }
    if (blackoutSound.enabled) {
      audio_playback_mono(blackoutSound.samples, blackoutSound.samples_n);
    }
    if (doorSwitch.enabled) {
      audio_playback_mono(doorSwitch.samples, doorSwitch.samples_n);
    }
    if (lockdownSound.enabled) {
      audio_playback_mono(lockdownSound.samples, lockdownSound.samples_n);
    }
    if (unlockSound.enabled) {
      audio_playback_mono(unlockSound.samples, unlockSound.samples_n);
    }
    if (pickupSound.enabled) {
      audio_playback_mono(pickupSound.samples, pickupSound.samples_n);
    }
    if (alarmSound.enabled) {
      audio_playback_mono(alarmSound.samples, alarmSound.samples_n);
    }

    if (currentPickups == 8) {
      // Show the player the random 10 bit code on the LEDs and the screen
      volatile int *ledr_ptr = (int *)LED_BASE;
      *ledr_ptr = pin;
      drawKeyIcon();
      drawPin(pin);
      showPin = true;
    }

    // check to see if player has entered the pin at the gate and opened it
    if (playerOpensGate(pin))
      winner = PLAYER_WIN; // Open the gate

    if (playerEscapes())
      break;

    // display gametime
    drawTimeLabel(258, 7);
    displayGameTime(gameTime);

    // display pickups
    displayPickUps();

    // check if final gate is open
    drawEndGate();

    // update player position
    getKeyboardInput(slack, PS2_ptr, &byte1, &byte2, &byte3);

    // player rendering
    if (slack >= 3) {
      slack = 0;
      if (byte3 == (char)0x1d) { // W
        dy = -1;
        dx = 0;

        if (playerOrientation == playerFacingLeft) {

          if (level1[320 * (yCoord) + (xCoord - 1)] == 0x3186) {
            playerOrientation = playerFacingUp;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (playerOrientation == playerFacingRight) {
          // Ensure final gate is sticky
          if ((level1[320 * (yCoord) + (xCoord + 2)] == 0x3186) &&
              (level1[320 * (yCoord) + (xCoord + 3)] == 0x3186) &&
              !((winner != PLAYER_WIN) && (xCoord + 2 == 305) &&
                ((yCoord >= 22) && (yCoord <= 46)))) {
            playerOrientation = playerFacingUp;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (checkCollision(xCoord, yCoord))
          playerOrientation = playerFacingUp;

      } else if (byte3 == (char)0x1c) { // A
        dx = -1;
        dy = 0;

        if (playerOrientation == playerFacingUp) {

          if (level1[320 * (yCoord - 1) + (xCoord)] == 0x3186) {
            playerOrientation = playerFacingLeft;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (playerOrientation == playerFacingDown) {
          if (level1[320 * (yCoord + 2) + (xCoord)] == 0x3186) {
            playerOrientation = playerFacingLeft;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (checkCollision(xCoord, yCoord))
          playerOrientation = playerFacingLeft;

      } else if (byte3 == (char)0x1b) { // S
        dy = 1;
        dx = 0;

        if (playerOrientation == playerFacingLeft) {

          if ((level1[320 * (yCoord) + (xCoord - 1)] == 0x3186) &&
              (level1[320 * (yCoord) + (xCoord - 2)] == 0x3186)) {
            playerOrientation = playerFacingDown;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (playerOrientation == playerFacingRight) {
          // Ensure final gate is sticky
          if ((level1[320 * (yCoord) + (xCoord + 2)] == 0x3186) &&
              !((winner != PLAYER_WIN) && (xCoord + 2 == 305) &&
                ((yCoord >= 22) && (yCoord <= 46)))) {
            playerOrientation = playerFacingDown;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (checkCollision(xCoord, yCoord))
          playerOrientation = playerFacingDown;

      } else if (byte3 == (char)0x23) { // D
        dx = 1;
        dy = 0;

        if (playerOrientation == playerFacingUp) {

          if (level1[320 * (yCoord - 1) + (xCoord)] == 0x3186) {
            playerOrientation = playerFacingRight;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (playerOrientation == playerFacingDown) {
          if (level1[320 * (yCoord + 2) + (xCoord)] == 0x3186) {
            playerOrientation = playerFacingRight;
          } else {
            dx = 0;
            dy = 0;
          }

        } else if (checkCollision(xCoord, yCoord))
          playerOrientation = playerFacingRight;
      }

      clearPreviousDrawings();

      // drawPlayerBox(xCoord, yCoord, 0x00FFFF);
      drawPlayer(xCoord, yCoord, 0, playerOrientation);

      drawPickUps();

      updateDrawingsPrevPositions();

      updatePlayerPosition();
    }

    computerAbilities();

    *pixel_ctrl_ptr = 1;
    do {
      status_reg = *(pixel_ctrl_ptr + 3);
    } while ((status_reg & 0x01) !=
             0); // poll until the the buffer has completed drawing
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  }

  if (winner == COMPUTER_WIN) {
    soundIterator = 0;
    computerWinSound.duration = 28;
    computerWinSound.enabled = true;
    generateMap(endscreen); // pixel_buffer_start points to the pixel buffer

    /* now, swap the front/back buffers, to set the front buffer location */
    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    generateMap(endscreen); // pixel_buffer_start points to the pixel buffer
    *pixel_ctrl_ptr = 1;
    wait_for_vsync(pixel_ctrl_ptr);
    while (1) {
      if (*(timer_ptr)&0x00000001 == 1) {
        computerWinSound.duration--;
        if (computerWinSound.duration <= 0) {
          computerWinSound.enabled = false;
          computerWinSound.duration = 28;
          break;
        }
        *timer_ptr = 0;
      }
      if (computerWinSound.enabled)
        audio_playback_mono(computerWinSound.samples,
                            computerWinSound.samples_n);
    }
  } else if (winner == PLAYER_WIN) {
    soundIterator = 0;
    playerWinSound.enabled = true;
    generateMap(
        playerEndScreen); // pixel_buffer_start points to the pixel buffer

    /* now, swap the front/back buffers, to set the front buffer location */
    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    generateMap(
        playerEndScreen); // pixel_buffer_start points to the pixel buffer
    *pixel_ctrl_ptr = 1;
    wait_for_vsync(pixel_ctrl_ptr);
    while (1) {
      if (*(timer_ptr)&0x00000001 == 1) {
        playerWinSound.duration--;
        if (playerWinSound.duration <= 0) {
          playerWinSound.enabled = false;
          // playerWinSound.duration = 8;
          break;
        }
        *timer_ptr = 0;
      }
      if (playerWinSound.enabled)
        audio_playback_mono(playerWinSound.samples, playerWinSound.samples_n);
    }

    computerDeathSound.enabled = true;
    soundIterator = 0;
    while (1) {
      if (*(timer_ptr)&0x00000001 == 1) {
        computerDeathSound.duration--;
        if (computerDeathSound.duration <= 0) {
          computerDeathSound.enabled = false;
          // computerDeathSound.duration = 2;
          break;
        }
        *timer_ptr = 0;
      }
      if (computerDeathSound.enabled)
        audio_playback_mono(computerDeathSound.samples,
                            computerDeathSound.samples_n);
    }
  }
}

/*=======================================================================*/
/*Helper Functions*/
/*=======================================================================*/
void plot_pixel(int x, int y, short int line_color) {
  volatile short int *one_pixel_address;
  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  *one_pixel_address = line_color;
}

void clear_screen() {
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      plot_pixel(x, y, 0);
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, short int colour) {
  bool is_steep = abs(y1 - y0) > abs(x1 - x0);
  if (is_steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0;

  int y_step = 0;
  if (y0 < y1)
    y_step = 1;
  else
    y_step = -1;

  for (int x = x0; x <= x1; x++) {
    if (is_steep) {
      plot_pixel(y, x, colour);
    } else {
      plot_pixel(x, y, colour);
    }

    error = error + deltay;
    if (error > 0) {
      y = y + y_step;
      error = error - deltax;
    }
  }
}

void drawPlayerBox(int x, int y, short int line_color) {
  for (int height = y - (int)(PLAYER_SIZE / 2);
       height < (int)(y + PLAYER_SIZE / 2); height++) {
    for (int width = x - (int)(PLAYER_SIZE / 2);
         width < (int)(x + PLAYER_SIZE / 2); width++) {
      plot_pixel(width, height, line_color);
    }
  }
}

void drawPlayer(int x, int y, int clear, int orientation) {
  if (orientation == playerFacingUp)
    drawPlayerFacingUp(x, y, clear);
  else if (orientation == playerFacingDown)
    drawPlayerFacingDown(x, y, clear);
  else if (orientation == playerFacingRight)
    drawPlayerFacingRight(x, y, clear);
  else
    drawPlayerFacingLeft(x, y, clear);
}

void drawPlayerFacingUp(int x, int y, int clear) {
  for (int width = 0; width < 4; width++) {
    for (int height = 0; height < 2; height++) {
      if (clear)
        plot_pixel(x + width, y + height, 0x3186);
      else
        plot_pixel(x + width, y + height, playerUp[4 * height + width]);
    }
  }
}

void drawPlayerFacingDown(int x, int y, int clear) {
  for (int width = 0; width < 4; width++) {
    for (int height = 0; height < 2; height++) {
      if (clear)
        plot_pixel(x + width, y + height, 0x3186);
      else
        plot_pixel(x + width, y + height, playerDown[4 * height + width]);
    }
  }
}

void drawPlayerFacingRight(int x, int y, int clear) {
  for (int width = 0; width < 2; width++) {
    for (int height = 0; height < 4; height++) {
      if (clear)
        plot_pixel(x + width, y + height, 0x3186);
      else
        plot_pixel(x + width, y + height, playerRight[2 * height + width]);
    }
  }
}

void drawPlayerFacingLeft(int x, int y, int clear) {
  for (int width = 0; width < 2; width++) {
    for (int height = 0; height < 4; height++) {
      if (clear)
        plot_pixel(x + width, y + height, 0x3186);
      else
        plot_pixel(x + width, y + height, playerLeft[2 * height + width]);
    }
  }
}

void drawBox(int x, int y, int startWidth, int startHeight,
             short int line_color) {
  for (int height = y - (int)(startHeight / 2);
       height < (int)(y + startHeight / 2); height++) {
    for (int width = x - (int)(startWidth / 2);
         width < (int)(x + startWidth / 2); width++) {
      plot_pixel(width, height, line_color);
    }
  }
}

void wait_for_vsync(volatile int *pixel_ctrl_ptr) {
  do {
    status_reg = *(pixel_ctrl_ptr + 3);
  } while ((status_reg & 0x01) != 0); // poll until the the buffer has
  // completed drawing
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void generateMap(int currentMap[]) {
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      plot_pixel(x, y, currentMap[320 * y + x]);
    }
  }
}

bool checkCollision(int xCoord, int yCoord) {
  int leftShoulderXPx = 0, leftShoulderYPx = 0, leftHeadXPx = 0,
      leftHeadYPx = 0;
  int rightHeadXPx = 0, rightHeadYPx = 0, rightShoulderXPx = 0,
      rightShoulderYPx = 0;

  if (playerOrientation == playerFacingUp) {
    leftShoulderXPx = xCoord;
    leftShoulderYPx = yCoord;

    leftHeadXPx = xCoord + 1;
    leftHeadYPx = yCoord;

    rightHeadXPx = leftHeadXPx + 1;
    rightHeadYPx = leftHeadYPx;

    rightShoulderXPx = leftShoulderXPx + 3;
    rightShoulderYPx = leftShoulderYPx;

  } else if (playerOrientation == playerFacingDown) {
    leftShoulderXPx = xCoord + 3;
    leftShoulderYPx = yCoord + 1;

    leftHeadXPx = xCoord + 2;
    leftHeadYPx = yCoord + 1;

    rightHeadXPx = leftHeadXPx - 1;
    rightHeadYPx = leftHeadYPx;

    rightShoulderXPx = xCoord;
    rightShoulderYPx = yCoord + 1;

  } else if (playerOrientation == playerFacingRight) {
    leftShoulderXPx = xCoord + 1;
    leftShoulderYPx = yCoord;

    leftHeadXPx = xCoord + 1;
    leftHeadYPx = yCoord + 1;

    rightHeadXPx = leftHeadXPx;
    rightHeadYPx = leftHeadYPx + 1;

    rightShoulderXPx = leftShoulderXPx;
    rightShoulderYPx = leftShoulderYPx + 3;

  } else {
    leftShoulderXPx = xCoord;
    leftShoulderYPx = yCoord + 3;

    leftHeadXPx = xCoord;
    leftHeadYPx = yCoord + 2;

    rightHeadXPx = leftHeadXPx;
    rightHeadYPx = leftHeadYPx - 1;

    rightShoulderXPx = xCoord;
    rightShoulderYPx = yCoord;
  }

  leftShoulderYPx += dy;
  leftShoulderXPx += dx;

  leftHeadYPx += dy;
  leftHeadXPx += dx;

  rightHeadYPx += dy;
  rightHeadXPx += dx;

  rightShoulderYPx += dy;
  rightShoulderXPx += dx;

  bool result =
      (level1[320 * (leftShoulderYPx) + (leftShoulderXPx)] == 0x3186 &&
       level1[320 * (leftHeadYPx) + (leftHeadXPx)] == 0x3186 &&
       level1[320 * (rightHeadYPx) + (rightHeadXPx)] == 0x3186 &&
       level1[320 * (rightShoulderYPx) + (rightShoulderXPx)] == 0x3186);

  // Ensure player can't escape if final gate is closed
  if (winner != PLAYER_WIN) {
    // Check to see if the player collides with gate
    bool finalGateCollision =
        !((leftShoulderYPx >= 22 && leftShoulderYPx <= 46 &&
           leftShoulderXPx == 305) ||
          (leftHeadYPx >= 22 && leftHeadYPx <= 46 && leftHeadXPx == 305) ||
          (rightHeadYPx >= 22 && rightHeadYPx <= 46 && rightHeadXPx == 305) ||
          (rightShoulderYPx >= 22 && rightShoulderYPx <= 46 &&
           rightShoulderXPx == 305));

    result = result && finalGateCollision;

  } // else, no need to check for collision as player can walk past the open
    // gate

  // If there are not gates placed, no need to check gate collision logic
  if (gateCoolDown == 0)
    return result;

  currLevelGatesArray = gates[currMapNumber];
  currGate = currLevelGatesArray[selectedGate];

  // Check corner collision with closed gate
  return !((currGate.startX <= leftShoulderXPx &&
            currGate.endX >= leftShoulderXPx) &&
               (currGate.startY <= leftShoulderYPx &&
                currGate.endY >= leftShoulderYPx) ||
           (currGate.startX <= leftHeadXPx && currGate.endX >= leftHeadXPx) &&
               (currGate.startY <= leftHeadYPx &&
                currGate.endY >= leftHeadYPx) ||
           (currGate.startX <= rightHeadXPx && currGate.endX >= rightHeadXPx) &&
               (currGate.startY <= rightHeadYPx &&
                currGate.endY >= rightHeadYPx) ||
           (currGate.startX <= rightShoulderXPx &&
            currGate.endX >= rightShoulderXPx) &&
               (currGate.startY <= rightShoulderYPx &&
                currGate.endY >= rightShoulderYPx)) &&
         result;
}

void displayGameTime(int Gametime) {
  volatile int *hex_ptr = (int *)HEX3_HEX0_BASE;
  int HundredsDigit = 0, tensDigit = 0, onesDigit = 0, displayTime = 0;
  // Clear the previous number on display
  drawBox(300, 7, 18, 10, 0x3186);

  // Isolate the digits of the counter value
  onesDigit = Gametime % 10;
  Gametime /= 10;
  tensDigit = Gametime % 10;
  HundredsDigit = Gametime / 10;

  // Add the hundreds digit
  displayTime += hexPattern[HundredsDigit];
  displayTime = displayTime << 8;

  // Add the tens digit
  displayTime += hexPattern[tensDigit];
  displayTime = displayTime << 8;

  // Add the ones digit
  displayTime += hexPattern[onesDigit];

  // Display the game counter
  *(hex_ptr) = displayTime;
  drawDigit(294, 7, HundredsDigit);
  drawDigit(300, 7, tensDigit);
  drawDigit(306, 7, onesDigit);
}

void displayPickUps() {
  // Clear the previous count value
  drawBox(240, 7, 8, 8, 0x3186);

  // Draw the current pick up
  drawDigit(240, 7, currentPickups);

  drawKeyCard(245, 5);
}

void drawDigit(int x, int y, int digit) {
  if (digit == 0) {
    draw_line(x - 2, y - 2, x - 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 1) {
    draw_line(x, y - 2, x, y + 2, 0x00FFFF);
  } else if (digit == 2) {
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y, 0x00FFFF);
    draw_line(x + 2, y, x - 2, y, 0x00FFFF);
    draw_line(x - 2, y, x - 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 3) {
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
    draw_line(x + 2, y, x - 2, y, 0x00FFFF);
    draw_line(x + 2, y + 2, x - 2, y + 2, 0x00FFFF);
  } else if (digit == 4) {
    draw_line(x - 2, y - 2, x - 2, y, 0x00FFFF);
    draw_line(x - 2, y, x + 2, y, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 5) {
    draw_line(x + 2, y - 2, x - 2, y - 2, 0x00FFFF);
    draw_line(x - 2, y - 2, x - 2, y, 0x0FFFF);
    draw_line(x - 2, y, x + 2, y, 0x00FFFF);
    draw_line(x + 2, y, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 6) {
    draw_line(x + 2, y - 2, x - 2, y - 2, 0x00FFFF);
    draw_line(x - 2, y - 2, x - 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y, x + 2, y, 0x00FFFF);
    draw_line(x + 2, y, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 7) {
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
  } else if (digit == 8) {
    draw_line(x - 2, y - 2, x - 2, y + 2, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y, x + 2, y, 0x0FFFF);
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
  } else if (digit == 9) {
    draw_line(x - 2, y - 2, x + 2, y - 2, 0x00FFFF);
    draw_line(x - 2, y - 2, x - 2, y, 0x00FFFF);
    draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);
    draw_line(x - 2, y, x + 2, y, 0x00FFFF);
    draw_line(x - 2, y + 2, x + 2, y + 2, 0x00FFFF);
  } else
    return;
}

void clearPreviousDrawings() {
  if (pixel_buffer_start == (int)&Buffer1) {

    if (orientationPrevArray[0] == playerFacingUp)
      drawPlayer(xPrevArray[0], yPrevArray[0], 1, playerFacingUp);
    else if (orientationPrevArray[0] == playerFacingDown)
      drawPlayer(xPrevArray[0], yPrevArray[0], 1, playerFacingDown);
    else if (orientationPrevArray[0] == playerFacingRight)
      drawPlayer(xPrevArray[0], yPrevArray[0], 1, playerFacingRight);
    else
      drawPlayer(xPrevArray[0], yPrevArray[0], 1, playerFacingLeft);

    // drawPlayerBox(xPrevArray[0], yPrevArray[0], 0x3186);
    draw_line(gatePrevPos[0][0], gatePrevPos[0][1], gatePrevPos[0][2],
              gatePrevPos[0][3], 0x3186);

    drawEndGate();

  } else {

    if (orientationPrevArray[1] == playerFacingUp)
      drawPlayer(xPrevArray[1], yPrevArray[1], 1, playerFacingUp);
    else if (orientationPrevArray[1] == playerFacingDown)
      drawPlayer(xPrevArray[1], yPrevArray[1], 1, playerFacingDown);
    else if (orientationPrevArray[1] == playerFacingRight)
      drawPlayer(xPrevArray[1], yPrevArray[1], 1, playerFacingRight);
    else
      drawPlayer(xPrevArray[1], yPrevArray[1], 1, playerFacingLeft);

    // drawPlayerBox(xPrevArray[1], yPrevArray[1], 0x3186);
    draw_line(gatePrevPos[1][0], gatePrevPos[1][1], gatePrevPos[1][2],
              gatePrevPos[1][3], 0x3186);

    drawEndGate();
  }
}

void drawLightBulb() {
  if (blackOutActive) {
    // Draw the off light bulb
    for (int width = 0; width < 10; width++) {
      for (int height = 0; height < 14; height++) {
        plot_pixel(width + 180, height + 7, lightOff[10 * height + width]);
      }
    }

  } else {
    // Draw the on light bulb
    for (int width = 0; width < 10; width++) {
      for (int height = 0; height < 14; height++) {
        plot_pixel(width + 180, height + 7, lightOn[10 * height + width]);
      }
    }
  }
}

void drawLockIcon() {
  if (lockedDoor) {
    // Draw the locked icon
    for (int width = 0; width < 10; width++) {
      for (int height = 0; height < 14; height++) {
        plot_pixel(width + 140, height + 7, locked[10 * height + width]);
      }
    }

  } else {
    // Draw the unlocked icon
    for (int width = 0; width < 10; width++) {
      for (int height = 0; height < 14; height++) {
        plot_pixel(width + 140, height + 7, unlock[10 * height + width]);
      }
    }
  }
}

void drawPickUps() {
  // Pickup rendering
  for (int i = 0; i < NUM_PICKUPS; i++) {
    if (pickupArray[i][0] != -1) {
      if (blackOutActive) {
        drawLightBulb();
        drawLockIcon();
        drawBox(pickupArray[i][0] + 4, pickupArray[i][1] + 2, 8, 6, 0x3186);
      } else {
        drawLightBulb();
        drawLockIcon();
        drawKeyCard(pickupArray[i][0],
                    pickupArray[i][1]); // drawBox(pickupArray[i][0],
                                        // pickupArray[i][1], PICKUP_SIZE,
                                        // PICKUP_SIZE, 0xFFFF00);
      }
    }
  }
}

void updateDrawingsPrevPositions() {
  if (pixel_buffer_start == (int)&Buffer1) {
    xPrevArray[0] = xCoord;
    yPrevArray[0] = yCoord;
    orientationPrevArray[0] = playerOrientation;

    currLevelGatesArray = gates[currMapNumber];
    currGate = currLevelGatesArray[selectedGate];

    gatePrevPos[0][0] = currGate.startX;
    gatePrevPos[0][1] = currGate.startY;
    gatePrevPos[0][2] = currGate.endX;
    gatePrevPos[0][3] = currGate.endY;

  } else {
    xPrevArray[1] = xCoord;
    yPrevArray[1] = yCoord;
    orientationPrevArray[1] = playerOrientation;

    currLevelGatesArray = gates[currMapNumber];
    currGate = currLevelGatesArray[selectedGate];

    gatePrevPos[1][0] = currGate.startX;
    gatePrevPos[1][1] = currGate.startY;
    gatePrevPos[1][2] = currGate.endX;
    gatePrevPos[1][3] = currGate.endY;
  }
}

void computerAbilities() {

  // ensure that gate building ability is not on cooldown
  if (gateCoolDown == 0) {

    // get edge capture reg contents
    if (*(key_ptr + 3) & 0x00000001) {
      if (!soundPlaying) {
        doorSwitch.enabled = true;
        soundPlaying = true;
      }
      // if user wants to increment gate number and reached the last gate,
      // wrap around the list
      if (selectedGate == (numGatesInLevel[currMapNumber] - 1))
        selectedGate = 0;
      else
        selectedGate++;

      // reset edge capture
      *(key_ptr + 3) = 0x0000000F;

      currLevelGatesArray = gates[currMapNumber];
      currGate = currLevelGatesArray[selectedGate];

      draw_line(currGate.startX, currGate.startY, currGate.endX, currGate.endY,
                0x0ffd);

    } else if (*(key_ptr + 3) & 0x00000002) {
      if (!soundPlaying) {
        doorSwitch.enabled = true;
        soundPlaying = true;
      }
      if (selectedGate == 0)
        selectedGate = numGatesInLevel[currMapNumber] - 1;
      else
        selectedGate--;

      // reset edge capture
      *(key_ptr + 3) = 0x0000000F;

      // draw the cyan gate
      currLevelGatesArray = gates[currMapNumber];
      currGate = currLevelGatesArray[selectedGate];
      draw_line(currGate.startX, currGate.startY, currGate.endX, currGate.endY,
                0x0ffd);

    } else if (*(key_ptr + 3) & 0x00000004) {
      if (!soundPlaying) {
        lockdownSound.enabled = true;
        soundPlaying = true;
      }
      lockedDoor = true;
      drawLockIcon();
      // build gate at current location
      gateCoolDown = 6; // y = 6 - 2t -- > 6 = 2t -> t = 3 seconds cooldown

      // reset edge capture
      *(key_ptr + 3) = 0x0000000F;

      // draw the red gate
      currLevelGatesArray = gates[currMapNumber];
      currGate = currLevelGatesArray[selectedGate];
      draw_line(currGate.startX, currGate.startY, currGate.endX, currGate.endY,
                0xf800);

    } else if (*(key_ptr + 3) & 0x00000008) {
      // blackout ability used
      if (!blackOutActive && blackedOut <= 0) {
        blackedOut = 14; // y = 6 - 2t -- > 6 = 2t -> t = 3 seconds cooldown
        blackOutActive = true;
        if (!soundPlaying) {
          blackoutSound.enabled = true;
          soundPlaying = true;
        }
      }

      // reset edge capture
      *(key_ptr + 3) = 0x0000000F;

    } else {
      if (lockedDoor) {
        lockedDoor = false;
        drawLockIcon();
        if (!soundPlaying) {
          unlockSound.enabled = true;
          soundPlaying = true;
        }
      }
      currLevelGatesArray = gates[currMapNumber];
      currGate = currLevelGatesArray[selectedGate];
      draw_line(currGate.startX, currGate.startY, currGate.endX, currGate.endY,
                0x0ffd);
    }
  } else if (gateCoolDown > 0) {
    // draw red gate at the selected gate position
    currLevelGatesArray = gates[currMapNumber];
    currGate = currLevelGatesArray[selectedGate];
    draw_line(currGate.startX, currGate.startY, currGate.endX, currGate.endY,
              0xf800);
  }
}

void getKeyboardInput(int slack, volatile int *PS2_ptr, char *byte1,
                      char *byte2, char *byte3) {
  int PS2_data = *(PS2_ptr);      // read the Data register in the PS/2 port
  int RVALID = PS2_data & 0x8000; // extract the RVALID field
  if (RVALID) {
    /* shift the next data byte into the display */
    *byte1 = *byte2;
    *byte2 = *byte3;
    *byte3 = PS2_data & 0xFF;
    if ((*byte2 == (char)0xAA) && (*byte3 == (char)0x00))
      *(PS2_ptr) = 0xF4;
  }
}

void updatePlayerPosition() {
  if (checkCollision(xCoord, yCoord)) {
    xCoord += dx;
    yCoord += dy;
  }

  int leftShoulderXPx = 0, leftShoulderYPx = 0, leftHeadXPx = 0,
      leftHeadYPx = 0;
  int rightHeadXPx = 0, rightHeadYPx = 0, rightShoulderXPx = 0,
      rightShoulderYPx = 0;

  if (playerOrientation == playerFacingUp) {
    leftShoulderXPx = xCoord;
    leftShoulderYPx = yCoord;

    leftHeadXPx = xCoord + 1;
    leftHeadYPx = yCoord;

    rightHeadXPx = leftHeadXPx + 1;
    rightHeadYPx = leftHeadYPx;

    rightShoulderXPx = leftShoulderXPx + 3;
    rightShoulderYPx = leftShoulderYPx;

  } else if (playerOrientation == playerFacingDown) {
    leftShoulderXPx = xCoord + 3;
    leftShoulderYPx = yCoord + 1;

    leftHeadXPx = xCoord + 2;
    leftHeadYPx = yCoord + 1;

    rightHeadXPx = leftHeadXPx - 1;
    rightHeadYPx = leftHeadYPx;

    rightShoulderXPx = xCoord;
    rightShoulderYPx = yCoord + 1;

  } else if (playerOrientation == playerFacingRight) {
    leftShoulderXPx = xCoord + 1;
    leftShoulderYPx = yCoord;

    leftHeadXPx = xCoord + 1;
    leftHeadYPx = yCoord + 1;

    rightHeadXPx = leftHeadXPx;
    rightHeadYPx = leftHeadYPx + 1;

    rightShoulderXPx = leftShoulderXPx;
    rightShoulderYPx = leftShoulderYPx + 3;

  } else {
    leftShoulderXPx = xCoord;
    leftShoulderYPx = yCoord + 3;

    leftHeadXPx = xCoord;
    leftHeadYPx = yCoord + 2;

    rightHeadXPx = leftHeadXPx;
    rightHeadYPx = leftHeadYPx - 1;

    rightShoulderXPx = xCoord;
    rightShoulderYPx = yCoord;
  }
  // check pickup status
  for (int i = 0; i < NUM_PICKUPS; i++) {

    if (touchedPickup(leftShoulderXPx, leftShoulderYPx, i) ||
        touchedPickup(leftHeadXPx, leftHeadYPx, i) ||
        touchedPickup(rightHeadXPx, rightHeadYPx, i) ||
        touchedPickup(rightShoulderXPx, rightShoulderYPx, i)) {
      currentPickups++;
      if (!soundPlaying) {
        pickupSound.enabled = true;
        soundPlaying = true;
      }
      drawBox(pickupArray[i][0] + 4, pickupArray[i][1] + 2, 8, 6, 0x3186);
      *pixel_ctrl_ptr = 1;

      do {
        status_reg = *(pixel_ctrl_ptr + 3);
      } while ((status_reg & 0x01) !=
               0); // poll until the the buffer has completed drawing

      pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      drawBox(pickupArray[i][0] + 4, pickupArray[i][1] + 2, 8, 6, 0x3186);
      pickupArray[i][0] = -1;
      pickupArray[i][1] = -1;

      break;
    }
  }
}

bool touchedPickup(int x, int y, int i) {
  return (xCoord + dx >= pickupArray[i][0] &&
          xCoord + dx <= pickupArray[i][0] + 8 &&
          yCoord + dy >= pickupArray[i][1] &&
          yCoord + dy <= pickupArray[i][1] + 5);
}

// Right the word: "Time: " to enhance usability of game information
void drawTimeLabel(int x, int y) {
  // T
  draw_line(x, y - 2, x + 4, y - 2, 0x00FFFF);
  draw_line(x + 2, y - 2, x + 2, y + 2, 0x00FFFF);

  // i
  draw_line(x + 6, y - 2, x + 6, y + 2, 0x00FFFF);

  // M
  draw_line(x + 8, y - 2, x + 8, y + 2, 0x00FFFF);
  draw_line(x + 8, y - 2, x + 11, y, 0x00FFFF);
  draw_line(x + 11, y, x + 14, y - 2, 0x00FFFF);
  draw_line(x + 14, y - 2, x + 14, y + 2, 0x00FFFF);

  // e
  draw_line(x + 16, y - 2, x + 20, y - 2, 0x00FFFF);
  draw_line(x + 16, y, x + 18, y, 0x00FFFF);
  draw_line(x + 16, y + 2, x + 20, y + 2, 0x00FFFF);
  draw_line(x + 16, y - 2, x + 16, y + 2, 0x00FFFF);

  // :
  drawBox(x + 24, y - 2, 2, 2, 0x00FFFF);
  drawBox(x + 24, y + 2, 2, 2, 0x00FFFF);
}

void audio_playback_mono(int *samples, int n) {
  while (audiop->warc) {
    audiop->ldata = samples[soundIterator] << 16;
    audiop->rdata = samples[soundIterator++] << 16;
    if (soundIterator >= n) {
      break;
    }
  }
}

void drawKeyCard(int x, int y) {
  for (int row = 0; row < 8; row++) {
    for (int height = 0; height < 5; height++) {
      plot_pixel(x + row, y + height, keyCard[8 * height + row]);
    }
  }
}

void drawEndGate() {
  if (winner == PLAYER_WIN) {
    // Open the gate since the player won
    draw_line(305, 22, 305, 46, 0x3186);

  } else {
    // Draw red gate since player did not collect all cards
    draw_line(305, 22, 305, 46, 0xe8e4);
  }
}

int generateRandomBinaryDigit() { return rand() % 2; }

int generateRandomPin() {
  // Initialize result as either a 0 or 1
  int binaryPin = generateRandomBinaryDigit();

  // Loop 9 times, each time generate a random binary digit and concatinate it
  // with the previous result bit shifted to the left
  for (int i = 0; i < 9; i++) {
    binaryPin = (binaryPin << 1) | generateRandomBinaryDigit();
  }

  return binaryPin;
}

// Draw the key icon when the pin is revealed onto the screen
void drawKeyIcon() {
  for (int width = 0; width < 14; width++) {
    for (int height = 0; height < 7; height++) {
      plot_pixel(width + 8, height + 226, keyIcon[14 * height + width]);
    }
  }
}

void drawPin(int pin) {
  int bit = 0;
  for (int digitIdx = 0; digitIdx < 10; digitIdx++) {
    bit = pin & 0x1;
    pin = pin >> 1;
    drawDigit(80 - (digitIdx * 6), 229, bit);
  }
}

bool playerOpensGate(int pin) {
  if (playerNearFinalGate()) {
    // Take pin from switches and compared it with the random pin, if they're
    // the same, player wins
    volatile int *sw_ptr = (int *)SW_BASE;
    int userInputPin = *sw_ptr;

    return userInputPin == pin;

  } else
    return false;
}

bool playerNearFinalGate() {
  // If the pin is shown, check to see if the player is within the final gate
  // proximity
  if (showPin) {
    return (xCoord >= 290 && xCoord <= 304 && yCoord >= 22 && yCoord <= 47);
  }
}

bool playerEscapes() {
  // If the player steps past the opened gate, they win
  return (xCoord >= 315 && xCoord <= 319 && yCoord >= 22 && yCoord <= 47);
}

/*=======================================================================*/
