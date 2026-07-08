#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo1;
Servo servo2;

// ================= KONFIGURASI WiFi & SERVER =================
const char* ssid       = "your_ssid";
const char* password   = "your_password";
const char* serverUrl  = "http://192.168.1.100:3000/api/schedule";

// ================= DEKLARASI PIN ESP32 =================
const int pinBuzzer  = 14;
const int pinServo1  = 13;
const int pinServo2  = 12;
const int pinLED1    = 26;
const int pinLED2    = 27;
const int pinTombol1 = 4;

// Status tracker untuk logika buka-tutup kotak
bool statusKotak1 = false;
bool statusKotak2 = false;

// Variabel pendeteksi jumlah klik
unsigned long waktuTerakhirTekan = 0;
int jumlahKlik = 0;
const int jedaDoubleKlik = 400;

// Variabel polling API
unsigned long lastApiCheck = 0;
const unsigned long pollingInterval = 60000; // 60 detik

void setup() {
  Serial.begin(115200);

  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinTombol1, INPUT_PULLUP);

  digitalWrite(pinBuzzer, LOW);
  digitalWrite(pinLED1, LOW);
  digitalWrite(pinLED2, LOW);

  servo1.attach(pinServo1);
  servo2.attach(pinServo2);
  servo1.write(0);
  servo2.write(0);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  SMART MEDICINE  ");
  lcd.setCursor(0, 1);
  lcd.print("  Connecting...  ");

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(" Connecting.     ");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 1);
    lcd.print("  WiFi Connected ");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    lcd.setCursor(0, 1);
    lcd.print("  WiFi Failed    ");
  }

  delay(1500);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("1x Klik : Box 1 ");
  lcd.setCursor(0, 1);
  lcd.print("2x Klik : Box 2 ");

  // ---------- MANUAL MODE (TOMBOL) ----------
  if (digitalRead(pinTombol1) == LOW) {
    delay(50);
    jumlahKlik++;
    waktuTerakhirTekan = millis();

    digitalWrite(pinBuzzer, HIGH);
    delay(100);
    digitalWrite(pinBuzzer, LOW);

    while (digitalRead(pinTombol1) == LOW);
    delay(100);
  }

  if (jumlahKlik > 0 && (millis() - waktuTerakhirTekan > jedaDoubleKlik)) {
    if (jumlahKlik == 1) {
      statusKotak1 = !statusKotak1;
      if (statusKotak1) {
        servo1.write(90);
        digitalWrite(pinLED1, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 1 OPENED  ");
      } else {
        servo1.write(0);
        digitalWrite(pinLED1, LOW);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 1 CLOSED  ");
      }
      delay(1000);
      lcd.clear();
    } else if (jumlahKlik == 2) {
      statusKotak2 = !statusKotak2;
      if (statusKotak2) {
        servo2.write(90);
        digitalWrite(pinLED2, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 2 OPENED  ");
      } else {
        servo2.write(0);
        digitalWrite(pinLED2, LOW);
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 2 CLOSED  ");
      }
      delay(1000);
      lcd.clear();
    }
    jumlahKlik = 0;
  }

  // ---------- API POLLING (SETIAP 60 DETIK) ----------
  if (WiFi.status() == WL_CONNECTED && (millis() - lastApiCheck > pollingInterval)) {
    lastApiCheck = millis();
    checkApiSchedule();
  }
}

void checkApiSchedule() {
  HTTPClient http;
  http.begin(serverUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("API Response: " + payload);

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      JsonArray arr = doc.as<JsonArray>();

      for (JsonObject obj : arr) {
        const char* boxNum  = obj["box_number"];
        const char* medName = obj["medicine_name"];

        if (strcmp(boxNum, "1") == 0 && !statusKotak1) {
          statusKotak1 = true;
          servo1.write(90);
          digitalWrite(pinLED1, HIGH);
          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("  BOX 1 OPENED  ");
          lcd.setCursor(0, 1); lcd.print(medName);
          delay(2000);
          lcd.clear();
        } else if (strcmp(boxNum, "2") == 0 && !statusKotak2) {
          statusKotak2 = true;
          servo2.write(90);
          digitalWrite(pinLED2, HIGH);
          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("  BOX 2 OPENED  ");
          lcd.setCursor(0, 1); lcd.print(medName);
          delay(2000);
          lcd.clear();
        }
      }
    } else {
      Serial.println("JSON parse error");
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}
