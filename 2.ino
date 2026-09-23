// Arduino UNO built-in LED (D13) control via Serial.
// D13 doesn't support PWM, so values 1..254 are represented by blink frequency.
// 0 = OFF, 255 = ON.

const byte LED_PIN = LED_BUILTIN; // D13
int level = 0;
bool running = false;
bool ledState = false;
unsigned long lastChange = 0;

unsigned long blinkInterval() {
  // 1 = медленное мигание (1000 мс), 254 = частое (50 мс)
  return map(level, 1, 254, 1000, 50);
}

void applyOutput() {
  if (!running || level == 0) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
  } else if (level == 255) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Включаем светодиод и сбрасываем таймер для мигания
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    lastChange = millis();
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);
  Serial.println("Ready: START, STOP, or value 0-255");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command == "START") {
      if (level == 0) level = 255; // Если уровень не был задан, включаем на 100%
      running = true;
      applyOutput();
      Serial.print("LED STARTED (Level ");
      Serial.print(level);
      Serial.println(")");
    } 
    else if (command == "STOP") {
      running = false;
      applyOutput();
      Serial.println("LED OFF");
    } 
    else if (command.length() > 0) {
      bool numeric = true;
      for (unsigned int i = 0; i < command.length(); i++) {
        if (!isDigit(command[i])) {
          numeric = false;
          break;
        }
      }

      if (numeric) {
        int value = command.toInt();
        if (value >= 0 && value <= 255) {
          level = value;
          running = (level > 0);
          applyOutput();
          Serial.print("Value set to ");
          Serial.println(level);
        } else {
          Serial.println("Use a value from 0 to 255");
        }
      } else {
        Serial.println("Use START, STOP, or a number from 0 to 255");
      }
    }
  }

  // Неблокирующее мигание для значений от 1 до 254
  if (running && level > 0 && level < 255) {
    if (millis() - lastChange >= blinkInterval()) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      lastChange = millis();
    }
  }
}