// Simple example to embed pictures in your sketch
// and draw on the ILI9341 display with writeRect()
//
// By Frank Bösing
//
// https://forum.pjrc.com/threads/32601-SPI-Library-Issue-w-ILI9341-TFT-amp-PN532-NFC-Module-on-Teensy-3-2?p=94534&viewfull=1#post94534

#include <Arduino.h>
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>

// Converted to code with:
// http://www.rinkydinkelectronics.com/t_imageconverter565.php
//
#include "MenuPNGPage1.c" //Home
#include "MenuPNGPage2.c" //Effets
#include "MenuPNGPage3.c" //Analyseur

#define BUTTON_PIN 21

// Écran pins
#define TFT_DC      9
#define TFT_CS      10
#define TFT_RST     34  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

// Touch Screen pins
#define CS_PIN 8
#define Tirq 2

// Calibration datas
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Box Sizes
#define BOXSIZE 40
#define PENRADIUS 0.8

// Pad Constantes
#define PADSIZE 45
bool EtatBouton1 = 0;
bool EtatBouton2 = 0;
bool EtatBouton3 = 0;
bool EtatBouton4 = 0;
bool EtatBouton5 = 0;
bool EtatBouton6 = 0;
bool EtatBouton7 = 0;
bool EtatBouton8 = 0;
bool EtatBouton9 = 0;

// Etat Bouton Variables
int Bouton = 1;

// Etat Menu Variables
int Etat = 1;

// Volume Variables
float Slide = 96;
float SlideBuffer = 96;
float PourcentageVolume = 50;

// Mixer Variables
float Mixer1 = 140, Mixer2 = 140, Mixer3 = 140, Mixer4 = 140;
float Mixer1Buffer = 140, Mixer2Buffer = 140, Mixer3Buffer = 140, Mixer4Buffer = 140;
float PourcentageMixer1 = 0, PourcentageMixer2 = 0, PourcentageMixer3 = 0, PourcentageMixer4 = 0;


Button button = Button();
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
XPT2046_Touchscreen ts(CS_PIN, Tirq);

//Print
void ImagePrint(void);
void VolumePrint(void);
void MixerPrint(void);
void PadPrint(char);
void ReprintPads(void);
//Test
void TestBoutonAppuye(void);
void TestTactilAppuye(void);
void TestPadAppuye(void);

void setup() {

  Serial.begin(9600);
  Serial.println(F("Touch Paint!"));

  button.attach( BUTTON_PIN, INPUT ); // USE EXTERNAL PULL-UP
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button.interval(5);
  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button.setPressedState(LOW);

  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  ImagePrint();
  VolumePrint();

}

void loop(void) {

  button.update();

  // Si bouton appuyé
  if ( button.pressed() ) {
    TestBoutonAppuye();
  }

  // Si ecran tactile appuyé
  else if (ts.touched()) {
    TestTactilAppuye();
  }

  if (Serial.available()) {
    TestPadAppuye();
  }

  delay(100);

}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Affichage Image                            */
//////////////////////////////////////////////////////////////////////////////////////

void ImagePrint() {

  switch (Etat) {
    case 1:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      ReprintPads();

      break;

    case 2:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage2);

      tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK);
      tft.fillRoundRect(279, 26, 10, 136, 5, ILI9341_BLACK);

      tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);
      tft.drawRoundRect(279, 26, 10, 136, 5, ILI9341_WHITE);

      break;

    case 3:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage3);
      break;

    default:
      //tft.fillScreen(ILI9341_BLACK);
      tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Volume                              */
//////////////////////////////////////////////////////////////////////////////////////

void VolumePrint() {
  tft.fillRect(73, 2, int16_t(Slide), 16/*+1*/, ILI9341_RED);
  tft.setCursor(48, 8);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(int16_t(PourcentageVolume)); tft.println('%');
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Mixer1                              */
//////////////////////////////////////////////////////////////////////////////////////

void MixerPrint() {
  tft.fillRoundRect(80, Mixer1, 44, 22, 5, ILI9341_BLACK); //97-22+5 | 162-22
  tft.fillRoundRect(141, Mixer2, 44, 22, 5, ILI9341_BLACK); //158-22+5 | 162-22
  tft.fillRoundRect(201, Mixer3, 44, 22, 5, ILI9341_BLACK); //218-22+5 | 162-22
  tft.fillRoundRect(262, Mixer4, 44, 22, 5, ILI9341_BLACK); //279-22+5 | 162-22

  tft.drawRoundRect(80, Mixer1, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(141, Mixer2, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(201, Mixer3, 44, 22, 5, ILI9341_WHITE);
  tft.drawRoundRect(262, Mixer4, 44, 22, 5, ILI9341_WHITE);

  tft.setCursor(80, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer1:");
  tft.setCursor(77, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer1)); tft.println('%');

  tft.setCursor(141, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer2:");
  tft.setCursor(138, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer2)); tft.println('%');

  tft.setCursor(202, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer3:");
  tft.setCursor(199, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer3)); tft.println('%');

  tft.setCursor(263, 182);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Mixer4:");
  tft.setCursor(260, 200);
  tft.setTextSize(3);
  tft.print(int16_t(PourcentageMixer4)); tft.println('%');
}

//////////////////////////////////////////////////////////////////////////////////////
/*                           Fonction Affichage Pad                                 */
//////////////////////////////////////////////////////////////////////////////////////

void PadPrint(char bouton) {

  switch (bouton) {
    case '7':

      EtatBouton1 = !EtatBouton1;

      if (EtatBouton1 == 1) {
        tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '8':

      EtatBouton2 = !EtatBouton2;

      if (EtatBouton2 == 1) {
        tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '9':

      EtatBouton3 = !EtatBouton3;

      if (EtatBouton3 == 1) {
        tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '4':

      EtatBouton4 = !EtatBouton4;

      if (EtatBouton4 == 1) {
        tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '5':

      EtatBouton5 = !EtatBouton5;

      if (EtatBouton5 == 1) {
        tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '6':

      EtatBouton6 = !EtatBouton6;

      if (EtatBouton6 == 1) {
        tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '1':

      EtatBouton7 = !EtatBouton7;

      if (EtatBouton7 == 1) {
        tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '2':

      EtatBouton8 = !EtatBouton8;

      if (EtatBouton8 == 1) {
        tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    case '3':

      EtatBouton9 = !EtatBouton9;

      if (EtatBouton9 == 1) {
        tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
        tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      else {
        tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
        tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
      }
      break;

    default:
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Bouton                                */
//////////////////////////////////////////////////////////////////////////////////////

void TestBoutonAppuye() {

  if (Bouton <= 3) {
    Bouton = Bouton + 1;
  }
  else {
    Bouton = 1;
  }

  switch (Bouton) {
    case 1:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
      break;

    case 2:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 2;
      ImagePrint();
      VolumePrint();
      break;

    case 3:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 3;
      ImagePrint();
      VolumePrint();
      break;

    default:
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Tactil                                */
//////////////////////////////////////////////////////////////////////////////////////

void TestTactilAppuye() {

  // Retrieve a point
  TS_Point p = ts.getPoint();

  // Scale using the calibration #'s
  // and rotate coordinate system
  p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
  p.y = map(p.y, TS_MAXX, TS_MINX, 0, tft.width());

  Serial.print("X = "); Serial.print(p.x);
  Serial.print("\tY = "); Serial.print(p.y);
  Serial.print("\tPressure = "); Serial.println(p.z);

  //Boutons tactils
  if (p.x > 180) {

    if ((p.y > 40) && (p.y < 120)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
    } else if ((p.y > 130) && (p.y < 210)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 2;
      ImagePrint();
      VolumePrint();
      MixerPrint();
    } else if ((p.y > 255) && (p.y < 304)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 3;
      ImagePrint();
      VolumePrint();
    }
  }
  //Volume
  else if ((p.y >= 0) && (p.y < 30)) {
    if ((p.x > 50) && (p.x < 180)) {
      Slide = float((p.x - 181.0) / -0.6842);
      PourcentageVolume = float(100.0 * Slide / 190.0);
      Serial.println(PourcentageVolume);
      Serial.println(Slide);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      if ((Slide > SlideBuffer + 4) || (Slide < SlideBuffer - 4)) {
        ImagePrint();
        VolumePrint();
        if (Etat == 2) {
          MixerPrint();
        }

      }
      SlideBuffer = Slide;
    }
  }
  //Mixer1
  else if ((Etat == 2) && ((p.x >= 150) && (p.x < 175))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer1 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer1 = float(-0.877143 * Mixer1 + 122.80702);
      Serial.print(PourcentageMixer1); Serial.println("%");
      Serial.println(Mixer1);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer2
  else if ((Etat == 2) && ((p.x >= 110) && (p.x < 135))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer2 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer2 = float(-0.877143 * Mixer2 + 122.80702);
      Serial.print(PourcentageMixer2); Serial.println("%");
      Serial.println(Mixer2);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer3
  else if ((Etat == 2) && ((p.x >= 70) && (p.x < 95))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer3 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer3 = float(-0.877143 * Mixer3 + 122.80702);
      Serial.print(PourcentageMixer3); Serial.println("%");
      Serial.println(Mixer3);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
  //Mixer4
  else if ((Etat == 2) && ((p.x >= 25) && (p.x < 50))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer4 = float((0.67059 * p.y) - 0.7);
      PourcentageMixer4 = float(-0.877143 * Mixer4 + 122.80702);
      Serial.print(PourcentageMixer4); Serial.println("%");
      Serial.println(Mixer4);
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
      ImagePrint();
      VolumePrint();
      MixerPrint();
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////
/*                              Fonction Test Pad                                   */
//////////////////////////////////////////////////////////////////////////////////////

void TestPadAppuye() {
  char receivedChar = Serial.read();
  PadPrint(receivedChar);
}

//////////////////////////////////////////////////////////////////////////////////////
/*                              Reprint Pads                                        */
//////////////////////////////////////////////////////////////////////////////////////

void ReprintPads() {
  if (EtatBouton1 == 1) {
    tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton2 == 1) {
    tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton3 == 1) {
    tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton4 == 1) {
    tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton5 == 1) {
    tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton6 == 1) {
    tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton7 == 1) {
    tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton8 == 1) {
    tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton9 == 1) {
    tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
}
