#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {30, 31, 32, 33}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {34, 35, 36, 37}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


const int switchPins[] = {2, 3, 4, 5, 6};  // Pins connected to switches
bool machineStates[5] = {false, false, false, false, false};
const int irSensorPin = 7;
const int trigPin = 8;
const int echoPin = 9;
bool isOperator = false;


bool mode = false;  // false: Manual, true: Automatic
char currentUserRole = 'U'; // 'U': User, 'O': Operator

// Function Prototypes
void setupMachine1();
void controlMachine1(bool state);
void setupMachine2();
void controlMachine2(bool state);
void setupMachine3();
void controlMachine3(bool state);
void setupFeeder();
void controlFeeder(bool state);
void setupElectrochemical();
void controlElectrochemical(bool state);

void showStartupScreen();
void showLoginScreen();
void showMainScreen();
void handleKeypadInput();
void controlAutomaticMode();
void controlManualMode();
void checkSafetySensors();
void stopAllSystems();
void sendDataToESP();
void updateDisplay();

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  setupMachine1();
  setupMachine2();
  setupMachine3();
  setupFeeder();
  setupElectrochemical();

  pinMode(irSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  showStartupScreen();
}

void loop() {
  handleKeypadInput();
  checkSafetySensors();
  sendDataToESP();
  updateDisplay();
  if (mode) {
    controlAutomaticMode();
  } else {
    controlManualMode();
  }
}

void setupMachine1() {
  pinMode(switchPins[0], OUTPUT);
  controlMachine1(false);
}

void controlMachine1(bool state) {
  machineStates[0] = state;
  digitalWrite(switchPins[0], state ? HIGH : LOW);
}

// Similar setup and control functions for other machines...

void setupMachine2() {
  pinMode(switchPins[1], OUTPUT);
  controlMachine2(false);
}

void controlMachine2(bool state) {
  machineStates[1] = state;
  digitalWrite(switchPins[1], state ? HIGH : LOW);
}

void setupMachine3() {
  pinMode(switchPins[2], OUTPUT);
  controlMachine3(false);
}

void controlMachine3(bool state) {
  machineStates[2] = state;
  digitalWrite(switchPins[2], state ? HIGH : LOW);
}

void setupFeeder() {
  pinMode(switchPins[3], OUTPUT);
  controlFeeder(false);
}

void controlFeeder(bool state) {
  machineStates[3] = state;
  digitalWrite(switchPins[3], state ? HIGH : LOW);
}

void setupElectrochemical() {
  pinMode(switchPins[4], OUTPUT);
  controlElectrochemical(false);
}

void controlElectrochemical(bool state) {
  machineStates[4] = state;
  digitalWrite(switchPins[4], state ? HIGH : LOW);
}

void showStartupScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Select Mode:"));
  display.println(F("1. Automatic"));
  display.println(F("2. Manual"));
  display.display();
}

void showLoginScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Enter ID:"));
  display.display();
  
  String enteredID = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      enteredID += key;
      display.print(key);
      display.display();
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Enter PW:"));
  display.display();
  
  String enteredPW = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      enteredPW += key;
      display.print('*');
      display.display();
    }
  }

  if ((enteredID == "123" && enteredPW == "123") || (enteredID == "789" && enteredPW == "789")) {
    isOperator = (enteredID == "123");
    showMainScreen();
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("Invalid ID/PW"));
    display.display();
    delay(2000);
    showLoginScreen();
  }
}

void showMainScreen() {
  display.clearDisplay();
  if (mode) {
    display.println(F("Mode: Automatic"));
  } else {
    display.println(F("Mode: Manual"));
  }
  if (isOperator) {
    display.println(F("Role: Operator"));
  } else {
    display.println(F("Role: User"));
  }
  display.display();
}

void handleKeypadInput() {
  char key = keypad.getKey();
  if (key) {
    if (key == '1') {
      mode = true;
      showLoginScreen();
    } else if (key == '2') {
      mode = false;
      showLoginScreen();
    }
  }
}

void controlAutomaticMode() {
  static unsigned long previousMillis = 0;
  const long interval = 60000; // 1 minute interval
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (!machineStates[0] && !machineStates[1]) {
      controlMachine1(true);
    } else if (machineStates[0] && !machineStates[1]) {
      controlMachine1(false);
      controlMachine2(true);
    } else if (machineStates[1] && !machineStates[2]) {
      controlMachine2(false);
      controlMachine3(true);
    } else if (machineStates[2] && !machineStates[3]) {
      controlMachine3(false);
      controlFeeder(true);
    } else if (machineStates[3] && !machineStates[4]) {
      controlFeeder(false);
      controlElectrochemical(true);
    } else {
      stopAllSystems();
    }
  }
}

void controlManualMode() {
  if (digitalRead(switchPins[0]) == LOW) {
    controlMachine1(!machineStates[0]);
  }
  if (digitalRead(switchPins[1]) == LOW) {
    controlMachine2(!machineStates[1]);
  }
  if (digitalRead(switchPins[2]) == LOW) {
    controlMachine3(!machineStates[2]);
  }
  if (digitalRead(switchPins[3]) == LOW) {
    controlFeeder(!machineStates[3]);
  }
  if (digitalRead(switchPins[4]) == LOW) {
    controlElectrochemical(!machineStates[4]);
  }
}

void checkSafetySensors() {
  if (digitalRead(irSensorPin) == HIGH) {
    stopAllSystems();
    delay(5000); // Pause for 5 seconds
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  if (distance < 15) {
    stopAllSystems();
  }
}

void stopAllSystems() {
  for (int i = 0; i < 5; i++) {
    controlMachine1(false);
    controlMachine2(false);
    controlMachine3(false);
    controlFeeder(false);
    controlElectrochemical(false);
  }
}

void sendDataToESP() {
  // Send the status of each machine and the mode to the ESP8266
  for (int i = 0; i < 5; i++) {
    Serial1.print("M");
    Serial1.print(i);
    Serial1.print(":");
    Serial1.println(machineStates[i] ? "1" : "0");
  }
  Serial1.print("Mode:");
  Serial1.println(mode ? "1" : "0");
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("System Status:"));
  for (int i = 0; i < 5; i++) {
    display.print(F("Machine"));
    display.print(i + 1);
    display.print(F(": "));
    display.println(machineStates[i] ? F("ON") : F("OFF"));
  }
  display.display();
}
