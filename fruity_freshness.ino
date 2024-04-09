#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Buttons setup
const int buttonUp = 32;
const int buttonSelect = 33;
const int buttonDown = 35;
const int buzzer = 18;

// Bounce objects for debouncing the buttons
Bounce debouncerUp = Bounce();
Bounce debouncerSelect = Bounce();
Bounce debouncerDown = Bounce();

String fruits[] = {"Apples", "Bananas", "Strawberries", "Oranges"};
float fruitThresholds[] = {0.2, 0.4, 0.3, 0.5}; // ARBITRARY THRESHOLD VALUES FOR NOW
int fruitQuantities[] = {0, 0, 0, 0}; // quantities for each fruit
float methaneLimit = 0; // theoretical methane limit based on fruit quantities and thresholds
                        // calculated with calculateMethaneLimit()

int currentSelection = 0; // Currently selected menu item
bool inSelectionMode = false; // true if adjusting the quantity of a fruit

void setup() {
  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the backlight
  
  // Initialize buttons with internal pull-up and attach them to the debouncers
  pinMode(buttonUp, INPUT_PULLUP);
  debouncerUp.attach(buttonUp);
  debouncerUp.interval(5); // debounce interval in milliseconds
  
  pinMode(buttonSelect, INPUT_PULLUP);
  debouncerSelect.attach(buttonSelect);
  debouncerSelect.interval(5);
  
  pinMode(buttonDown, INPUT_PULLUP);
  debouncerDown.attach(buttonDown);
  debouncerDown.interval(5);
  
  pinMode(buzzer, OUTPUT);         // Set buzzer pin as output
  updateMenuDisplay();             // Display the initial menu

  Serial.begin(115200); // Start serial communication
}

void loop() {
  // Update the debouncers
  debouncerUp.update();
  debouncerSelect.update();
  debouncerDown.update();
  
  // Button logic
  if (debouncerSelect.fell()) {
    handleSelectButton();
  } 
  
  else if (!inSelectionMode) {
    if (debouncerUp.fell()) {
      navigateMenu(-1); // Navigate up
    } 
    else if (debouncerDown.fell()) {
      navigateMenu(1); // Navigate down
    }
  } 
  
  else {
    if (debouncerUp.fell()) {
      adjustQuantity(1); // Increase quantity
    } 
    else if (debouncerDown.fell()) {
      adjustQuantity(-1); // Decrease quantity
    }
  }

  // Here we would implement the methane sensor reading logic
  // If the methane level exceeds methaneLimit, activate the buzzer

  // float methaneLevel = readMethaneSensor();
  // if (methaneLevel > methaneLimit) {
  //   digitalWrite(buzzer, HIGH);
  // } else {
  //   digitalWrite(buzzer, LOW);
  // }
}

void updateMenuDisplay() {
  lcd.clear();
  lcd.print("Fruit: ");
  lcd.print(fruits[currentSelection]);
  // Optionally, display the current quantity in selection mode
}

void navigateMenu(int direction) {
  currentSelection += direction;
  int numFruits = sizeof(fruits) / sizeof(fruits[0]);

  if (currentSelection < 0) currentSelection = numFruits - 1;

  if (currentSelection >= numFruits) currentSelection = 0;

  updateMenuDisplay();
}

void handleSelectButton() {
  inSelectionMode = !inSelectionMode; // Toggle selection mode
  if (inSelectionMode) {
    lcd.clear();
    lcd.print("Set Qty for");
    lcd.setCursor(0, 1);
    lcd.print(fruits[currentSelection]);
    lcd.print(": ");
    lcd.print(fruitQuantities[currentSelection]);
  } 
  else {
    calculateMethaneLimit();
    updateMenuDisplay();
  }
}

void adjustQuantity(int adjustment) {
  fruitQuantities[currentSelection] += adjustment;
  if (fruitQuantities[currentSelection] < 0) fruitQuantities[currentSelection] = 0;
  lcd.setCursor(0, 1);
  lcd.print(fruits[currentSelection]);
  lcd.print(": ");
  lcd.print(fruitQuantities[currentSelection]);
  lcd.print("    "); // Clear any leftover characters
}

void calculateMethaneLimit() {
  methaneLimit = 0;
  for (int i = 0; i < sizeof(fruits) / sizeof(fruits[0]); i++) {
    methaneLimit += fruitQuantities[i] * fruitThresholds[i];
    Serial.println(methaneLimit);
  }
  // Display or use the calculated methaneLimit as needed
}
