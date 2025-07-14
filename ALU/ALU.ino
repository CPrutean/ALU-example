#include <LCD_I2C.h>
#include <string.h>

#define zeroButtonRead 4
#define oneButtonRead 2

#define bool char
#define true 1
#define false 0

const int buttonPresses[] = {1, 1, 16, 16, 1, 1, 1, 1, 1, 1};
static char* numInput[10];
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
    numInput[i] = (char*)malloc(sizeof(char)*buttonPresses[i]+1);
  }
  updateDisplay();
}
int phase = 0;
int phaseInd = 0;

void updatePhaseString(int buttonPress) {

  char temp = (char) (buttonPress+(int)'0');
  *(numInput[phase]+(buttonPresses[phase]-phaseInd-1)) = temp;
  lcd.print(temp);
  phaseInd++;
  lcd.setCursor(15-phaseInd, 1);

  if (phaseInd >= buttonPresses[phase]) {
    phase++;
    phaseInd = 0;
    delay(1000);
    updateDisplay();
  }
  if (phase >= 10) {
    calculations();
  }
}

void calculations() {
  int i;
  int xy[2][16];
  int oper[6];
  for (i = 0; i < 6 i++) {
    oper[i] = (int)*(numInput[4+i])-(int)'0';
  }
  char* operName[] = {"ox", "oy", "zx", "nx", "zy", "ny", "f", "N"};

  
  for (i = 0; i < 16; i++) {
    xy[0][15-i] = (int)*(numInput[2]+15-i)-(int)'0';
    xy[1][15-i] = (int)*(numInput[3]+15-i)-(int)'0';
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
  int k;
  int* final = (int*)malloc(sizeof(int)*16);
  int indOfOper;
  for (i = 0; i < length; i++) {
    lcd.setCursor (0, 0);
    lcd.print(display[i]);
    lcd.setCursor(0, 1);
    if (i < 7) {
      indOfOper = (int)*(operName+1)-(int)'x';
    }
    int lcdCursor = 0;

    for (j = 0; j < 16; j++) {
      if (*(operName[i]) == 'z' && oper[i+2] == 1) {
        xy[indOfOper][j] = 0;
      } else if (*(operName[i]) == 'n' && oper[i+2] == 1) {
        xy[indOfOper][j] = Not(xy[indOfOper][j]);
      } else if (*(operName[i]) == 'f') {
        addOrAnd(xy, oper[i], final);
      } else if (*(operName[i]) == 'N' && oper[i+2] == 1) {
        *(final+j) = Not(*(final+j));
        lcd.print(*(final+j));
      } else if (*(operName[i]) == 'N' && oper[i+2] == 0) {
        lcd.print(*(final+j));
      }
      lcd.print(xy[indOfOper][j]);
      lcd.setCursor(++lcdCursor, 1);
    }
    delay(2000);
    lcd.clear();
  }
}
int Not(int i) {
  return i==0;
}
//In the spirit of this project the logic for adding the bits together are going to be done with the same logic used for the chip
void addOrAnd(int inputArr[2][16], int sel, int* finalPtr) {
  int i;
  if (sel == 0) {
    for (i = 0; i < 16; i++) {
      *(finalPtr+i) = inputArr[0][i] == 1 && inputArr[1][i] == 1;
    }
  } else {
    int carry = inputArr[0][0] && inputArr[1][0];
    *(finalPtr) = Xor(inputArr[0][0], inputArr[1][0]);

    for (i = 1; i < 16; i++) {
      *(finalPtr+i) = Xor(Xor(inputArr[0][i], inputArr[1][i]), carry);
      carry = inputArr[0][i] && inputArr[1][i] && carry;
    }
  }
}



int Xor(int i, int j) {
  return i != j;
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