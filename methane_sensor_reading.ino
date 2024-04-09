#define BUZZER_PIN 18 // Buzzer connected to GPIO 18
#define ANALOG_PIN 13 // Analog sensor connected to GPIO 13
#define THRESHOLD 3550 // Threshold value for the analog reading

void setup() {
  pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as an output
  Serial.begin(115200); // Start serial communication at 115200 baud rate
}

void loop() {
  int sensorValue = analogRead(ANALOG_PIN); // Read the analog value from sensor
  Serial.println(sensorValue); // Print the sensor value to the Serial Monitor
  
  // Print the sensor value to the Serial Monitor for plotting
  Serial.println(sensorValue); 

  if(sensorValue > THRESHOLD) { 
    digitalWrite(BUZZER_PIN, HIGH); // If above threshold, turn the buzzer on
  } else {
    digitalWrite(BUZZER_PIN, LOW); // If not, turn the buzzer off
  }

  delay(100); // Short delay to reduce noise in analog reading
}
