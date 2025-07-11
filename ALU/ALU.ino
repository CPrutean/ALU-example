#include <LCD_I2C.h>
#define zeroButtonRead 4
#define oneButtonRead 2

#define bool char
#define true 1
#define false 0

const int buttonPresses[] = {1, 16, 16, 1, 1, 1, 1, 1, 1};
static char* numInput[9];
const char* phases[] = {
  "Welcome to the hack ALU demo",
  "Press any button to continue",
  "Enter x input",
  "Enter y input",
  "Zero x input?",
  "Not x input?",
  "Zero y input?",
  "Not y input?",
  "Add or and the values (1/0)",
  "Negate the output?"
};


LCD_I2C lcd(0x27, 16, 2);
void setup() {

  pinMode(zeroButtonRead, INPUT);
  pinMode(oneButtonRead, INPUT);
  lcd.begin();
  lcd.backlight();
  int i;
  for (i = 0; i < 9; i++) {
    numInput[i] = (char*)malloc(sizeof(char)*buttonPresses[i]+1);
  }
  updateDisplay();
}
int phase = 0;
int phaseInd = 0;

void updatePhaseString(int buttonPress) {
  char temp = (char)(buttonPress+(int)'0')
  *(numInput+phaseInd) = temp;
  lcd.print(temp);
  phaseind++;
  lcd.setCursor(15-phaseind, 1);

  if (phaseind >= buttonPresses[phase]) {
    phase++;
    updateDisplay();
  }

}

void isButtonPressed(int button1, int button2) {
  static int temp1;
  static int temp2;
  int num;
  if (temp1 == LOW && button1 == HIGH) {
    num = 0;
    updatePhaseString(num);
  } else if (temp2 == LOW && button2 == HIGH) {
    num = 1;
    updatePhaseString(num);
  } else {
    num = -1;
  }

  temp1 = button1;
  temp2 = button2;

}


void updateDisplay() {
  int temp = 0;
  int i;
  lcd.print(phases[phase]);
  for (i=0; i < buttonPresses[phase]; i++) {
    *(numInput[phase]+i) = '-';
  }
  lcd.setCursor(15, 1);
}


int zeroButton;
int oneButton;

void loop() {

  zeroButton = digitalRead(zeroButtonRead);
  oneButton = digitalRead(oneButtonRead);
  isButtonPressed(zeroButton, oneButton);
  

}