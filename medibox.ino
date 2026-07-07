#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo1;
Servo servo2;

// ================= DEKLARASI PIN ESP32 =================
const int pinBuzzer  = 14; 
const int pinServo1  = 13;
const int pinServo2  = 12;
const int pinLED1    = 26; 
const int pinLED2    = 27; 
const int pinTombol1 = 4;  // Satu-satunya tombol utama yang dipakai

// Status tracker untuk logika buka-tutup kotak
bool statusKotak1 = false;
bool statusKotak2 = false;

// Variabel pendeteksi jumlah klik
unsigned long waktuTerakhirTekan = 0;
int jumlahKlik = 0;
const int jedaDoubleKlik = 400; // Toleransi waktu jeda ketukan (400 milidetik)

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
  lcd.print(" MULTI-CLICK SYS ");
  delay(1500);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("1x Klik : Box 1 ");
  lcd.setCursor(0, 1);
  lcd.print("2x Klik : Box 2 ");

  // 1. MEMBACA KETUKAN TOMBOL
  if (digitalRead(pinTombol1) == LOW) {
    delay(50); // Debounce stabilisasi fisik tombol
    jumlahKlik++;
    waktuTerakhirTekan = millis(); // Catat waktu penekanan terakhir
    
    // Bunyi konfirmasi setiap kali tombol tertekan fisik
    digitalWrite(pinBuzzer, HIGH);
    delay(100);
    digitalWrite(pinBuzzer, LOW);
    
    while(digitalRead(pinTombol1) == LOW); // Tunggu sampai tombol dilepas
    delay(100);
  }

  // 2. LOGIKA KEPUTUSAN JUMAH KLIK (Setelah user selesai mengetuk)
  if (jumlahKlik > 0 && (millis() - waktuTerakhirTekan > jedaDoubleKlik)) {
    
    // ----------- JIKA DIKLIK 1 KALI (KOTAK 1) -----------
    if (jumlahKlik == 1) {
      statusKotak1 = !statusKotak1; // Balikkan status
      
      if (statusKotak1) {
        servo1.write(90);            // Buka Kotak 1
        digitalWrite(pinLED1, HIGH); 
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 1 OPENED  ");
      } else {
        servo1.write(0);             // Tutup Kotak 1
        digitalWrite(pinLED1, LOW);  
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 1 CLOSED  ");
      }
      delay(1000); // Jeda tampilan LCD
      lcd.clear();
    } 
    
    // ----------- JIKA DIKLIK 2 KALI (KOTAK 2) -----------
    else if (jumlahKlik == 2) {
      statusKotak2 = !statusKotak2; // Balikkan status
      
      if (statusKotak2) {
        servo2.write(90);            // Buka Kotak 2
        digitalWrite(pinLED2, HIGH); 
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 2 OPENED  ");
      } else {
        servo2.write(0);             // Tutup Kotak 2
        digitalWrite(pinLED2, LOW);  
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("  BOX 2 CLOSED  ");
      }
      delay(1000);
      lcd.clear();
    }
    
    // Reset hitungan untuk mendengarkan klik berikutnya
    jumlahKlik = 0; 
  }
}

