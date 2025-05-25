const int sensorPins[4] = { A0, A1, A2, A4 };
float voltages[4] = { 0, 0, 0, 0 };

const char* directions[4] = { "North", "East", "South", "West" };
const float voltageThreshold = 2.0;

struct Reading {
  String direction;
  unsigned long timestamp;
};
Reading history[5];
int historyIndex = 0;

String confirmedDirection = "None";
unsigned long directionEndTime = 0;
const unsigned long displayDuration = 5000;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) {
    history[i] = {"None", 0};
  }
}

void loop() {
  String currentDirection = "None";
  float maxVolt = voltageThreshold;
  
  for (int i = 0; i < 4; i++) {
    voltages[i] = analogRead(sensorPins[i]) * (5.0 / 1023.0);
    if (voltages[i] > maxVolt) {
      maxVolt = voltages[i];
      currentDirection = directions[i];
    }
  }

  history[historyIndex] = {currentDirection, millis()};
  historyIndex = (historyIndex + 1) % 5;

  if (confirmedDirection == "None" && currentDirection != "None") {
    int count = 0;
    unsigned long now = millis();
    
    for (int i = 0; i < 5; i++) {
      if (history[i].direction == currentDirection && 
          (now - history[i].timestamp) <= 5000) {
        count++;
        if (count >= 2) {
          confirmedDirection = currentDirection;
          directionEndTime = now + displayDuration;
          break;
        }
      }
    }
  }
  if (currentDirection == confirmedDirection && currentDirection != "None") {
    directionEndTime = millis() + displayDuration;
  }
  if (millis() > directionEndTime) {
    confirmedDirection = "None";
  }
  Serial.println(confirmedDirection);
  delay(1000);
}
