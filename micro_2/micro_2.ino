#define START_BTN 1
#define LEVEL_SENSOR_1 A1
#define LEVEL_SENSOR_2 A2
#define LEVEL_SENSOR_4 A3
#define LEVEL_SENSOR_5 A4
#define PUMP1_PIN 6
#define PUMP2_PIN 7
#define PUMP3_PIN 8
#define BOTTOM_VALVE_PIN 9
#define ALARM_PIN 10
#define INDICATION_LAMP_PIN 11
#define MIXER 12
#define TEMP_SENSOR_PIN A0  // Pin for temperature sensor
bool systemStarted = false;
int temp = 11;
// Pin configurations for each seven-segment display
const byte onesPins[] = {23, 25, 27, 29, 31, 33, 35};  // Pins for ones place
const byte tensPins[] = {37, 39, 41, 43, 45, 47, 49};  // Pins for tens place
const byte onesCathodePin = 2; // Common cathode pin for ones display
const byte tensCathodePin = 3; // Common cathode pin for tens display

// Segment bit patterns for numbers 0-9 (for common cathode display)
const byte digitPatterns[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};
void setup() {
  pinMode(START_BTN, INPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  pinMode(ALARM_PIN, OUTPUT);
  pinMode(INDICATION_LAMP_PIN, OUTPUT);
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(PUMP3_PIN, OUTPUT);
  pinMode(BOTTOM_VALVE_PIN, OUTPUT);
  pinMode(MIXER, OUTPUT);
  for (int i = 0; i < 7; i++) {
    pinMode(onesPins[i], OUTPUT);
    pinMode(tensPins[i], OUTPUT);
  }
}

void loop() {
     if (digitalRead(START_BTN) == 1) {  
        systemStarted = true;  
        digitalWrite(INDICATION_LAMP_PIN, HIGH);  
        processTankFilling();  
    } 
}

void processTankFilling() {  
    digitalWrite(PUMP1_PIN, HIGH);  
    digitalWrite(PUMP2_PIN, HIGH);  
    digitalWrite(PUMP3_PIN, HIGH);  
    
    while (true) {  
        if (digitalRead(LEVEL_SENSOR_1) == LOW) {  
            digitalWrite(ALARM_PIN, HIGH); // Alarm ON  
        } else {  
            digitalWrite(ALARM_PIN, LOW); // Alarm OFF  
        }  

        // Check levels  
        if (digitalRead(LEVEL_SENSOR_2) == HIGH) {  
            digitalWrite(PUMP1_PIN, LOW);    
        }  

        if (digitalRead(LEVEL_SENSOR_4) == HIGH) {   
            digitalWrite(PUMP2_PIN, LOW);  
  
        }  

        if (digitalRead(LEVEL_SENSOR_5) == HIGH) {    
            digitalWrite(PUMP3_PIN, LOW);  
            // Fill the tank to level 5   
                mixAndFeed();  
                break;   
              
        }  
    }  
}  

void mixAndFeed() {  
    int temperature = readTemperature();
    display(temperature);  
    digitalWrite(MIXER, HIGH);    
    // Assuming the temperature threshold is 70 degrees  
    while (temperature <= 70) {  
        temperature = readTemperature();  // Update temperature
        display(temperature);                                   
    }  
    digitalWrite(MIXER, LOW);
    digitalWrite(BOTTOM_VALVE_PIN, HIGH);  
    delay(500);
    digitalWrite(BOTTOM_VALVE_PIN, LOW); 
    // Reset system for next operation 

    systemStarted = false;  
    digitalWrite(INDICATION_LAMP_PIN, LOW); 
    display(0);
}  


int readTemperature() {
  int sensorValue = analogRead(A0);  // Read the analog value from LM35
  float voltage = sensorValue * (5.0 / 1023.0);  // Convert the value to voltage (range: 0-5V)
  int temperature = voltage * 100;  // LM35 gives 10mV per degree Celsius, so multiply by 100 to get the temperature in °C
  return temperature;
}
// Function to display a number on the 7-segment displays
void display(int num) {
  // Split the number into tens and ones
  int tens = num / 10;
  int ones = num % 10;

  // Display ones place (first display)
  digitalWrite(onesCathodePin, LOW);  // Enable ones display
  digitalWrite(tensCathodePin, HIGH);  // Disable tens display
  displayDigit(ones, onesPins);
  delay(5);  // Short delay to avoid flickering

  // Display tens place (second display)
  digitalWrite(onesCathodePin, HIGH);  // Disable ones display
  digitalWrite(tensCathodePin, LOW);   // Enable tens display
  displayDigit(tens, tensPins);
  delay(5);  // Short delay to avoid flickering
}

// Function to display a single digit on a 7-segment display
void displayDigit(int digit, const byte pins[]) {
  // Get the bit pattern for the digit
  byte pattern = digitPatterns[digit];
  
  // Set each segment based on the pattern
  for (int i = 0; i < 7; i++) {
    digitalWrite(pins[i], bitRead(pattern, i)); // Write the bit to the corresponding pin
  }
}