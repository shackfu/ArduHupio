#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Parameter
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Relais Pins
const int relays[] = {2, 3, 4, 5, 6, 7}; // Relais 1-6
const int numRelays = 6;

// Menü Variablen
int menuIndex = 0;
const int numOptions = 4; // Anzahl der Optionen inkl. Manuelle Steuerung
String options[numOptions] = {"Song 1", "Song 2", "Song 3", "Manuelle Steuerung"};
bool isPlaying = false;
bool manualControl = false; // Flag für manuellen Modus

// Joystick Pins
const int joystickX = A0;  // X-Achse
const int joystickY = A1;  // Y-Achse
const int buttonSelect = 8; // Joystick Button
int lastJoystickX = 0; // Variable zur Speicherung der letzten X-Position
int lastJoystickY = 0; // Variable zur Speicherung der letzten Y-Position

void setup() {
  // Relais Pins als Ausgang festlegen
  for (int i = 0; i < numRelays; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], LOW); // Alle Relais ausschalten
  }

  // Joystick Pins
  pinMode(buttonSelect, INPUT_PULLUP);

  // Initialisierung des Displays
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Begrüßung
  display.setCursor(0, 0);
  display.println("Hupe Auswahl");
  display.display();
  delay(2000);
  
  showMenu();
}

void loop() {
  // Joystick Positionen lesen
  int currentJoystickX = analogRead(joystickX);
  int currentJoystickY = analogRead(joystickY);

  // Joystick Button zum Wechsel zwischen Menü und Modi
  if (digitalRead(buttonSelect) == LOW) {
    if (manualControl) {
      manualControl = false; // Zurück zum Hauptmenü
      showMenu();
    } else {
      // Prüfe ob "Manuelle Steuerung" oder ein Song ausgewählt wurde
      if (menuIndex < numOptions - 1) {
        isPlaying = !isPlaying;
        if (isPlaying) {
          playSong(menuIndex);
        } else {
          stopSong();
        }
      } else {
        manualControl = true; // In den manuellen Modus wechseln
        showManualControl();
      }
    }
    delay(200); // Debounce
  }

  if (manualControl) {
    // Manuelle Steuerung: Hupen mit dem Joystick steuern
    handleManualControl(currentJoystickX, currentJoystickY);
  } else {
    // Navigation durch das Menü
    if (currentJoystickX < 400 && lastJoystickX >= 400) { // Links
      menuIndex--;
      if (menuIndex < 0) menuIndex = numOptions - 1;
      showMenu();
      delay(200); // Debounce
    }

    if (currentJoystickX > 600 && lastJoystickX <= 600) { // Rechts
      menuIndex++;
      if (menuIndex >= numOptions) menuIndex = 0;
      showMenu();
      delay(200); // Debounce
    }

    lastJoystickX = currentJoystickX;
  }
}

void showMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Waehle eine Option:");
  for (int i = 0; i < numOptions; i++) {
    if (i == menuIndex) {
      display.print("> "); // Zeiger für ausgewählte Option
    } else {
      display.print("  ");
    }
    display.println(options[i]);
  }
  display.display();
}

void playSong(int songIndex) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Playing:");
  display.println(options[songIndex]);
  display.display();

  // Relais-Muster für die verschiedenen Songs
  switch (songIndex) {
    case 0: // Song 1
      digitalWrite(relays[0], HIGH);
      delay(500);
      digitalWrite(relays[0], LOW);
      digitalWrite(relays[1], HIGH);
      delay(500);
      digitalWrite(relays[1], LOW);
      break;
    case 1: // Song 2
      for (int i = 0; i < 3; i++) {
        digitalWrite(relays[i], HIGH);
        delay(300);
        digitalWrite(relays[i], LOW);
      }
      break;
    case 2: // Song 3
      for (int i = 0; i < 6; i++) {
        digitalWrite(relays[i], HIGH);
        delay(200);
        digitalWrite(relays[i], LOW);
      }
      break;
  }
}

void stopSong() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Stopped");
  display.display();
  
  // Alle Relais ausschalten
  for (int i = 0; i < numRelays; i++) {
    digitalWrite(relays[i], LOW);
  }
}

void showManualControl() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Manuelle Steuerung:");
  display.println("Joystick bewegt Hupen.");
  display.display();
}

// Funktion zur manuellen Steuerung der Hupen
void handleManualControl(int x, int y) {
  // Hupe 1 - Joystick nach oben
  if (y < 400 && x > 400 && x < 600) {
    digitalWrite(relays[0], HIGH);
  } else {
    digitalWrite(relays[0], LOW);
  }

  // Hupe 2 - Joystick nach unten
  if (y > 600 && x > 400 && x < 600) {
    digitalWrite(relays[1], HIGH);
  } else {
    digitalWrite(relays[1], LOW);
  }

  // Hupe 3 - Joystick nach links
  if (x < 400 && y > 400 && y < 600) {
    digitalWrite(relays[2], HIGH);
  } else {
    digitalWrite(relays[2], LOW);
  }

  // Hupe 4 - Joystick nach rechts
  if (x > 600 && y > 400 && y < 600) {
    digitalWrite(relays[3], HIGH);
  } else {
    digitalWrite(relays[3], LOW);
  }

  // Hupe 5 - Joystick oben rechts (diagonal)
  if (x > 600 && y < 400) {
    digitalWrite(relays[4], HIGH);
  } else {
    digitalWrite(relays[4], LOW);
  }

  // Hupe 6 - Joystick unten rechts (diagonal)
  if (x < 400 && y < 400) {
    digitalWrite(relays[5], HIGH);
  } else {
    digitalWrite(relays[5], LOW);
  }

  // Aktuelle Joystick-Position speichern
  lastJoystickX = x;
  lastJoystickY = y;
}
