#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Buttons setup
const int buttonUp = 8;
const int buttonSelect = 9;
const int buttonDown = 10;

// Bounce objects for debouncing the buttons
Bounce debouncerUp = Bounce();
Bounce debouncerSelect = Bounce();
Bounce debouncerDown = Bounce();

// Photoresistor and light threshold
const int photoResistorPin = 17;  
const int lightThreshold = 850;

// Sensor and Buzzer setup
const int methaneSensorPin = 15;
const int buzzerPin = 3;
const int methaneSensorPowerPin = 14;  // Power control pin for the methane sensor


// Menu items, quantities, and thresholds
String menuItems[] = {"Apples", "Bananas", "Strawberries", "Oranges"};
int fruitQuantities[] = {0, 0, 0, 0};
float fruitThresholds[] = {0.2, 0.5, 0.3, 0.1};  // Threshold values for each fruit
int currentMenuItem = 0;
bool inEditMode = false;

unsigned long lastCheck = 0;  // Last time the methane level was checked
const long interval = 1000;  // Interval at which to check methane levels (milliseconds)

bool LPM = false;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();

  pinMode(buttonUp, INPUT_PULLUP);
  debouncerUp.attach(buttonUp);
  debouncerUp.interval(10);
  
  pinMode(buttonSelect, INPUT_PULLUP);
  debouncerSelect.attach(buttonSelect);
  debouncerSelect.interval(10);

  pinMode(buttonDown, INPUT_PULLUP);
  debouncerDown.attach(buttonDown);
  debouncerDown.interval(10);

  pinMode(photoResistorPin, INPUT);
  pinMode(methaneSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(methaneSensorPowerPin, OUTPUT);

  digitalWrite(methaneSensorPowerPin, HIGH);  // Initially turn on the methane sensor

  updateMenuDisplay();
}

void loop() {
  debouncerUp.update();
  debouncerSelect.update();
  debouncerDown.update();
  managePowerStates();

  digitalWrite(methaneSensorPowerPin, LOW);  // Turn off methane sensor to save power

  if (millis() - lastCheck >= interval && !LPM) {
    digitalWrite(methaneSensorPowerPin, HIGH);  // Turn off methane sensor to save power
    delay(500);
    checkSensorAndAlert();
    lastCheck = millis();
  }

  if (debouncerSelect.fell()) {
    inEditMode = !inEditMode;
    if (inEditMode) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set Qty for:");
      lcd.setCursor(0, 1);
      lcd.print(menuItems[currentMenuItem]);
      lcd.print(": ");
      lcd.print(fruitQuantities[currentMenuItem]);
    } else {
      updateMenuDisplay();
    }
  }

  if (inEditMode) {
    if (debouncerUp.fell()) {
      fruitQuantities[currentMenuItem]++;
      displayQuantity();
    } else if (debouncerDown.fell() && fruitQuantities[currentMenuItem] > 0) {
      fruitQuantities[currentMenuItem]--;
      displayQuantity();
    }
  } else {
    if (debouncerUp.fell()) {
      navigateMenu(-1);
    } else if (debouncerDown.fell()) {
      navigateMenu(1);
    }
  }
}

void managePowerStates() {
  int lightLevel = digitalRead(photoResistorPin);
  if (lightLevel == LOW) {
    LPM = true;
    lcd.noBacklight();
    digitalWrite(methaneSensorPowerPin, LOW);  // Turn off methane sensor to save power
  } else {
    LPM = false;
    lcd.backlight();
    digitalWrite(methaneSensorPowerPin, HIGH);  // Turn on methane sensor
  }
}

void checkSensorAndAlert() {
  float sensorValue = analogRead(methaneSensorPin);

  Serial.print("METHANE LEVEL!!!!!!!!!!!: ");
  Serial.print(sensorValue);
  Serial.println();

  float theoretical_limit = 400;

  


  /*for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++) {
    theoretical_limit += fruitQuantities[i] * fruitThresholds[i];
  }*/

  Serial.print("THEORETICAL LIMIT: ");
  Serial.print(theoretical_limit);
  Serial.println();
  
  if (sensorValue > theoretical_limit) {
    analogWrite(buzzerPin, 127);
  } 
  else {
    analogWrite(buzzerPin, 0);
  }
}

void navigateMenu(int direction) {
  currentMenuItem += direction;
  int numFruits = sizeof(menuItems) / sizeof(String);
  if (currentMenuItem >= numFruits) {
    currentMenuItem = 0;
  } else if (currentMenuItem < 0) {
    currentMenuItem = numFruits - 1;
  }
  updateMenuDisplay();
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Fruit:");
  lcd.setCursor(0, 1);
  lcd.print("> " + menuItems[currentMenuItem]);
  lcd.setCursor(0, 3);
  lcd.print("Qty: ");
  lcd.print(fruitQuantities[currentMenuItem]);
}

void displayQuantity() {
  lcd.setCursor(0, 1);
  lcd.print(menuItems[currentMenuItem]);
  lcd.print(": ");
  lcd.print(fruitQuantities[currentMenuItem]);
  lcd.print("    ");
}
