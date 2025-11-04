

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN   D1        // GPIO5 on WeMos D1
#define BUZZER_PIN D2 
#define NUM_LEDS  12        // change to your strip length

#define MUL_ROW_0 D8
#define MUL_ROW_1 D3
#define MUL_ROW_2 D4

#define MUL_COL_0 D0
#define MUL_COL_1 D7
#define MUL_COL_2 D6
#define MUL_COL_3 D5



const uint8_t ROWS = 3;
const uint8_t COLS = 4;

const uint8_t PixelToLedIndxMap[ROWS][COLS] = {
  {0, 1, 2, 3},
  {7, 6, 5, 4},
  {8, 9, 10, 11}
};

const uint8_t RowPins[ROWS] = {MUL_ROW_0, MUL_ROW_1, MUL_ROW_2};
const uint8_t colPins[COLS] = {MUL_COL_0, MUL_COL_1, MUL_COL_2, MUL_COL_3};

// Use NEO_GRBW for RGBW chips (4 channels per pixel) + NEO_KHZ800 for timing
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint8_t RED[4]   = {255, 0, 0, 0};
const uint8_t GREEN[4] = {0, 255, 0, 0};
const uint8_t BLUE[4]  = {0, 0, 255, 0};
const uint8_t PURPLE[4] = {128, 0, 128, 0};
const uint8_t YELLOW[4] = {255, 255, 0, 0};
const uint8_t CYAN[4]   = {0, 255, 255, 0};
const uint8_t WHITE[4] = {255, 255, 255, 0};

const uint8_t ORANGE[4] = {255, 165, 0, 0};
const uint8_t HOTPINK[4] = {255, 105, 180, 0};
const uint8_t MAGENTA[4] = {255, 0, 255, 0};
const uint8_t VIOLET[4]  = {238, 130, 238, 0};
const uint8_t PINK[4]    = {255, 192, 203, 0};

const uint8_t OFF[4]   = {0, 0, 0, 0};

const uint8_t ColorCount = 6;
const uint8_t* COLORS[ColorCount] = {RED, GREEN, BLUE, PURPLE, YELLOW, WHITE};

const unsigned int TONES[ColorCount] = {261, 329, 392, 523, 659, 784}; // C4, E4, G4, C5, E5, G5, C6

struct Pixel{
  uint8_t m_buttonStateRaw = LOW;
  uint8_t const * m_color = nullptr;
};

Pixel pixels[ROWS][COLS];


void swap(uint8_t &a, uint8_t &b) {
  uint8_t temp = a;
  a = b;
  b = temp;
}

void shuffleArray(uint8_t *array, size_t n) {
  for (size_t i = n - 1; i > 0; i--) {
    size_t j = random(0, i + 1); // pick random index 0..i
    swap(array[i], array[j]);
  }
}


void renderPixelState(){

  // render pixel state to stip


  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      uint8_t idx = PixelToLedIndxMap[r][c];
      if (pixels[r][c].m_color == nullptr) {
        strip.setPixelColor(idx, OFF[0], OFF[1], OFF[2], OFF[3]);
        continue;
      }
      strip.setPixelColor(idx, pixels[r][c].m_color[0], pixels[r][c].m_color[1], pixels[r][c].m_color[2], pixels[r][c].m_color[3]);
    }
  }
  strip.show();
}
static uint32_t ColorToGuess[ROWS][COLS];


unsigned long randomSeed() {
  unsigned long seed = 0;
  for (int i = 0; i < 32; i++) {
    seed ^= (analogRead(A0) & 0x01) << i; // use only least significant bit
    delay(1); // small delay so noise changes
  }
  return seed ^ millis(); // also mix in timer
}


void ClearDisplay() {
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      pixels[r][c].m_color = nullptr;
    }
  }
  renderPixelState();
}
void setup() {
  // Serial.begin(9600);
  // Serial.println();
  // Serial.println("Memory Info:");

  // Serial.printf("Free Sketch space: %d\n", ESP.getFreeSketchSpace());
  // Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  // Serial.printf("Chip ID: %06X\n", ESP.getChipId());
  // Serial.printf("Flash Chip ID: %06X\n", ESP.getFlashChipId());

  // setup buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  
  //setup cols pins to digital input with pull-down resistors
  for (uint8_t c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  // setup rows pins to output
  for (uint8_t r = 0; r < ROWS; r++) {
    pinMode(RowPins[r], OUTPUT);
    digitalWrite(RowPins[r], HIGH); // set all rows to HIGH (not active)
  }

  strip.begin();
  strip.setBrightness(180);
  strip.show();

  pixels[0][0].m_color = RED;
  pixels[0][1].m_color = RED;
  pixels[0][2].m_color = RED;
  pixels[0][3].m_color = RED;

  pixels[1][0].m_color = GREEN;
  pixels[1][1].m_color = GREEN;
  pixels[1][2].m_color = GREEN;
  pixels[1][3].m_color = GREEN;

  pixels[2][0].m_color = BLUE;
  pixels[2][1].m_color = BLUE;
  pixels[2][2].m_color = BLUE;
  pixels[2][3].m_color = BLUE;

  renderPixelState();

  // clear all pixels
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      pixels[r][c].m_color = nullptr;
      pixels[r][c].m_buttonStateRaw = LOW;
    }
  }
  // Init game
  // randomize seed
  int seed = randomSeed();
  randomSeed(seed);
  

  uint8_t colorIndexes [2*ColorCount];
  for (uint8_t i = 0; i < 2*ColorCount; i++) {
    colorIndexes[i] = i;
  }
  shuffleArray(colorIndexes, 2*ColorCount);

  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      const uint8_t pixelIndex = r*COLS + c;
      uint8_t colorIndex = colorIndexes[pixelIndex] % ColorCount;
      ColorToGuess[r][c] = colorIndex;
    }
  }

  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
        pixels[r][c].m_color = COLORS[ColorToGuess[r][c]];
        tone(BUZZER_PIN, TONES[ColorToGuess[r][c]], 150);
        renderPixelState();
        delay(100);
    }
  }
  delay(500);
  ClearDisplay();


}

struct RowCol {
  uint8_t row = 255;
  uint8_t col = 255;
  void reset() {
    row = 255;
    col = 255;
  }
  bool isSet() const {
    return row != 255 && col != 255;
  }
};

void BlockingTone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  // update the display
  renderPixelState();
  delay(10);
  tone(pin, frequency);
  delay(duration + 20);
  noTone(pin);
}

Pixel& getPixel(const RowCol &rc) {
  return pixels[rc.row][rc.col];
}


const unsigned int animationWinSteps [12][ROWS][COLS] = {
  {
    {1, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
   {
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
   {
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
   },
   {
    {1, 1, 1, 1},
    {1, 1, 0, 0},
    {0, 0, 0, 0}
   },
   {
    {1, 1, 1, 1},
    {1, 1, 1, 0},
    {0, 0, 0, 0}
   },
   {
    {1, 1, 1, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 0}
   },
   {
    {1, 1, 1, 1},
    {1, 1, 1, 1},
    {1, 0, 0, 0}
   },
   {
    {1, 1, 1, 1},
    {1, 1, 1, 1},
    {1, 1 ,0 ,0}
   },
   {
    {1 ,1 ,1 ,1},
    {1 ,1 ,1 ,1},
    {1 ,1 ,1 ,0}
   },
   {
    {1 ,1 ,1 ,1},
    {1 ,1 ,1 ,1},
    {1 ,1 ,1 ,1}
   }
};

void DisplayStep(const unsigned int step[ROWS][COLS]) {
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      if (step[r][c] == 1) {
        pixels[r][c].m_color = COLORS[ColorToGuess[r][c]];
      } else {
        pixels[r][c].m_color = nullptr;
      }
    }
  }
  renderPixelState();
}

void WinAnim(){
  ClearDisplay(); 
  // play melody

  const unsigned int melody[12] = {523, 659, 784, 1046, 784, 659, 523, 392, 523, 659, 784, 1046}; // C5 E5 G5 C6 G5 E5 C5 G4 C5 E5 G5 C6
  const unsigned int noteDurations[12] = {150, 150, 150, 300, 150, 150, 300, 150, 150, 150, 150, 300};

  for (uint8_t i = 0; i < 12; i++) {
    DisplayStep(animationWinSteps[i]);
    BlockingTone(BUZZER_PIN, melody[i], noteDurations[i]);
  }
  ClearDisplay();
  setup();

}

void OnButtonPress(uint8_t row, uint8_t col) {
  static RowCol firstPick;
  static RowCol secondPick;
  if (pixels[row][col].m_color != nullptr) {
    // already revealed
    return;
  }
  
  // FIRST PICK
  if (!firstPick.isSet() && !secondPick.isSet()) {
    // both picks are set, reset
   firstPick = {row, col};
   getPixel(firstPick).m_color = COLORS[ColorToGuess[row][col]];
   BlockingTone(BUZZER_PIN, TONES[ColorToGuess[row][col]], 100);

   return;
  }

  // SECOND PICK
  if (firstPick.isSet() && !secondPick.isSet()) {
    if (firstPick.row == row && firstPick.col == col) {
      // same pixel, ignore
      return;
    }
    secondPick = {row, col};
    getPixel(secondPick).m_color = COLORS[ColorToGuess[row][col]];
 
    BlockingTone(BUZZER_PIN, TONES[ColorToGuess[row][col]], 100);
    // check for match
    if (ColorToGuess[firstPick.row][firstPick.col] == ColorToGuess[secondPick.row][secondPick.col]) {
  
      // match, leave revealed
      firstPick.reset();
      secondPick.reset();
      if (std::all_of(&pixels[0][0], &pixels[0][0] + ROWS*COLS, [](const Pixel &p){ return p.m_color != nullptr; })) {
        // all pixels revealed, win
        WinAnim();
        WinAnim();
        WinAnim();
      }
  
    } else {
      // no match, hide after delay
      delay(1000);
      getPixel(firstPick).m_color = nullptr;
      getPixel(secondPick).m_color = nullptr;
      firstPick.reset();
      secondPick.reset();
    }
    renderPixelState();
  }

  // check for matches

}

void loop() {
  static uint8_t currentRow = 2;
  for (uint8_t r = 0; r < ROWS; r++) {
    digitalWrite(RowPins[r], (r == currentRow) ? LOW : HIGH);
  }

  // read columns
  for (uint8_t c = 0; c < COLS; c++) {
    uint8_t buttonState = digitalRead(colPins[c]);
   
    if (buttonState != pixels[currentRow][c].m_buttonStateRaw) {
      // button state changed
      pixels[currentRow][c].m_buttonStateRaw = buttonState;
      if (buttonState == LOW) {
        OnButtonPress(currentRow, c);
        
        //pixels[currentRow][c].m_color = WHITE;
      }
    }
  }
  //Serial.printf("Row %d c1 : %d c2 : %d c3 : %d c4 : %d\n", currentRow, digitalRead(colPins[0]), digitalRead(colPins[1]), digitalRead(colPins[2]), digitalRead(colPins[3]));
  currentRow = (currentRow + 1) % ROWS;

  renderPixelState();

  

}