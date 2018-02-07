


/*HPLC injector
 * R.I.C.O.
 * Remote
 * Injector
 * and
 * fraction COllector
 *
  May 17th, 2015
  Victor Amador
  St. Jude Children's Research Hospital
  victor.amador@stjude.org


  CHANGE HISTORY
  v.1 - Works with an external toggle switch
        Accepts a flush button in case the preset time is not enough
        Reads from the Carroll$Ramsey M-101HC radiation detector/amplifier
        Only sends the trigger to Synthera and datalogger once for automatic injection based on LGS digital outputs

  VMA05 uses the Velleman VMA05 board.


  January 18th, 2017
  ALV requests modifications after introductory meeting:
  - no monitoring of overflow
      removed from the if that sets the value of the variable loop_status_2 to true

  - external indication of full loop and rheodyne rotation detection
      Added //EXTERNAL INDICATORS in definition
      will not switch to collection mode until injection has been detected

  - increase purge time to account for the flow restriction caused by the sterilizing filter in the FPV
    changed flushTime to 30s

*/

//LIBRARIES----------------------------------------------------------------------------------------

#include <microsmooth.h>

#include <Arduino.h>
#include <SPI.h>
#include <Event.h>
#include <Timer.h>
#include <Wire.h> //for LK162
//#include <LiquidCrystal_I2C.h>  //for Sainsmart 2004

#include <Adafruit_SSD1306.h> //128x32_i2c
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#define OLED_RESET 14
Adafruit_SSD1306 display(OLED_RESET);

#include <Adafruit_ADS1015.h>
Adafruit_ADS1015 ads(0x48);     /* Use this for the 12-bit version */

/*
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};
*/

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif



//Radiation detector amplifier---------------------------------------------------------
//const int RADpin = A0;
unsigned int RAD = 0;

//Radiation detector Calibration-------------------------------------------------------
float RADvoltage = 0;
//float activity_uCi = 0;

//microsmoothing
uint16_t *ptr;



void setup() {

  Wire.begin();

  ads.begin();

  //SERIAL------------------------------------------------------------------------------------------------------
  Serial.begin(9600);

  // Set up the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize with the I2C addr 0x3D if not working use 0x3C (for the 128x64)
  //display.setTextColor(WHITE);
  //display.display();
  //delay(2000);
  //display.clearDisplay();

  //pinMode (RADpin, INPUT);

  //microsmoothing
  ptr = ms_init(SMA);
  if (ptr == NULL) Serial.println("No memory");


}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loop() {

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV


  int16_t adc0, adc1, adc2, adc3;

  Serial.println(" ");


  String gain;
  float Vrange;
  int DACrange = 2048;
  int gainArray [6] = {0 , 512 , 1024 , 1536 , 2048 , 2560};
  Serial.print("gain: "); Serial.println(ads.getGain());

  if (ads.getGain() == gainArray[0]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[1]);
    //RADvoltage = ((RAD / 2047.0) * 6.144 * 2);      //convert ADC to volts
    gain = "x2/3";
    Vrange = 6.144;
  }
  if (ads.getGain() == gainArray[1]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[2]);
    //RADvoltage = ((RAD / 2047.0) * (4.096/2));      //convert ADC to volts
    //RADvoltage = RAD*5/2047;
    gain = "x1";
    Vrange = 4.096;
  }
  if (ads.getGain() == gainArray[2]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[3]);
    //float RADvoltage = ((RAD / 2047.0) * 2.048 * 2);      //convert ADC to volts
    gain = "x2";
    Vrange = 2.048;
  }
  if (ads.getGain() == gainArray[3]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[4]);
    //float RADvoltage = ((RAD / 2047.0) * 1.024 * 2);      //convert ADC to volts
    gain = "x4";
    Vrange = 1.024;
  }
  if (ads.getGain() == gainArray[4]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[5]);
    //float RADvoltage = ((RAD / 2047.0) * 0.512 * 2);      //convert ADC to volts
    gain = "x8";
    Vrange = 0.512;
  }
  if (ads.getGain() == gainArray[5]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[6]);
    //float RADvoltage = ((RAD / 2047.0) * 0.256 * 2);      //convert ADC to volts
    gain = "x8";
    Vrange = 0.256;
  }

  //single ended method
  //Channel to A0 of the DAC
  adc0 = ads.readADC_SingleEnded(0);
  RAD = adc0;
  float RADvoltage = RAD * (Vrange / DACrange);

  /*differential method
  //connect +Sensor to A0 of the DAC
  //connect +V to sensor to A1 of the DAC
  //In the Model-101 this means
  //Channel to A0
  //+12V to A1 -------THIS DAMAGES THE DAC, use single ended method
  //RAD = ads.readADC_Differential_0_1();
  */

  //Smoothing http://www.honfablab.org/wordpress/alexandria/technology/30Arduino_Projects_EviGenius.pdf
  static double oldValue = 0;
  float alpha = 0.5;                //smoothing percentage
  RAD = alpha * oldValue + (1 - alpha) * RAD;
  oldValue = RAD;

  Serial.print("RAD: "); Serial.println(RAD);
  Serial.print("RADvoltage: "); Serial.print(RADvoltage); Serial.println (" Volts");
  float activity_mCi = (RADvoltage * 10);        //placeholder Volts to mCi calibration factor
  Serial.print("activity_mCi: "); Serial.print(activity_mCi); Serial.println (" mCi");
  float activity_uCi = (activity_mCi * 1000);        //mCi to uCi
  Serial.print("activity_uCi: "); Serial.print(activity_uCi); Serial.println (" uCi");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(X, Y);
  display.setCursor(0, 0);
  display.print("RAD");
  display.setCursor(40, 0);
  display.println(RAD);

  display.setCursor(90, 0);
  display.print(float2OLED (RADvoltage, 1, 3));
  display.setCursor(105, 0);
  display.print(" V");

  display.setCursor(0, 10);
  display.print("activity_mCi:");
  display.setCursor(80, 10);
  display.print(float2OLED (activity_mCi, 1, 4));
  display.setCursor(105, 10);
  display.print("mCi");

  display.setCursor(0, 24);
  display.print("range:");
  display.setCursor(35, 24);
  display.print(float2OLED (Vrange, 3, 5));

  display.setCursor(73, 24);
  display.print("gain:");
  display.setCursor(102, 24);
  display.print(gain);

  display.display();


  //DisplayActivityOLED ();
  //sma_filter(RAD, ptr); //microsmoothing
  delay(250);


}
