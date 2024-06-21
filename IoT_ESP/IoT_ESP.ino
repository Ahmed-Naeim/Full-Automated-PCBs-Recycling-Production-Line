#define BLYNK_TEMPLATE_ID "TMPL2acXqDvYO"
#define BLYNK_TEMPLATE_NAME "FAPRL"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Blynk Auth Token
char auth[] = "YourAuthToken";

// WiFi credentials
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

bool machineStates[5] = {false, false, false, false, false};
bool mode = false; // false: Manual, true: Automatic

BlynkTimer timer;

// Function Prototypes
void sendStatus();
void updateMachineState(int machine, bool state);
void updateMode(bool newMode);

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendStatus); // Send status to Blynk every second
}

void loop() {
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V1) {
  updateMachineState(0, param.asInt());
}

BLYNK_WRITE(V2) {
  updateMachineState(1, param.asInt());
}

BLYNK_WRITE(V3) {
  updateMachineState(2, param.asInt());
}

BLYNK_WRITE(V4) {
  updateMachineState(3, param.asInt());
}

BLYNK_WRITE(V5) {
  updateMachineState(4, param.asInt());
}

BLYNK_WRITE(V6) {
  updateMode(param.asInt());
}

void sendStatus() {
  for (int i = 0; i < 5; i++) {
    Blynk.virtualWrite(V1 + i, machineStates[i]);
  }
  Blynk.virtualWrite(V6, mode);
}

void updateMachineState(int machine, bool state) {
  machineStates[machine] = state;
  Serial.print("Machine");
  Serial.print(machine + 1);
  Serial.print(": ");
  Serial.println(state ? "ON" : "OFF");
  Serial1.print("M");
  Serial1.print(machine);
  Serial1.print(":");
  Serial1.println(state ? "1" : "0");
}

void updateMode(bool newMode) {
  mode = newMode;
  Serial.print("Mode: ");
  Serial.println(mode ? "Automatic" : "Manual");
  Serial1.print("Mode:");
  Serial1.println(mode ? "1" : "0");
}
