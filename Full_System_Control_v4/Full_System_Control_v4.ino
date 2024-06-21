#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// GLCD setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
const int relayPin1 = 2;
const int ledPin1 = 3;
const int switchPin1 = 4;
const int ultrasonicTriggerPin1 = 22;
const int ultrasonicEchoPin1 = 23;
const int relayPin2 = 5;
const int ledPin2 = 6;
const int switchPin2 = 7;
const int stepPin3 = 8;
const int dirPin3 = 9;
const int enablePin3 = 10;
const int ledPin3 = 11;
const int switchPin3 = 12;
const int feederMotor1Pin = 13;
const int feederMotor2Pin = 14;
const int feederLedPin = 15;
const int feederSwitchPin = 16;
const int electroHeaterPin = 17;
const int electroPumpPin = 18;
const int electroLedPin = 19;
const int electroSwitchPin = 20;
const int tempSensorPin = 21;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {23, 25, 27, 29};
byte colPins[COLS] = {31, 33, 35, 37};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// States and modes
enum Screen { WELCOME, MAIN_MENU, MANUAL_OPERATION, AUTOMATIC_OPERATION, USER_MANUAL_LOGIN, OPERATOR_MANUAL_LOGIN, USER_AUTOMATIC_LOGIN, OPERATOR_AUTOMATIC_LOGIN, USER_MANUAL_CONTROL, OPERATOR_MANUAL_CONTROL, USER_AUTOMATIC_CONTROL, OPERATOR_AUTOMATIC_CONTROL };
Screen currentScreen = WELCOME;

bool machine1State = false;
bool machine2State = false;
bool machine3State = false;
bool feederState = false;
bool electroState = false;
volatile bool keepRunning = false;  // Global variable to control the stepper motor
int feederSpeed = 650; // RPM
unsigned long previousMillis = 0;
const long interval = 60000; // 1 minute interval

String inputID = "";
String inputPassword = "";
int loginAttempts = 0;

// User credentials
const String userManualID = "1212";
const String userManualPassword = "1212";
const String operatorManualID = "3434";
const String operatorManualPassword = "3434";
const String userAutomaticID = "5656";
const String userAutomaticPassword = "5656";
const String operatorAutomaticID = "7878";
const String operatorAutomaticPassword = "7878";

void setup() {
  Serial.begin(9600); // Arduino Mega Serial
  Serial1.begin(9600); // Communication with ESP8266
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Initializing..."));
  display.display();
  delay(2000);

  setupMachine1();
  setupMachine2();
  setupMachine3();
  setupVibratoryFeeder();
  setupElectrochemicalSystem();
 // setupKeypad();
  showWelcomeScreen();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
  switch (currentScreen) {
    case WELCOME:
      // Do nothing, wait for welcome screen timeout
      break;
    case MAIN_MENU:
      showMainMenu();
      break;
    case MANUAL_OPERATION:
      showManualOperationScreen();
      break;
    case AUTOMATIC_OPERATION:
      showAutomaticOperationScreen();
      break;
    case USER_MANUAL_LOGIN:
    case OPERATOR_MANUAL_LOGIN:
    case USER_AUTOMATIC_LOGIN:
    case OPERATOR_AUTOMATIC_LOGIN:
      handleLogin();
      break;
    case USER_MANUAL_CONTROL:
      userManualControl();
      break;
    case OPERATOR_MANUAL_CONTROL:
      operatorManualControl();
      break;
    case USER_AUTOMATIC_CONTROL:
      userAutomaticControl();
      break;
    case OPERATOR_AUTOMATIC_CONTROL:
      operatorAutomaticControl();
      break;
  }

  controlMachine1();
  controlMachine2();
  controlMachine3();
  controlVibratoryFeeder();
  controlElectrochemicalSystem();
  timedOperation();
  updateDisplay();
  sendDataToESP();

  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    handleESPCommand(data);
  }
}

void setupMachine1() {
  pinMode(relayPin1, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(ultrasonicTriggerPin1, OUTPUT);
  pinMode(ultrasonicEchoPin1, INPUT);
  digitalWrite(relayPin1, LOW);
  digitalWrite(ledPin1, LOW);
}

void setupMachine2() {
  pinMode(relayPin2, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(switchPin2, INPUT_PULLUP);
  digitalWrite(relayPin2, LOW);
  digitalWrite(ledPin2, LOW);
}

void setupMachine3() {
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(enablePin3, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(switchPin3, INPUT_PULLUP);
  digitalWrite(enablePin3, LOW);
  digitalWrite(ledPin3, LOW);
}

void setupVibratoryFeeder() {
  pinMode(feederMotor1Pin, OUTPUT);
  pinMode(feederMotor2Pin, OUTPUT);
  pinMode(feederLedPin, OUTPUT);
  pinMode(feederSwitchPin, INPUT_PULLUP);
  digitalWrite(feederMotor1Pin, LOW);
  digitalWrite(feederMotor2Pin, LOW);
  digitalWrite(feederLedPin, LOW);
}

void setupElectrochemicalSystem() {
  pinMode(electroHeaterPin, OUTPUT);
  pinMode(electroPumpPin, OUTPUT);
  pinMode(electroLedPin, OUTPUT);
  pinMode(electroSwitchPin, INPUT_PULLUP);
  pinMode(tempSensorPin, INPUT);
  digitalWrite(electroHeaterPin, LOW);
  digitalWrite(electroPumpPin, LOW);
  digitalWrite(electroLedPin, LOW);
}

void controlMachine1() {
  if (digitalRead(switchPin1) == LOW) {
    if (getUltrasonicDistance() > 15) { // Safety check with ultrasonic sensor
      machine1State = true;
      digitalWrite(relayPin1, HIGH);
      digitalWrite(ledPin1, HIGH);
    }
  } else {
    machine1State = false;
    digitalWrite(relayPin1, LOW);
    digitalWrite(ledPin1, LOW);
  }
}

void controlMachine2() {
  if (digitalRead(switchPin2) == LOW) {
    machine2State = true;
    digitalWrite(relayPin2, HIGH);
    digitalWrite(ledPin2, HIGH);
  } else {
    machine2State = false;
    digitalWrite(relayPin2, LOW);
    digitalWrite(ledPin2, LOW);
  }
}

void controlMachine3() {
  if (digitalRead(switchPin3) == LOW) {
    machine3State = true;
    digitalWrite(ledPin3, HIGH);
    keepRunning = true;
    stepperMove(1, 375); // Adjusted for 160 RPM
  } else {
    machine3State = false;
    digitalWrite(ledPin3, LOW);
    stopMotor();
  }
}

void controlVibratoryFeeder() {
  if (digitalRead(feederSwitchPin) == LOW) {
    feederState = true;
    analogWrite(feederMotor1Pin, feederSpeed);
    analogWrite(feederMotor2Pin, feederSpeed);
    digitalWrite(feederLedPin, HIGH);
  } else {
    feederState = false;
    digitalWrite(feederMotor1Pin, LOW);
    digitalWrite(feederMotor2Pin, LOW);
    digitalWrite(feederLedPin, LOW);
  }
}

void controlElectrochemicalSystem() {
  if (digitalRead(electroSwitchPin) == LOW) {
    electroState = true;
    if (getTemperature() < 90) {
      digitalWrite(electroHeaterPin, HIGH);
    } else {
      digitalWrite(electroHeaterPin, LOW);
    }
    digitalWrite(electroPumpPin, HIGH);
    digitalWrite(electroLedPin, HIGH);
  } else {
    electroState = false;
    digitalWrite(electroHeaterPin, LOW);
    digitalWrite(electroPumpPin, LOW);
    digitalWrite(electroLedPin, LOW);
  }
}

void stepperMove(int direction, int speed) {
  digitalWrite(enablePin3, HIGH);
  digitalWrite(dirPin3, direction);
  for (int i = 0; i < speed; i++) {
    digitalWrite(stepPin3, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin3, LOW);
    delayMicroseconds(1000);
  }
  digitalWrite(enablePin3, LOW);
}

void stopMotor() {
  digitalWrite(enablePin3, LOW);
}

void showWelcomeScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print(F("Welcome to"));
  display.setCursor(0, 16);
  display.print(F("FAPRL"));
  display.display();
  delay(3000);
  currentScreen = MAIN_MENU;
}

void showMainMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("1. Manual Operation"));
  display.setCursor(0, 10);
  display.print(F("2. Automatic Operation"));
  display.display();
}

void showManualOperationScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("1. User Mode"));
  display.setCursor(0, 10);
  display.print(F("2. Operator Mode"));
  display.setCursor(0, 20);
  display.print(F("0. Back"));
  display.display();
}

void showAutomaticOperationScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("1. User Mode"));
  display.setCursor(0, 10);
  display.print(F("2. Operator Mode"));
  display.setCursor(0, 20);
  display.print(F("0. Back"));
  display.display();
}

void handleLogin() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Enter ID:"));
  display.setCursor(0, 10);
  display.print(inputID);
  display.display();

  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.print(F("Enter Password:"));
      display.setCursor(0, 10);
      display.print(inputPassword);
      display.display();

      char key2 = keypad.getKey();
      if (key2) {
        if (key2 == '#') {
          if (validateLogin(inputID, inputPassword)) {
            loginAttempts = 0;
            if (currentScreen == USER_MANUAL_LOGIN) {
              currentScreen = USER_MANUAL_CONTROL;
            } else if (currentScreen == OPERATOR_MANUAL_LOGIN) {
              currentScreen = OPERATOR_MANUAL_CONTROL;
            } else if (currentScreen == USER_AUTOMATIC_LOGIN) {
              currentScreen = USER_AUTOMATIC_CONTROL;
            } else if (currentScreen == OPERATOR_AUTOMATIC_LOGIN) {
              currentScreen = OPERATOR_AUTOMATIC_CONTROL;
            }
          } else {
            loginAttempts++;
            if (loginAttempts >= 2) {
              currentScreen = WELCOME;
            } else {
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              display.print(F("Invalid ID or Password"));
              display.display();
              delay(2000);
            }
          }
          inputID = "";
          inputPassword = "";
        } else {
          inputPassword += key2;
        }
      }
    } else {
      inputID += key;
    }
  }
}

bool validateLogin(String id, String password) {
  if ((currentScreen == USER_MANUAL_LOGIN && id == userManualID && password == userManualPassword) ||
      (currentScreen == OPERATOR_MANUAL_LOGIN && id == operatorManualID && password == operatorManualPassword) ||
      (currentScreen == USER_AUTOMATIC_LOGIN && id == userAutomaticID && password == userAutomaticPassword) ||
      (currentScreen == OPERATOR_AUTOMATIC_LOGIN && id == operatorAutomaticID && password == operatorAutomaticPassword)) {
    return true;
  }
  return false;
}

void userManualControl() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("User Manual Control"));
  display.setCursor(0, 10);
  display.print(F("1. Machine 1: "));
  display.print(machine1State ? "ON" : "OFF");
  display.setCursor(0, 20);
  display.print(F("2. Machine 2: "));
  display.print(machine2State ? "ON" : "OFF");
  display.setCursor(0, 30);
  display.print(F("3. Machine 3: "));
  display.print(machine3State ? "ON" : "OFF");
  display.setCursor(0, 40);
  display.print(F("4. Feeder: "));
  display.print(feederState ? "ON" : "OFF");
  display.setCursor(0, 50);
  display.print(F("5. Electro: "));
  display.print(electroState ? "ON" : "OFF");
  display.display();

  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
}

void operatorManualControl() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Operator Manual Control"));
  display.setCursor(0, 10);
  display.print(F("1. Machine 1: "));
  display.print(machine1State ? "ON" : "OFF");
  display.setCursor(0, 20);
  display.print(F("2. Machine 2: "));
  display.print(machine2State ? "ON" : "OFF");
  display.setCursor(0, 30);
  display.print(F("3. Machine 3: "));
  display.print(machine3State ? "ON" : "OFF");
  display.setCursor(0, 40);
  display.print(F("4. Feeder: "));
  display.print(feederState ? "ON" : "OFF");
  display.setCursor(0, 50);
  display.print(F("5. Electro: "));
  display.print(electroState ? "ON" : "OFF");
  display.setCursor(0, 60);
  display.print(F("6. Inc Feeder Spd"));
  display.setCursor(0, 70);
  display.print(F("7. Dec Feeder Spd"));
  display.display();

  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
}

void userAutomaticControl() {
  static unsigned long lastSwitchTime = millis();
  static int currentMachine = 0;

  if (millis() - lastSwitchTime >= interval) {
    lastSwitchTime = millis();
    switch (currentMachine) {
      case 0:
        machine1State = true;
        machine2State = false;
        machine3State = false;
        currentMachine = 1;
        break;
      case 1:
        machine1State = false;
        machine2State = true;
        machine3State = false;
        currentMachine = 2;
        break;
      case 2:
        machine1State = false;
        machine2State = false;
        machine3State = true;
        currentMachine = 0;
        break;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("User Automatic Control"));
  display.setCursor(0, 10);
  display.print(F("Machine 1: "));
  display.print(machine1State ? "ON" : "OFF");
  display.setCursor(0, 20);
  display.print(F("Machine 2: "));
  display.print(machine2State ? "ON" : "OFF");
  display.setCursor(0, 30);
  display.print(F("Machine 3: "));
  display.print(machine3State ? "ON" : "OFF");
  display.setCursor(0, 40);
  display.print(F("Press 0 to stop"));
  display.display();

  char key = keypad.getKey();
  if (key == '0') {
    machine1State = false;
    machine2State = false;
    machine3State = false;
    currentScreen = MAIN_MENU;
  }
}

void operatorAutomaticControl() {
  static unsigned long lastSwitchTime = millis();
  static int currentMachine = 0;

  if (millis() - lastSwitchTime >= interval) {
    lastSwitchTime = millis();
    switch (currentMachine) {
      case 0:
        machine1State = true;
        machine2State = false;
        machine3State = false;
        currentMachine = 1;
        break;
      case 1:
        machine1State = false;
        machine2State = true;
        machine3State = false;
        currentMachine = 2;
        break;
      case 2:
        machine1State = false;
        machine2State = false;
        machine3State = true;
        currentMachine = 0;
        break;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Operator Automatic Control"));
  display.setCursor(0, 10);
  display.print(F("Machine 1: "));
  display.print(machine1State ? "ON" : "OFF");
  display.setCursor(0, 20);
  display.print(F("Machine 2: "));
  display.print(machine2State ? "ON" : "OFF");
  display.setCursor(0, 30);
  display.print(F("Machine 3: "));
  display.print(machine3State ? "ON" : "OFF");
  display.setCursor(0, 40);
  display.print(F("Press 0 to stop"));
  display.display();

  char key = keypad.getKey();
  if (key == '0') {
    machine1State = false;
    machine2State = false;
    machine3State = false;
    currentScreen = MAIN_MENU;
  }
}

void timedOperation() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    machine1State = !machine1State;
    machine2State = !machine2State;
    machine3State = !machine3State;
  }
}

void updateDisplay() {
  // Refresh display based on current screen
}

void handleKeypadInput(char key) {
  switch (currentScreen) {
    case MAIN_MENU:
      if (key == '1') {
        currentScreen = MANUAL_OPERATION;
      } else if (key == '2') {
        currentScreen = AUTOMATIC_OPERATION;
      }
      break;
    case MANUAL_OPERATION:
      if (key == '1') {
        currentScreen = USER_MANUAL_LOGIN;
      } else if (key == '2') {
        currentScreen = OPERATOR_MANUAL_LOGIN;
      } else if (key == '0') {
        currentScreen = MAIN_MENU;
      }
      break;
    case AUTOMATIC_OPERATION:
      if (key == '1') {
        currentScreen = USER_AUTOMATIC_LOGIN;
      } else if (key == '2') {
        currentScreen = OPERATOR_AUTOMATIC_LOGIN;
      } else if (key == '0') {
        currentScreen = MAIN_MENU;
      }
      break;
    case USER_MANUAL_CONTROL:
    case OPERATOR_MANUAL_CONTROL:
      // Control machines based on keypad input
      if (key == '1') {
        machine1State = !machine1State;
      } else if (key == '2') {
        machine2State = !machine2State;
      } else if (key == '3') {
        machine3State = !machine3State;
      } else if (key == '4') {
        feederState = !feederState;
      } else if (key == '5') {
        electroState = !electroState;
      } else if (currentScreen == OPERATOR_MANUAL_CONTROL) {
        if (key == '6') {
          feederSpeed += 10;
        } else if (key == '7') {
          feederSpeed -= 10;
        }
      }
      break;
  }
}

void sendDataToESP() {
  String data = "Machine1=" + String(machine1State) + ",Machine2=" + String(machine2State) + ",Machine3=" + String(machine3State) + ",Feeder=" + String(feederState) + ",Electro=" + String(electroState) + "\n";
  Serial1.print(data);
}

void handleESPCommand(String command) {
  if (command == "Machine1=ON") {
    machine1State = true;
  } else if (command == "Machine1=OFF") {
    machine1State = false;
  } else if (command == "Machine2=ON") {
    machine2State = true;
  } else if (command == "Machine2=OFF") {
    machine2State = false;
  } else if (command == "Machine3=ON") {
    machine3State = true;
  } else if (command == "Machine3=OFF") {
    machine3State = false;
  } else if (command == "Feeder=ON") {
    feederState = true;
  } else if (command == "Feeder=OFF") {
    feederState = false;
  } else if (command == "Electro=ON") {
    electroState = true;
  } else if (command == "Electro=OFF") {
    electroState = false;
  }
}

float getUltrasonicDistance() {
  digitalWrite(ultrasonicTriggerPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTriggerPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTriggerPin1, LOW);

  long duration = pulseIn(ultrasonicEchoPin1, HIGH);
  float distance = (duration / 2) / 29.1;

  return distance;
}

float getTemperature() {
  int sensorValue = analogRead(tempSensorPin);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperature = (voltage - 0.5) * 100.0;
  return temperature;
}
