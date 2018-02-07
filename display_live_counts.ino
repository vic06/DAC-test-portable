/*
 * 1. read the analog input                   readAnalog()
 * 2. smooth it                               smooth()
 * 3. convert ADC value from int to float
 * 4. convert ADC (float) to Volts (float)
 * 5. convert Volts (float) to PSI (float)
 * 6. convert PSI (float) to kPa (float)
 * 7. convert kPa (float) to a String         float_i2c()
 * 5. convert the String to a charArray       x.tocharArray()
 * 6. send the charArray to the LCD via i2c   Wire.write()
 *
 * Order of Nested functions:
 * DisplayCounts ()
 *    1. acquire ADC
 *    2. smooth(ADC)
 *    3. convert smooth(ADC) to kPa
 *    4. float_i2c (int kPa) = String kPa
 *    5. SendToLCDPressureKPA(String to charArray)
 *
 *Fully Nested:
 *DisplayCounts( SendToLCDPressureKPA ( to.charArray ( float_i2c ( kPa ( PSI ( smooth ( readAnalog (pressureSENSORvalue))))))))
 *
 */




// DISPLAY RADIATION ON THE OLED DISPLAY -----------------------------------------------

void DisplayActivityOLED () {
/*
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  //  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV


  int16_t adc0, adc1, adc2, adc3;

  Serial.println(" ");
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  /*
  Serial.print("AIN0: "); Serial.println(adc0);
  Serial.print("AIN1: "); Serial.println(adc1);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.println(" ");
  */
/*
  String gain;

  float gainArray [6] = {0.0 , 512.0 , 1024.0 , 1536.0 , 2048.0 , 2560.0};

  Serial.print("gain: "); Serial.println(ads.getGain());

  if (ads.getGain() == gainArray[0]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[1]);
    //RADvoltage = ((RAD / 2047.0) * 6.144 * 2);      //convert ADC to volts
    gain = "x2/3";
  }
  if (ads.getGain() == gainArray[1]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[2]);
    //RADvoltage = ((RAD / 2047.0) * (4.096/2));      //convert ADC to volts
    //RADvoltage = RAD*5/2047;
    gain = "x1";
  }
  if (ads.getGain() == gainArray[2]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[3]);
    //float RADvoltage = ((RAD / 2047.0) * 2.048 * 2);      //convert ADC to volts
  }
  if (ads.getGain() == gainArray[3]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[4]);
    //float RADvoltage = ((RAD / 2047.0) * 1.024 * 2);      //convert ADC to volts
  }
  if (ads.getGain() == gainArray[4]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[5]);
    //float RADvoltage = ((RAD / 2047.0) * 0.512 * 2);      //convert ADC to volts
  }
  if (ads.getGain() == gainArray[5]) {
    //Serial.print("gainArray: "); Serial.println(gainArray[6]);
    //float RADvoltage = ((RAD / 2047.0) * 0.256 * 2);      //convert ADC to volts
  }


  //RAD = adc0;

  //RAD = smooth (adc0, 2);
  //RAD = pulseIn(RAD, HIGH, 100); //microsmoothing


  int RAD2 = adc1;
  float RAD2voltage = RAD2 * (5.0 / 2048.0);

  Serial.print("RAD: "); Serial.println(RAD);
  //RAD = smoothPercentage(RAD, 0.8);
  
  float  value, oldvalue = 0, alpha=0.7;
  RAD = alpha * oldvalue + (1 - alpha) * adc0;
  oldvalue = RAD;

  Serial.print("RAD: "); Serial.println(RAD);

  //single ended method
  float RADvoltage = RAD * (5.0 / 2048.0);
  Serial.print("RADvoltage: "); Serial.print(RADvoltage); Serial.println (" Volts");

  float activity_mCi = (RADvoltage * 10);        //placeholder Volts to mCi calibration factor
  Serial.print("activity_mCi: "); Serial.print(activity_mCi); Serial.println (" mCi");

  float activity_uCi = (activity_mCi * 1000);        //mCi to uCi
  Serial.print("activity_uCi: "); Serial.print(activity_uCi); Serial.println (" uCi");

  /*
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(X, Y);
  display.setCursor(10, 0);
  display.println("RADIATION DETECTOR");
  display.setTextSize(3);
  display.setCursor(27, 11);
  display.print(float2OLED (activity_mCi));
  display.setTextSize(2);
  display.setCursor(85, 18);
  display.print("mCi");
  display.display();

  */




/*

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(X, Y);
  display.setCursor(0, 0);
  display.print("A0:");
  display.setCursor(20, 0);
  display.println(RAD);

  display.setCursor(70, 0);
  display.print(float2OLED (RADvoltage, 1, 3));
  display.setCursor(90, 0);
  display.print(" Volts");

  display.setCursor(0, 8);
  display.print("A1:");
  display.setCursor(20, 8);
  display.println(RAD2);

  display.setCursor(70, 8);
  display.print(float2OLED (RAD2voltage, 1, 3));
  display.setCursor(90, 0);
  display.print(" Volts");

  display.setCursor(0, 16);
  display.print("activity_mCi:");
  display.setCursor(80, 16);
  display.print(float2OLED (activity_mCi, 1, 4));
  display.setCursor(105, 16);
  display.print("mCi");
  /*
    display.setCursor(0, 30);
    display.print("activity_uCi:");
    display.setCursor(80, 30);
    display.print(float2OLED (activity_uCi, 0, 3));
    display.setCursor(105, 30);
    display.print("uCi");
  
  display.setCursor(0, 24);
  display.print("gain:");
  display.setCursor(40, 24);
  display.print(ads.getGain());
  display.setCursor(80, 24);
  display.print(gain);

  display.display();

*/

  /*
    if (activity_mCi >= 1000.0) {

      String StringRAD = float_i2c(activity_mCi, 0, 3);

      char char_activity_mCi[50];
      StringRAD.toCharArray(char_activity_mCi, 50);

      char char_ten[50];
      char char_one_hundred[50];

      String String_ten = float_i2c(10.0, 1, 6);
      String String_one_hundred = float_i2c(100.0, 1, 6);

      String_ten.toCharArray(char_ten, 50);
      String_one_hundred.toCharArray(char_one_hundred, 50);


      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      //display.setCursor(X, Y);
      display.setCursor(10, 0);
      display.println("RADIATION DETECTOR");
      display.setTextSize(3);
      display.setCursor(27, 11);
      display.print(char_activity_mCi);
      display.setTextSize(2);
      display.setCursor(85, 18);
      display.print("mCi");
      display.display();

    }



    if (activity_mCi < 1000.0) {

      String StringRAD = float_i2c(activity_uCi, 0, 3);

      char char_activity_uCi[50];
      StringRAD.toCharArray(char_activity_uCi, 50);

      char char_ten[50];
      char char_one_hundred[50];

      String String_ten = float_i2c(10.0, 1, 6);
      String String_one_hundred = float_i2c(100.0, 1, 6);

      String_ten.toCharArray(char_ten, 50);
      String_one_hundred.toCharArray(char_one_hundred, 50);


      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      //display.setCursor(X, Y);
      display.setCursor(10, 0);
      display.println("RADIATION DETECTOR");
      display.setTextSize(3);
      display.setCursor(27, 11);
      display.print(char_activity_uCi);
      display.setTextSize(2);
      display.setCursor(85, 18);
      display.print("uCi");
      display.display();

    }
  */

}

/* BACKUP OF THE RADIATION DETECTOR SCREEN
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    //display.setCursor(X, Y);
    display.setCursor(10, 0);
    display.println("RADIATION DETECTOR");
    display.setTextSize(3);
    display.setCursor(27, 11);
    display.print(char_activity_mCi);
    display.setTextSize(2);
    display.setCursor(85, 18);
    display.print("mCi");
    display.display();
*/



//SMOOTHING---------------------------------------------------------------
float smooth(int sensorValue, int numReadings) {
  //const int numReadings = 2;

  int readings[numReadings];      // the readings from the analog input
  int index = 0;                  // the index of the current reading
  int total = 0;                  // the running total
  int average = 0;                // the average

  // subtract the last reading:
  total = total - readings[index];
  // read from the sensor:
  readings[index] = sensorValue;
  // add the reading to the total:
  total = total + readings[index];
  // advance to the next position in the array:
  index = index + 1;

  // if we're at the end of the array...
  if (index >= numReadings)
    // ...wrap around to the beginning:
    index = 0;

  // calculate the average:
  average = total / numReadings;
  delay(1);
  return (average);
}



String float_i2c (float number, uint8_t precision, uint8_t size) {
  // Based on mem,  16.07.2008
  // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num = 1207226548/6#6
  // ---
  // avenue33, April 10th, 2010
  // Added rounding, size and overflow #
  // ftoa(343.1453, 2, 10) -> "    343.15"
  // ftoa(343.1453, 4,  7) -> "#      "

  String s = "";

  // Negative
  if (number < 0.0)  {
    s = "-";
    number = -number;
  }

  double rounding = 0.5;
  for (uint8_t i = 0; i < precision; ++i)    rounding /= 10.0;

  number += rounding;
  s += String(uint16_t(number));  // prints the integer part

  if (precision > 0) {
    s += ".";                // prints the decimal point
    uint32_t frac;
    uint32_t mult = 1;
    uint8_t padding = precision - 1;
    while (precision--)     mult *= 10;

    frac = (number - uint16_t(number)) * mult;

    uint32_t frac1 = frac;
    while (frac1 /= 10)    padding--;
    while (padding--)      s += "0";

    s += String(frac, DEC) ; // prints the fractional part
  }

  if (size > 0)              // checks size
    if (s.length() > size)        return ("#");
    else while (s.length() < size) s = " " + s;

  return s;

}

String float2OLED(float x, uint8_t precision, uint8_t size) {

  String String_x = float_i2c(x, precision, size);

  char char_x[50];
  String_x.toCharArray(char_x, 50);

  char char_ten[50];
  char char_one_hundred[50];

  String String_ten = float_i2c(10.0, 1, 6);
  String String_one_hundred = float_i2c(100.0, 1, 6);

  String_ten.toCharArray(char_ten, 50);
  String_one_hundred.toCharArray(char_one_hundred, 50);

  return String_x;

}

int smoothPercentage (int rawvalue, float percent) {
  int value, oldvalue = 0;
  value = percent * oldvalue + (1 - percent) * rawvalue;
  oldvalue = value;
  return value;
}

