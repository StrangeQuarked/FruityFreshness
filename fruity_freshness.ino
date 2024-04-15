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
const int photoResistorPin = A3;  // Change to an appropriate analog pin
const int lightThreshold = 850;   // Set the light/dark threshold

// Menu items and quantities
String menuItems[] = {"Apples", "Bananas", "Strawberries", "Oranges"};
int fruitQuantities[] = {0, 0, 0, 0};
int currentMenuItem = 0;
bool inEditMode = false;

void setup() {
  Serial.begin(9600);
  
  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Setup button pins with internal pull-up and debouncing
  pinMode(buttonUp, INPUT_PULLUP);
  debouncerUp.attach(buttonUp);
  debouncerUp.interval(10);
  
  pinMode(buttonSelect, INPUT_PULLUP);
  debouncerSelect.attach(buttonSelect);
  debouncerSelect.interval(10);

  pinMode(buttonDown, INPUT_PULLUP);
  debouncerDown.attach(buttonDown);
  debouncerDown.interval(10);

  // Set up the photoresistor pin as input (already an analog pin)
  pinMode(photoResistorPin, INPUT);

  // Initial display of menu
  updateMenuDisplay();
}

void loop() {
  debouncerUp.update();
  debouncerSelect.update();
  debouncerDown.update();
  managePowerStates();  // Manage power states based on ambient light

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
  int lightLevel = analogRead(photoResistorPin);
  Serial.println(lightLevel);
  if (lightLevel < lightThreshold) {
    lcd.noBacklight();  // Turn off the LCD backlight
    // Add calls here to deactivate other power-hungry components
  } else {
    lcd.backlight();  // Ensure the LCD backlight is on
    // Add calls here to reactivate those components
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
