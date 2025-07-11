/*
Smart Product Traceability System - Arduino Code
Hardware: Arduino Uno + Sensors + Actuators
*/

#include <Servo.h>
#include <LiquidCrystal.h>

// Pin Definitions
#define TRIG_PIN 7          // Ultrasonic sensor trigger
#define ECHO_PIN 6          // Ultrasonic sensor echo
#define SERVO_PIN 9         // Servo motor for reject mechanism
#define GREEN_LED 2         // Pass indicator
#define RED_LED 3           // Reject indicator  
#define BLUE_LED 4          // Processing indicator
#define BUZZER_PIN 8        // Status buzzer
#define BUTTON_PIN 5        // Manual trigger button

// LCD pins (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 10, A0, A1, A2);

// Objects
Servo rejectServo;

// Variables
bool productDetected = false;
bool systemReady = true;
int productCount = 0;
int passCount = 0;
int rejectCount = 0;
String currentProductID = "";
String inspectionResult = "";

// System states
enum SystemState {
  WAITING,
  DETECTING,
  PROCESSING,
  LABELING,
  INSPECTING,
  DECISION,
  COMPLETE
};

SystemState currentState = WAITING;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize servo
  rejectServo.attach(SERVO_PIN);
  rejectServo.write(0); // Initial position (accept path)
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Smart Labeling");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  
  // Initialize LEDs
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, HIGH); // System ready indicator
  
  Serial.println("Smart Product Traceability System Initialized");
  Serial.println("Waiting for products...");
  
  delay(2000);
  clearDisplay();
}

void loop() {
  switch(currentState) {
    case WAITING:
      handleWaitingState();
      break;
      
    case DETECTING:
      handleDetectingState();
      break;
      
    case PROCESSING:
      handleProcessingState();
      break;
      
    case LABELING:
      handleLabelingState();
      break;
      
    case INSPECTING:
      handleInspectingState();
      break;
      
    case DECISION:
      handleDecisionState();
      break;
      
    case COMPLETE:
      handleCompleteState();
      break;
  }
  
  // Check for manual trigger
  if (digitalRead(BUTTON_PIN) == LOW && currentState == WAITING) {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      triggerManualInspection();
    }
  }
  
  delay(100);
}

void handleWaitingState() {
  displayStatus("System Ready", "Waiting...");
  setLEDs(false, false, true); // Blue LED on
  
  // Check for product detection
  float distance = getDistance();
  if (distance < 10.0 && distance > 2.0) { // Product detected within 10cm
    productDetected = true;
    currentState = DETECTING;
    productCount++;
    currentProductID = "PRD" + String(productCount, DEC);
    
    Serial.println("PRODUCT_DETECTED:" + currentProductID);
    playBeep(1);
  }
}

void handleDetectingState() {
  displayStatus("Product Found", currentProductID);
  setLEDs(false, false, true); // Processing blue
  
  // Simulate product positioning delay
  delay(1000);
  
  Serial.println("REQUEST_INSPECTION:" + currentProductID);
  currentState = PROCESSING;
}

void handleProcessingState() {
  displayStatus("Processing...", "AI Inspection");
  setLEDs(false, false, true); // Processing blue
  
  // Wait for Python AI module response
  if (Serial.available()) {
    String response = Serial.readStringUntil('\n');
    response.trim();
    
    if (response.startsWith("INSPECTION_RESULT:")) {
      inspectionResult = response.substring(18); // Remove "INSPECTION_RESULT:"
      currentState = LABELING;
    }
  }
}

void handleLabelingState() {
  displayStatus("Labeling...", currentProductID);
  setLEDs(false, false, true); // Processing blue
  
  // Simulate labeling process
  Serial.println("APPLYING_LABEL:" + currentProductID);
  delay(2000); // Labeling time
  
  Serial.println("LABEL_APPLIED:" + currentProductID);
  currentState = INSPECTING;
}

void handleInspectingState() {
  displayStatus("Label Check", "OCR Verify");
  setLEDs(false, false, true); // Processing blue
  
  // Request label verification
  Serial.println("VERIFY_LABEL:" + currentProductID);
  
  // Wait for OCR verification result
  if (Serial.available()) {
    String response = Serial.readStringUntil('\n');
    response.trim();
    
    if (response.startsWith("LABEL_RESULT:")) {
      String labelResult = response.substring(13);
      
      // Combine inspection and label results
      if (inspectionResult == "PASS" && labelResult == "PASS") {
        inspectionResult = "PASS";
      } else {
        inspectionResult = "REJECT";
      }
      
      currentState = DECISION;
    }
  }
}

void handleDecisionState() {
  if (inspectionResult == "PASS") {
    // Product passes - continue on conveyor
    displayStatus("PASSED", currentProductID);
    setLEDs(true, false, false); // Green LED
    rejectServo.write(0); // Keep in accept path
    passCount++;
    playBeep(2); // Success beeps
    
    Serial.println("PRODUCT_PASSED:" + currentProductID);
    
  } else {
    // Product fails - activate reject mechanism
    displayStatus("REJECTED", currentProductID);
    setLEDs(false, true, false); // Red LED
    rejectServo.write(90); // Move to reject path
    rejectCount++;
    playBeep(3); // Reject beeps
    
    Serial.println("PRODUCT_REJECTED:" + currentProductID);
  }
  
  // Log the result
  logProductData();
  
  delay(3000); // Show result
  currentState = COMPLETE;
}

void handleCompleteState() {
  // Reset for next product
  rejectServo.write(0); // Reset to accept position
  productDetected = false;
  inspectionResult = "";
  
  // Show statistics
  displayStats();
  delay(2000);
  
  currentState = WAITING;
  Serial.println("READY_FOR_NEXT");
}

void triggerManualInspection() {
  if (currentState == WAITING) {
    productCount++;
    currentProductID = "MAN" + String(productCount, DEC);
    productDetected = true;
    currentState = DETECTING;
    
    Serial.println("MANUAL_TRIGGER:" + currentProductID);
    playBeep(1);
  }
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // Convert to cm
  
  return distance;
}

void setLEDs(bool green, bool red, bool blue) {
  digitalWrite(GREEN_LED, green);
  digitalWrite(RED_LED, red);
  digitalWrite(BLUE_LED, blue);
}

void displayStatus(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayStats() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P:" + String(passCount) + " R:" + String(rejectCount));
  lcd.setCursor(0, 1);
  lcd.print("Total: " + String(productCount));
}

void clearDisplay() {
  lcd.clear();
}

void playBeep(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void logProductData() {
  // Send logging data to Python
  String logData = "LOG_DATA:";
  logData += currentProductID + ",";
  logData += "BATCH001,"; // Simulated batch ID
  logData += "2025-07-01,"; // Manufacturing date
  logData += "RoHS_OK,"; // Compliance status
  logData += inspectionResult + ",";
  logData += String(millis()); // Timestamp
  
  Serial.println(logData);
}
