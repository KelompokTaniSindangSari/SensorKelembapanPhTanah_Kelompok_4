#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "config.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void printLCDLine(int row, String text) {

    if (text.length() > 16) {
        text = text.substring(0, 16);
    }

    lcd.setCursor(0, row);
    lcd.print(text);

    for (int i = text.length(); i < 16; i++) {
        lcd.print(" ");
    }
}

void lcdMulai() {

    Wire.begin(LCD_SDA, LCD_SCL);
    lcd.init();
    lcd.backlight();
    lcd.clear();

    printLCDLine(0, "MONITORING TANAH");
    printLCDLine(1, "ESP32 STARTING");
}

void lcdStatus(String baris1, String baris2) {

    lcd.clear();
    printLCDLine(0, baris1);
    printLCDLine(1, baris2);
}

// Menyingkat status supaya muat di LCD 16 karakter.
String singkatStatus(String s) {
    if (s == "Sangat Asam") return "Sgt Asam";
    return s;
}

// Baris 1: pH + status, baris 2: kelembapan + status.
// Contoh:
//   pH:4,6 Asam
//   Lmb:45% Normal
void lcdTampilMonitoring(
    float ph,
    float volt,
    String statusPH,
    float kelembapan,
    String statusKelembapan,
    float adcPH,
    int adcMoisture
) {
    String phText = String(ph, 1);
    phText.replace(".", ",");              // 4.6 -> 4,6

    String lembapText = String(kelembapan, 0);

    printLCDLine(0, "pH:" + phText + " " + singkatStatus(statusPH));
    printLCDLine(1, "Lmb:" + lembapText + "% " + statusKelembapan);
}

#endif