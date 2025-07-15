#include <LCD_I2C.h>
#include <string.h>

#define zeroButtonRead 4
#define oneButtonRead 2

#define bool char
#define true 1
#define false 0

const int buttonPresses[] = {1, 1, 16, 16, 1, 1, 1, 1, 1, 1};
static int* numInput[10];
static int arrFinal[16];
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
  Serial.begin(9600);
  pinMode(zeroButtonRead, INPUT);
  pinMode(oneButtonRead, INPUT);
  lcd.begin();
  lcd.backlight();
  int i;
  for (i = 0; i < 9; i++) {
    numInput[i] = (int*)malloc(sizeof(int)*buttonPresses[i]+1);
  }
  updateDisplay();
}
int phase = 0;
int phaseInd = 0;

void updatePhaseString(int buttonPress) {

  *(numInput[phase]+(buttonPresses[phase]-phaseInd-1)) = buttonPress;
  lcd.print(buttonPress);
  lcd.setCursor(15- ++phaseInd, 1);

  if (phaseInd >= buttonPresses[phase]) {
    phase++;
    phaseInd = 0;
    delay(1000);
    updateDisplay();
  }
  if (phase >= 9) {
    calculations();
  }
}

void addOrAnd(int arr[2][16], int sel) {

  int i;
  for (i = 0; i < 16; i++ ) {
    arrFinal[i] = 0;
  }
  if (sel == 0) {
    for(i = 0; i < 16; i++) {
      if (arr[0][i] == 1 && arr[1][i] == 1) {
        arrFinal[i] = 1;
      } else {
        arrFinal[i] = 0;
      }
    }
  } else {
    int carry;
    int tempXor;
    int tempAnd;
    if (arr[0][0] != arr[1][0]) {
      arrFinal[i] = 1;
    }
    tempAnd = arr[0][0] && arr[1][0];
    for (i = 1; i < 16; i++) {
      tempXor = arr[0][i] != arr[1][i];
      arrFinal[i] = tempXor != carry;
      carry = arr[0][i] && arr[1][i] && carry;
    }
  }
}

void calculations() {
  int i;
  int xy[2][16];
  int oper[6];
  for (i = 0; i < 6; i++) {
    oper[i] = *(numInput[4+i]);
  }
  char* operName[] = {"ox", "oy", "zx", "nx", "zy", "ny", "f", "N"};
  
  
  for (i = 0; i < 16; i++) {
    xy[0][15-i] = *(numInput[2]+15-i);
    xy[1][15-i] = *(numInput[3]+15-i);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calculating");
  lcd.setCursor(11, 0);
  int j;
  int cursor = strlen("Calculating");
  for (i=0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      lcd.print(".");
      lcd.setCursor(++cursor, 0);
      delay(250);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calculating");
    cursor = strlen("Calculating");
    lcd.setCursor(cursor, 0);
  }

  char* display[] = {"Original X:", "Original Y:", "Zero X:", "Not X:", "Zero Y:", "Not Y:", "Add or And:", "Not output:"};
  int length = sizeof(display)/sizeof(display[0]);
  int bitLength = sizeof(xy[0])/sizeof(xy[0][0]);
  int ind;
  int cursInd;
  for (i = 0; i < length-2; i++) {
    cursInd = 0;
    lcd.setCursor(0, 0);
    lcd.print(display[i]);
    lcd.setCursor(0, 1);
    if (i < 7) {
      ind = (int)*(operName[i]+1)-(int)'x';
    }
    for (j = 0; j < 16; j++) {
      if (*(operName[i]) == 'z' && oper[i-2] == 1) {
        xy[ind][j] = 0;
      } else if (*(operName[i]) == 'n' && oper[i-2] == 1) {
        if (xy[ind][j] == 1) {
          xy[ind][j] = 0;
        } else {
          xy[ind][j] = 1;
        }
      } else if (*(operName[i]) == 'f') {
        addOrAnd(xy, oper[i-2]);
        break;
      }
      lcd.setCursor(j, 1);
      lcd.print(xy[ind][j]);
    }
    delay(1000);
    lcd.clear();
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Final display");
  for (i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.print(final[i]);
  }
  free(display);
  free(operName);
  free(oper);
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
  lcd.clear();
  lcd.setCursor(0, 0);
  int i = 0;
  int length = 0;

  while (*(phases[phase]+i) != '\0') {
    i++;
    length++;
  }

  if (length > 16) {
    int temp = 0;
    int currSpaceIndex = 0;
    for (i = 0; i < length; i++) {
      if (*(phases[phase]+i) == ' ') {
        temp = currSpaceIndex;
        currSpaceIndex = i;
      } else if (currSpaceIndex>16) {
        currSpaceIndex = temp;
        break;
      }
    }
    char* tempString1 = (char*)malloc(sizeof(char)*currSpaceIndex);
    char* tempString2 = (char*)malloc(sizeof(char)*(length-currSpaceIndex));
    for (i = 0; i < currSpaceIndex; i++) {
      *(tempString1+i) = *(phases[phase]+i); 
    }
    *(tempString1+currSpaceIndex) = '\0';
    for (i = 0; i < (length-currSpaceIndex); i++) {
      *(tempString2+i) = *(phases[phase]+(i+currSpaceIndex+1));
    }
    *(tempString2+length) = '\0';

     
    lcd.print(tempString1);
    lcd.setCursor(0, 1);
    lcd.print(tempString2);
    lcd.setCursor(15, 1);
    free(tempString1);
    free(tempString2);
  } else {
    lcd.print(phases[phase]);
  }
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