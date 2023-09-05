/*
* Morse decoder V4
* By YO2GAR
* 
* Decodes one letter/number at the time
* Has full range of alphabet and numbers
* 
* A dot is equal to every impulse that is larger than 50 millis(0.05 sec) and smaller than 150 millis(0.15 sec)
* A line is equal to every impulse that is larger than 150 millis(0.15 sec)
* 
* My hardware:
* - Arduino Nano with CH340 USB 
* - A 5V active buzzer
* - A LED
* - One 330R resistor
* - A push button
* - A breadboard that can fit the Arduino Nano, a push button and a Buzzer
* - A display from GeekUniverse (If you have a normal Oled Display check out: ###)
* - Jump Wires
* 
* Connections:
* ###########################
* # Arduino Nano  # Display #
* ###########################
* #     5V        #   VCC   #
* ###########################
* #     GND       #   GND   #
* ###########################
* #     SDA       #   SDA   #
* ###########################
* #     SCL       #   SCL   #
* ###########################
* 
* ###############################
* #  Component  # Arduino Nano  #
* ###############################
* #   Buzzer    #     Pin 9     #
* ###############################
* #   LED       #     Pin 9     #
* ###############################
* # Push Button #     Pin 2     #
* ###############################
* 
* More info and diagrams can be found in repository
* 
*/
#include <avr/pgmspace.h>
#include <LCD.h>
#include <Wire.h>

int address = 1616;
int row = 1;
int col = 1;

LCD lcd;

unsigned long signal_len, t1, t2;   // Time for which button is pressed
int inputPin = 2;                   // Input pin for push button
int ledPin = 9;                     // Output pin for LED
String code = "";                   // String in which one alphabet is stored

void Clear() {
  for (int i = 0; i <= 16; i++) {
    for (int j = 0; j <= 16; j++) {
      lcd.Display_MatrixClear(i, j, address);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT_PULLUP);  // Internal pullup resistor is used to simplify the circuit
  pinMode(ledPin, OUTPUT);

  lcd.Init();
  lcd.Display_MatrixClear(0, 0, address);
  Clear();

  lcd.Display_ASCII(2, 5, "Morse Decoder");
  lcd.Display_ASCII(4, 7, "by YO2GAR");
  lcd.Display_ASCII(8, 11, "V4");
  delay(5000);
  Clear();
}

void loop() {
NextDotDash:
  while (digitalRead(inputPin) == HIGH) {}
  t1 = millis();                    // Time at button press
  digitalWrite(ledPin, HIGH);       // LED on while button pressed
  while (digitalRead(inputPin) == LOW) {}
  t2 = millis();                    // Time at button release
  digitalWrite(ledPin, LOW);        // LED off on button release
  signal_len = t2 - t1;             // Time for which button is pressed
  if (signal_len > 50) {            // To account for switch debouncing
    code += readio();               // Function to read dot or dash
  }
  while ((millis() - t2) < 500) {   // If time between button press greater than 0.5sec, skip loop and go to next alphabet
    if (digitalRead(inputPin) == LOW) {
      goto NextDotDash;
    }
  }
  convertor();                      // Function to decipher code into alphabet
}

char readio() {
  if (signal_len < 150 && signal_len > 50) {
    return '.';                    // If button press less than 0.6sec, it is a dot
  } else if (signal_len > 150) {
    return '-';                    // If button press more than 0.6sec, it is a dash
  }
}

void convertor() {
  static String letters[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
                             "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "E"
                            };
  char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";  // Letters and numbers in the same order as the Morse code table
  int i = 0;
  if (code == ".-.-.-") {
    Serial.print(".");              // For break
  } else {
    while (letters[i] != "E") {      // Loop for comparing input code with letters array
      if (letters[i] == code) {
        Serial.print(alphabet[i]);
        String foo = String(alphabet[i]);
        lcd.Display_ASCII(col, row, foo);
        if (row == 16) {
          col++;
          row = 1;
        }
        else {
          row++;
        }
        Serial.print("col: "); Serial.print(col); Serial.print(" row: "); Serial.println(row);
        //row++;
        break;
      }
      i++;
    }
    if (letters[i] == "E") {
      Serial.println("<Wrong input>");      // If input code doesn't match any letter, error
      //lcd.Display_ASCII(next, 2, "<Wrong input>");
    }
  }
  code = "";                              // Reset code to blank string
}
