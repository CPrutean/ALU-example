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
    finalCalculations();
  }
}

void finalCalculations() {
  bool x[16];
  bool y[16];
  bool finalOutput[16];
  int i;
  for (i = 0; i < 16; i++) {
    x[i] = (bool) *(numInput[2]+i);
    y[i] = (bool) *(numInput[3]+i);
  }
  
  char* outputDisplay[] = {"Original X", "Original Y", "Zero X", "Not X", "Zero Y", "Not Y", "Add or And", "Negate output"};
  char* output[] = {"ox", "oy", "zx", "nx", "zy", "ny", "ff", "nf"};
  int length = sizeof(outputDisplay)/sizeof(outputDisplay[0]);
  bool opers[6];
  for (i = 0; i < 6; i++) {
    opers[i] = (bool) *(numInput[i+3]);
  }

  bool** arrPtr = (bool**)malloc(sizeof(bool*)*3);
  *(arrPtr) = x;
  *(arrPtr+1) = y;
  *(arrPtr+2) = finalOutput;
  int j;
  int arrInd = 0;
  for (i = 0; i < length; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(outputDisplay[i]);
    lcd.setCursor(0, 1);
    //Indexes 0 and 1 are printing the original inputs
    //Indexes 2 and 3 are for manipulating x
    //Indexes 3 and 4 are for manipulating y
    //Indexes 5 and 6 are for manipulating the final outputs
    if (*(output[i]+1) == 'x') {
      arrInd = 0;
    } else if (*(output[i]+1) == 'y') {
      arrInd = 1;
    } else {
      arrInd = 2;
    }
    bool carry = false;
    for (j = 0; j < 16; j++) {
      if (*(output[i]) == 'n' && opers[i-2]) {
        *(*(arrPtr+arrInd)+j) = ! *(*(arrPtr+arrInd)+j);
      } else if (*(output[i]) == 'z' && opers[i-2]) {
        *(*(arrPtr+arrInd)+j) = false;
      } else if (*(output[i]) == 'f' && opers[4] == false) {
        finalOutput[j] = x[j] && y[j];
      } else if (*(output[i]) == 'f' && opers[4] == true) {
        carry = adder(carry, x[j], y[j], *(arrPtr+arrInd), j);
      }
      lcd.print((int) *(*(arrPtr+arrInd)+j));
    }
    delay(2000);
  }
}

//Will set the value at the index for the final array and return the carry bit
bool adder(bool bit1, bool bit2, bool bit3, bool* value, int ind) {
  bool tempBool = bit1!=bit2;
  *(value+ind) = tempBool != bit3;
  return bit1 && bit2 && bit3;
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