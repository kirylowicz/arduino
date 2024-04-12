int DATA[2];
int LATCH[2];
int CLOCK[2];
int BUTTON[2];

int PLAYER_1 = 0;
int PLAYER_2 = 1;

bool hasCooldown = false;
bool isPressed[2];
byte dataArray[10];
int points[2];

void setup() {
  ////Pin connected to DS of 74HC595
  DATA[PLAYER_1] = 3;
  //Pin connected to ST_CP of 74HC595
  LATCH[PLAYER_1] = 2;
  //Pin connected to SH_CP of 74HC595
  CLOCK[PLAYER_1] = 1;
  // Pin connected to button
  BUTTON[PLAYER_1] = 4;

  ////Pin connected to DS of 74HC595
  DATA[PLAYER_2] = 16;
  //Pin connected to ST_CP of 74HC595
  LATCH[PLAYER_2] = 17;
  //Pin connected to SH_CP of 74HC595
  CLOCK[PLAYER_2] = 18;
  // Pin connected to button
  BUTTON[PLAYER_2] = 15;

  dataArray[0] = 0x00; //0b00000000
  dataArray[1] = 0x80; //0b10000000
  dataArray[2] = 0xC0; //0b11000000
  dataArray[3] = 0xE0; //0b11100000
  dataArray[4] = 0xF0; //0b11110000
  dataArray[5] = 0xF8; //0b11111000
  dataArray[6] = 0xFC; //0b11111100
  dataArray[7] = 0xFE; //0b11111110
  // dataArray[8] = 0xFF; //0b11111111

  isPressed[PLAYER_1] = false;
  isPressed[PLAYER_2] = false;

  points[PLAYER_1] = 0;
  points[PLAYER_2] = 0;

  for (int i=0; i<2; i++) {
  //set pins to output so you can control the shift register
    pinMode(LATCH[i], OUTPUT);
    pinMode(CLOCK[i], OUTPUT);
    pinMode(DATA[i], OUTPUT);
    pinMode(BUTTON[i], INPUT_PULLUP);
  }
}

void readButton(int color) {
  bool isNowPressed = digitalRead(BUTTON[color]) == LOW;

  if (isPressed[color] == isNowPressed) {
    return;
  }

  isPressed[color] = isNowPressed;

  if (isPressed[color]) {
    points[color]++;
    hasCooldown = true;
  }
  
  if (points[color] > 7) {
    points[color] = 0;
  }
}

void display(int color, int value) {
    // turn off the output so the pins don't light up
    // while you're shifting bits:
    digitalWrite(LATCH[color], LOW);
    // shift the bits out:
    shiftOut(DATA[color], CLOCK[color], MSBFIRST, dataArray[value]);
    // turn on the output so the LEDs can light up:
    digitalWrite(LATCH[color], HIGH);
}

void celebrate(int winner, int looser) {
  // turn off looser display
  for (int i = points[looser] - 1; i >= 0; i--) {
    display(winner, 7);
    delay(350);
    display(looser, i);
    display(winner, 0);
    delay(50);
  }

  if (points[looser] == 0) {
    display(winner, 0);
    delay(50);
  }
  delay(750);

  // Flash winner display
  for (int i = 0; i < 7; i++) {
    display(winner, 7);

    if (i == 6) {
      delay(800);
    } else {
      delay(50);
    }

    display(winner, 0);

    if (i == 4) {
      delay(400);
    } else {
      delay(150);
    }
  }
}

void loop() {
  readButton(PLAYER_1);
  readButton(PLAYER_2);

  display(PLAYER_1, points[PLAYER_1]);
  display(PLAYER_2, points[PLAYER_2]);

  if (hasCooldown) {
    hasCooldown = false;
    delay(1500);
  }

  if (points[PLAYER_1] == 7 || points[PLAYER_2] == 7) {
    int winner = points[PLAYER_1] > points[PLAYER_2] ? PLAYER_1 : PLAYER_2;
    int looser = points[PLAYER_1] < points[PLAYER_2] ? PLAYER_1 : PLAYER_2;
    celebrate(winner, looser);
    points[PLAYER_1] = 0;
    points[PLAYER_2] = 0;
  }
}