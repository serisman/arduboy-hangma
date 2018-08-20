// HANGMAN!
// --------
// by serisman <arduboy@serisman.com>
// version: 1.0.0

#include <Arduboy2.h>
Arduboy2 arduboy;

//#define DEBUG 1

#include "words.h"

#define MODE_TITLE    0
#define MODE_PLAY     1
#define MODE_CORRECT  2
#define MODE_DEAD     3

uint8_t mode = MODE_TITLE;

uint16_t score = 0;
uint8_t hangman = 0;
uint16_t previousWordIndex = 0xFFFF;
char currentWord[9];
uint8_t cursor = 0;
uint8_t cursorBlink = 0;
uint8_t usedLetters[26];

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
}

void loop() {
  if (arduboy.nextFrame()) {
    nextFrame();
  } else {
    arduboy.idle();
  }
}

void nextFrame() {
  arduboy.pollButtons();
  arduboy.clear();
  switch (mode) {
    case MODE_TITLE:
      nextTitleFrame();
      break;
    case MODE_PLAY:
      nextPlayFrame();
      break;
    case MODE_CORRECT:
      nextCorrectFrame();
      break;
    case MODE_DEAD:
      nextDeadFrame();
      break;
  }    
  arduboy.display();
}

void nextTitleFrame() {
  if (arduboy.justPressed(A_BUTTON)) {
    startPlaying();
    return;
  }
  
  if (arduboy.everyXFrames(10)) {
    if (hangman < 6) hangman++;
    else hangman = 0;
  }
  
  drawTitle();
  drawHangman();
}

void drawTitle() {
  arduboy.setCursor(0,10);
  arduboy.print(F("HANGMAN!"));
  arduboy.drawFastHLine(0,19,45);
  arduboy.setCursor(0,22);
  arduboy.print(F(" by serisman"));
  arduboy.setCursor(0,64-15);
  arduboy.print(F("Press A\n to begin."));
}

void startPlaying() {
  pickAWord();
  cursor = 0;
  hangman = 0;
  memset(usedLetters, 0, sizeof(usedLetters));
  mode = MODE_PLAY;
}

void nextPlayFrame() {
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    if (cursor == 25) cursor = 0;
    else cursor++;
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    if (cursor == 0) cursor = 25;
    else cursor--;
  }
  if (arduboy.justPressed(UP_BUTTON)) {
    if (cursor > 8) cursor -= 9;
  }
  if (arduboy.justPressed(DOWN_BUTTON)) {
    if (cursor < (26-9)) cursor += 9;
  }

  if (arduboy.justPressed(A_BUTTON)) {
    if (usedLetters[cursor] == 0) {
      usedLetters[cursor] = 1;
      scoreResponse(cursor+65);
    }    
  }

#ifdef DEBUG
  if (arduboy.justPressed(B_BUTTON)) { // For easier testing...
    startPlaying();
  }
#endif

  if (arduboy.everyXFrames(10)) {
    cursorBlink = 1-cursorBlink;
  }
  
  drawScore();
  drawHangman();
  drawWord();
  drawKeyboard();  
}

void nextCorrectFrame() {
  if (arduboy.justPressed(A_BUTTON)) {
    startPlaying();
    return;
  }
  
  drawScore();
  drawHangman();
  drawWord();
  drawCorrect();
}

void nextDeadFrame() {
  if (arduboy.justPressed(A_BUTTON)) {
    startPlaying();
    return;
  }
  
  drawScore();
  drawHangman();
  drawWord();
  drawDead();
}

void drawScore() {
  char buf[12];
  itoa(score, buf, 10);
  
  arduboy.setCursor((128-50)-(strlen(buf)*6),0);
  arduboy.print(buf);
}

void drawHangman() {
  const uint8_t LEFT = (128-45);
  // width: 50
  // height: 64

  arduboy.fillRect(LEFT+5, 61, 40, 3); // ground
  arduboy.fillRect(LEFT+30, 0, 3, 64); // post
  arduboy.fillRect(LEFT+10, 0, 20, 3); // bar
  arduboy.fillRect(LEFT+10, 0, 3, 10); // noose

  if (hangman > 0) { // head
    arduboy.drawCircle(LEFT+11, 15, 5);
  }
  if (hangman > 1) { // body
    arduboy.drawFastVLine(LEFT+11, 20, 20);
  }
  if (hangman > 2) { // left arm
    arduboy.drawLine(LEFT+0, 22, LEFT+11, 25);
  }
  if (hangman > 3) { // right arm
    arduboy.drawLine(LEFT+22, 22, LEFT+11, 25);
  }
  if (hangman > 4) { // left leg
    arduboy.drawLine(LEFT+0, 52, LEFT+11, 40);
  }
  if (hangman > 5) { // right leg
    arduboy.drawLine(LEFT+22, 52, LEFT+11, 40);
  }  
}

void drawWord() {
  
#ifdef DEBUG
  arduboy.setCursor(0,0);
  arduboy.print(currentWord); // for easier testing...
#endif
  
  uint8_t x = 0;
  const uint8_t y = 10;
  for (uint8_t chr=0; chr<strlen(currentWord); chr++) {
    char letter = currentWord[chr];
    if (usedLetters[letter-65] == 1) {
      arduboy.setCursor(x, y);
      arduboy.print(letter);
    } else {
      arduboy.drawFastHLine(x-1, y+9, 7);
    }
    x += 9;
  }
}

void drawKeyboard() {
  uint8_t x = 0;
  uint8_t y = 19;
  for (uint8_t chr=0; chr<26; chr++) {
    if (chr % 9 == 0) {
      x = 0;
      y += 12;
    } else {
      x += 9;
    }

    if (cursor == chr && cursorBlink) {
      arduboy.drawFastHLine(x-1, y+8, 7);
    }

    arduboy.setCursor(x, y);
    if (usedLetters[chr] == 0) {
      arduboy.write(chr+65); 
    } else {
      arduboy.write(chr+97); // use lower case for already used letters
    }
  }  
}

void drawCorrect() {
  arduboy.setCursor(0,35);
  arduboy.print(F("YOU GOT IT!"));
  printContinue();
}

void drawDead() {
  arduboy.setCursor(0,35);
  arduboy.print(F("YOU'RE DEAD!"));
  printContinue();
}

void printContinue() {
  arduboy.setCursor(0,64-7);
  arduboy.print(F("Press A to continue."));  
}

void scoreResponse(char letter) {
  bool allDone = true;
  bool letterOk = false;
  for (uint8_t chr=0; chr<strlen(currentWord); chr++) {
    char wordLetter = currentWord[chr];
    if (usedLetters[wordLetter-65] == 0) allDone = false;
    if (wordLetter == letter) {
      score++;
      letterOk = true;
    }
  }
  if (allDone) {
    mode = MODE_CORRECT;
  }
  if (!letterOk) {
    if (score > 0) score--;
    hangman++;    
    if (hangman == 6) {
      mode = MODE_DEAD;
    }
  }
}

void pickAWord() {
  uint16_t newWordIndex;
  uint8_t wordCount;
  uint16_t offset=0;
  
  //arduboy.initRandomSeed();
  randomSeed(millis());
  do {
    newWordIndex = random(WORD_COUNT);
  } while (newWordIndex == previousWordIndex);
  previousWordIndex = newWordIndex;

  for (uint8_t wordLen=2; wordLen<=8; wordLen++) {
    wordCount = pgm_read_byte(WORD_COUNTS + wordLen);
    if (newWordIndex < wordCount) {
      memset(currentWord, 0, sizeof(currentWord));
      offset += (newWordIndex * (uint16_t)wordLen);
      for (uint8_t i=0; i<wordLen; i++, offset++) {
        currentWord[i] = pgm_read_byte(WORDS + offset);
      }
      return;
    }
    newWordIndex -= wordCount;
    offset += ((uint16_t)wordCount * (uint16_t)wordLen);
  }
}

