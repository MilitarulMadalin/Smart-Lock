#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD and servo setup
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

// Pins
const int buttonUp = 2;
const int buttonDown = 3;
const int buttonOK = 4;
const int buttonChange = 5;

const int servoPin = 9;

const int redLED = 10;
const int greenLED = 11;

// Passwords
int correctPIN[4] = {1, 2, 3, 4};
int trollPIN[4] = {6, 7, 6, 7};
int enteredPIN[4] = {0, 0, 0, 0};

// System variables
int currentDigit = 0;
int wrongAttempts = 0;

bool systemLocked = false;

unsigned long lastButtonPress = 0;

// SETUP
void setup() {

  Serial.begin(9600);

  // LCD setup
  lcd.init();
  lcd.backlight();

  // Servo setup
  lockServo.attach(servoPin);
  lockServo.write(0);

  // Button inputs
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonOK, INPUT_PULLUP);
  pinMode(buttonChange, INPUT_PULLUP);

  // LED outputs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  resetSystem();
}

// MAIN LOOP
void loop() {

  // Stop everything if the system is locked
  if (systemLocked) return;

  // Turn off LCD after 20 seconds
  if (millis() - lastButtonPress > 20000) {

    lcd.noBacklight();

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
  }

  // Change password
  if (digitalRead(buttonChange) == LOW) {

    wakeSystem();

    delay(300);

    changePassword();
  }

  // Increase current digit
  if (digitalRead(buttonUp) == LOW) {

    wakeSystem();

    enteredPIN[currentDigit]++;

    if (enteredPIN[currentDigit] > 9) {
      enteredPIN[currentDigit] = 0;
    }

    updateLCD();

    delay(250);
  }

  // Decrease current digit
  if (digitalRead(buttonDown) == LOW) {

    wakeSystem();

    enteredPIN[currentDigit]--;

    if (enteredPIN[currentDigit] < 0) {
      enteredPIN[currentDigit] = 9;
    }

    updateLCD();

    delay(250);
  }

  // Confirm digit
  if (digitalRead(buttonOK) == LOW) {

    wakeSystem();

    delay(350);

    currentDigit++;

    // Check PIN after all digits are entered
    if (currentDigit > 3) {

      checkPIN();

      currentDigit = 0;
    }
    else {

      updateLCD();
    }
  }
}

// CHECK PIN
void checkPIN() {

  bool correct = true;
  bool troll = true;
  bool reversed = true;

  // Compare entered PINs
  for (int i = 0; i < 4; i++) {

    if (enteredPIN[i] != correctPIN[i]) {
      correct = false;
    }

    if (enteredPIN[i] != trollPIN[i]) {
      troll = false;
    }

    // Check reversed password
    if (enteredPIN[i] != correctPIN[3 - i]) {
      reversed = false;
    }
  }

  lcd.clear();

  //Easter egg
  if (troll) {

    lcd.print("AI LUAT TEAPA!");
    lcd.setCursor(0, 1);
    lcd.print("67 LUBENITE");

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, HIGH);

    delay(4000);

    resetSystem();
  }

  // Reverse password mode
  else if (reversed && !correct) {

    lcd.print("IN PERICOL!");
    lcd.setCursor(0, 1);
    lcd.print("SUNAM POLITIA...");

    // Blink red LED
    for (int i = 0; i < 10; i++) {

      digitalWrite(redLED, HIGH);
      delay(100);

      digitalWrite(redLED, LOW);
      delay(100);
    }

    lockSystem(10);
  }

  // Correct password
  else if (correct) {

    lcd.print("COD CORECT");

    digitalWrite(greenLED, HIGH);

    wrongAttempts = 0;

    delay(1500);

    openDoor();
  }

  // Wrong password
  else {

    wrongAttempts++;

    lcd.print("COD GRESIT!");

    digitalWrite(redLED, HIGH);

    // Lock system after 3 failed attempts
    if (wrongAttempts >= 3) {

      lockSystem(15);
    }
    else {

      delay(2000);

      resetSystem();
    }
  }
}

// CHANGE PASSWORD
void changePassword() {

  lcd.clear();

  lcd.print("PIN NOU:");

  currentDigit = 0;

  // Reset entered PIN
  for (int i = 0; i < 4; i++) {
    enteredPIN[i] = 0;
  }

  updateLCD();

  // Read new password
  while (currentDigit < 4) {

    if (digitalRead(buttonUp) == LOW) {

      enteredPIN[currentDigit]++;

      if (enteredPIN[currentDigit] > 9) {
        enteredPIN[currentDigit] = 0;
      }

      updateLCD();

      delay(250);
    }

    if (digitalRead(buttonDown) == LOW) {

      enteredPIN[currentDigit]--;

      if (enteredPIN[currentDigit] < 0) {
        enteredPIN[currentDigit] = 9;
      }

      updateLCD();

      delay(250);
    }

    if (digitalRead(buttonOK) == LOW) {

      currentDigit++;

      delay(400);

      if (currentDigit < 4) {
        updateLCD();
      }
    }
  }

  // Save new password
  for (int i = 0; i < 4; i++) {
    correctPIN[i] = enteredPIN[i];
  }

  lcd.clear();

  lcd.print("SALVAT!");

  digitalWrite(greenLED, HIGH);

  delay(2000);

  resetSystem();
}

// OPEN DOOR
void openDoor() {

  lcd.clear();

  lcd.print("DESCHIS");

  // Unlock
  lockServo.write(90);

  delay(5000);

  // Lock again
  lockServo.write(0);

  resetSystem();
}

// LOCK SYSTEM
void lockSystem(int seconds) {

  systemLocked = true;

  digitalWrite(redLED, HIGH);

  for (int i = seconds; i > 0; i--) {

    lcd.clear();

    lcd.print("BLOCAT: ");
    lcd.print(i);
    lcd.print("s");

    delay(1000);
  }

  digitalWrite(redLED, LOW);

  wrongAttempts = 0;

  systemLocked = false;

  resetSystem();
}

// UPDATE LCD
void updateLCD() {

  lcd.setCursor(0, 1);

  for (int i = 0; i < 4; i++) {

    // Confirmed digits
    if (i < currentDigit) {

      lcd.print(" * ");
    }

    // Current digit
    else if (i == currentDigit) {

      lcd.print("[");

      lcd.print(enteredPIN[i]);

      lcd.print("]");
    }

    // Remaining digits
    else {
      lcd.print(" _ ");
    }
  }
}

// WAKE SYSTEM
void wakeSystem() {

  lastButtonPress = millis();

  lcd.backlight();
}

// RESET SYSTEM
void resetSystem() {

  // Clear entered PIN
  for (int i = 0; i < 4; i++) {
    enteredPIN[i] = 0;
  }

  currentDigit = 0;

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  lcd.clear();

  lcd.print("INTRODU PIN:");

  updateLCD();
}