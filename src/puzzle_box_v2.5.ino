// Puzzle Box Code
// dmf 4.19.11 - begun
// 4.20.11 - this is test code for the 'OFF' function of the LV Switch
//         - ok, the program works - switch supplies power, LED lights
// 4.25.11 - setup to communicate with the LCD Display
//         - ok
// 4.25.11 - setup to drive the Servo
//         - ok
// 4.28.11 - begin setup for GPS
// 4.30.11 - seems to be working ok... still needs testing...
// 6.1.11  - revisions following construction of working latch
// 6.8.11 - ok, this version is working beautifully, and I'm not sure why.  in the previous version ("...to_Lab"), 
//        - the 'Looking for GPS' stage could take quite some time or hang altogether. so I added the 
//        - Serial.print(theValue, BYTE); statements to track the GPS data.  This confirmed that there was no 
//        - hardware issue - the connections to the GPS were fine - serial data was being sent to the board as 
//        - expected, and the data could be parsed. So, what's changed? 
//        a) the Serial.begin statement, and the Serial.print statements for communicating with the computer
//        b) I changed nss.available && tinygps.encode(nss.read()) to if nss.available -> theValue = nss.read() -> if encode(theValue) -> ...
//        - I haven't tested the various hypotheses, and, since it works, I'm leaving the Serial.print statements in the working code
// 4.6.12 - for the future: make the location a logical definition at the top of this code, but also make the initial text lines 
//          defined, so that you can give a clue like 'go to the beach' or 'take your bike'. 
//        - also, plan to setup a point->point->point type logic so we can do a real 'hunt'.
// 4.14.12 - Leslie reported '800 miles away' type error when she got to location - possible 0.0 distance error? If so, it has to 
//        - be fixed in the TinyGPS::distance_between function. So, take a look at that after conversion to Arduino 1.0
// 4.18.12 - THIS VERSION COMPILES UNDER ARDUINO 1.0
// 6.24.13 - updated for June 2013
// 12.23.16 - revisions to try to move towards simplifying setup
//          put coordinates and initial messages in file "locations.h"; longer term, should set this dynamically (still have to 
//          cut and paste from the list and recompile).  
//
// 5.4.20 - migrating to platformio (skipping _v3, which attempts to implement two-location tracking).
//        Not working yet. *** problem with LiquidCrystal library *** 
// 5.4.20 A more elegant solution to location handling not requiring recompilation every time would be nice! 
//        To be done. 
//
// 

/* Based on puzzle_box_sample.pde - Sample Arduino Puzzle Box sketch for MAKE.
 COPYRIGHT (c) 2008-2011 MIKAL HART.  All Rights Reserved.
 This software is licensed under the terms of the Creative
 Commons "Attribution Non-Commercial Share Alike" license, version
 3.0, which grants the limited right to use or modify it NON-
 COMMERCIALLY, so long as appropriate credit is given and
 derivative works are licensed under the IDENTICAL TERMS.  
 For supporting libraries and more information see http://arduiniana.org.
 */

// Require one Library for the LCD Display
// Note that SEEED states there are compatibility issues and suggest downloading a
// modified Library @ http://www.seeedstudio.com/depot/images/product/LiquidCrystal.rar (not used)
// 5.4.20 - getting problems during compilation. 
#include <LiquidCrystal.h>   // Included with Arduino distribution

// Require one Library for the Servo
// See discussion @ http://arduiniana.org/libraries/pwmservo/
// The file is placed in ~/Documents/Arduino/libraries
// 5.4.20 - Installed PWMServo library through Platformio
#include <PWMServo.h>         // Distributed by Mikal Hart

// Require EEPROM Library to keep record of # of Attempts
#include <EEPROM.h>

// Require two Libraries for the GPS.  
// The first sets up Serial input, the second sets up the GPS.  
// See discussion @ http://arduiniana.org/libraries/newsoftserial 
// and http://arduiniana.org/libraries/tinygps. 
// TinyGPS is placed in ~/Documents/Arduino/libraries
#include <SoftwareSerial.h> 

// 5.4.20 - installed tinygps library through Platformio. 
// Note that there is now a 'tinygpsplus' library available. 
#include <TinyGPS.h>

// ASSIGN THE LOCATION by extracting from a list of 
// locations that are #defined in an external include file 
#include "locations.h"
// 5.4.20 File contains list of locations as: 
//     #define THE_PLACE                 36.794225, -71.091475
// from which list one is chosen to set the target coordinates
// for the compiled version as initialized here - 
static const double coordinates[] = {THE_PLACE};
// 5.4.20 The File also contains startup messages. A kluge for now. 
// DEFINE THE INITIAL TEXT MESSAGES
const char* initialText1[] = {StartupMessage2016A};
const char* initialText2[] = {StartupMessage2016B}; 
// 5.4.20 A more elegant solution not requiring
// recompilation every time would be nice. To be done. 

// Pin assignments for basic on/off functionality
static const int LV_Switch_Off = 12; // LV OFF Switch Pin 
static const int LED_Pin = 2; //  Button Switch LED Pin
// Pin assignments for the LCD Display 
// Pin locations at the LCD (& my corresponding Arduino assignments) - 
// GND is Pin 1 (GND) Supply Voltage Vdd is Pin 2 (5V), Vo is Pin 3 (GND)
// Enable is Pin 6 (Pin 6), RS is Pin 4 (Pin 5), RW is Pin 5 (Pin 7)
// Databus lines - DB4 is Pin 11 (Pin A2 = 16), DB5 is Pin 12 (Pin A3 = 17), 
//                 DB6 is Pin 13 (Pin A4 = 18), DB7 is Pin 14 (Pin A5 = 19)
// LED Backlight - LEDA is Pin 15 (3V), LEDK is Pin16 (GND)
// Note: these assignments are different from the example code!
static const int LCD_Enable = 6, LCD_RS = 5, LCD_RW = 7; 
static const int LCD_DB4 = 16, LCD_DB5 = 17, LCD_DB6 = 18, LCD_DB7 = 19; 
// Pin assignment for Servo control
static const int servo_control = 9;
// Pin assignment for GPS
// IMPORTANT! Note that these are the INVERSE of  
// the pin assignments AT the GPS! 
static const int GPSrx = 4, GPStx = 3; 

// Servo angle assignments 
// *** NEED TO ADJUST THESE FOR THE FINAL BOX CONSTRUCTION ***
static const int CLOSED_ANGLE = 90; // degrees
static const int OPEN_ANGLE = 45; // degrees
// EEPROM Offset 
// *** Don't know why this is required, yet ***
static const int EEPROM_OFFSET = 100;
// Set maximum # of attempts (<100)
static const int DEF_ATTEMPT_MAX = 99; 

// Create an AttemptCounter global
int attempt_counter;
// Create an interval global
unsigned long millisInt = millis();
// Create the LCD Display object
LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_Enable, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
// Create the Servo object
PWMServo servo;
// Create the Serial object
// 4.18.12 NewSoftSerial -> SoftwareSerial in the Arduino 1.0 core
SoftwareSerial nss(GPSrx, GPStx);
// Create the GPS object
TinyGPS tinygps;

// HOW CLOSE? 
// static const int RADIUS = 50;  // meters
static const int RADIUS = 50; // changed 6.24.13

// define the location values used in the legacy code: 
static const double DEST_LATITUDE = coordinates[0];
static const double DEST_LONGITUDE =  coordinates[1]; 

/* The Arduino setup() function */
void setup()
{

  /* DEBUG */
  /* establish a debug session with a host computer */
  Serial.begin(115200);

  // Initialize LV (Power Off) switch pin as OUTPUT and LOW 
  pinMode(LV_Switch_Off, OUTPUT);
  digitalWrite(LV_Switch_Off, LOW);

  // Initialize the PushButton LED pin as OUTPUT and HIGH (on)
  pinMode(LED_Pin, OUTPUT);
  digitalWrite(LED_Pin, HIGH);

  // Establish communication with the GPS
  nss.begin(4800);

  // Establish communication with 8x2 LCD 
  lcd.begin(16,2); 

  // Attach the servo motor
  servo.attach(servo_control);

  // Make sure Servo Latch is closed
  servo.write(CLOSED_ANGLE); 

  // Read the Attempt Counter
  attempt_counter = EEPROM.read(EEPROM_OFFSET);
  if (attempt_counter == 0xFF) // brand new EEPROM?
    attempt_counter = 0;
  // And increment it
  ++ attempt_counter; 

  // Display a Startup message (2s pause between displays)
 Msg(lcd,initialText1[0],initialText1[1], 1500);
 Msg(lcd,initialText2[0],initialText2[1], 1500); 

/* DEBUG */
       Serial.print(" message top: "); 
       Serial.println(initialText1[0]); 
       Serial.print(" message bottom ");
       Serial.println(initialText2[0]);
       Serial.print(" latitude "); 
       Serial.println(DEST_LATITUDE);
       Serial.print(" longitude ");
       Serial.println(DEST_LONGITUDE);
/* DEBUG */ 
       

  // clean up the transition to the "Looking for GPS" messages
  lcd.clear();
  delay(2000);

  // Test the Attempt Counter 
  // If too many attempts, shut off the system and quit
  // dmf 6.1.11 slience this test
  //  if (attempt_counter >= DEF_ATTEMPT_MAX){
  //  Msg(lcd, "No More", "Attempts", 2000);
  //  Msg(lcd, "That's", "It!", 2000);
  //  PowerOff();
  // } 

  // Display the # of Attempts so far
  // dmf 6.1.11 silence this
  //  Msg(lcd, "This is", "attempt", 2000); 
  //  MsgAttempts(lcd, attempt_counter, DEF_ATTEMPT_MAX, 2000);

  // And record the # of Attempts
  EEPROM.write(EEPROM_OFFSET, attempt_counter);

}

// a toggle for the user message
boolean lookingFlag = true; 

/* The Arduino loop() function */
void loop()
// So we sit here and wait for a good (complete) GPS signal and evaluate it
// If it takes too long, timeout and shut down
{

  // Notify user every 3 seconds
  if ((millis() - millisInt) > 3000){
    millisInt = millis();
    if (lookingFlag) {
      Msg(lcd,"Finding", "  you!", 500);
      lcd.clear();
      lookingFlag = !lookingFlag; 
    } 
    else {
      Msg(lcd,"Wait, be", "patient!", 500);
      lcd.clear();
      lookingFlag = !lookingFlag; 
    }
  }

  if (nss.available()){
    int theValue = nss.read();
    Serial.write(theValue);
    if (tinygps.encode(theValue)){
      float lat, lon;
      unsigned long fix_age;

      // Note: tinygps.encode() returns 1 only for RMC and GGA strings (not for GSV or GSA strings) -
      // these strings are the two that provide positional information. 
      Serial.println("\n Got a good string");

      tinygps.f_get_position(&lat, &lon, &fix_age); 
      if (fix_age != TinyGPS::GPS_INVALID_AGE){

        Serial.print(" fix_age: "); 
        Serial.println(fix_age); 

        Serial.print(" Latitude ");
        Serial.println(lat); 
        Serial.print(" Longitude "); 
        Serial.println(lon); 

        float distance_meters = TinyGPS::distance_between(lat,lon,DEST_LATITUDE,DEST_LONGITUDE);
        float distance_feet = distance_meters / 0.3048 ;

        // Succesful GPS find... 
        Msg(lcd,"I found","you!", 2000); 
        Msg(lcd,"You are","...", 2000); 
        // always report how far away you are... 
        if (distance_feet < 10560){
          // < 2 miles
          MsgDistance(lcd, (int) distance_feet, "feet", 5000);
        } 
        else {
          // note miles will round (down?)
          MsgDistance(lcd, (int) (distance_feet / 5280), "miles", 5000);
        }
        Msg(lcd,"from the","prize!", 2000);

        // Make a decision 
        if (distance_meters <= RADIUS){
          Msg(lcd,"You're","close!", 2000);
          // dmf change 5.31.11 (servo open before 'open me' msg)
          servo.write(OPEN_ANGLE);
          delay(2000);  // This is essential to allow servo to move
          Msg(lcd,"Open","me up!", 10000); 
        } 
        else {
          Msg(lcd,"You are","too far!", 2000);
          Msg(lcd,"Bring me","closer!", 2000); 
          Msg(lcd, "Bye!","   ",2000);
        }
        // Since we got a good measurement and made a decision...
        PowerOff();
      } // end of valid fix_age test
    } // end of tinygps.encode() got a good string test
  } // end of serial processing test

  // If nothing is happening, this turns off the box after 3 minutes
  if (millis() >= 180000) {
    // (note that millis will re-initialize at every power up)
    Msg(lcd,"I am"," lost!",2000); 
    Msg(lcd,"Try me","later!", 2000);
    Msg(lcd,"Bye!","  ", 2000); 
    PowerOff();
  } // end of timeout test

} // end of loop()

/* Called to shut off the system using the Pololu switch */
void PowerOff()
{  
  // Bring Pololu switch control pin HIGH to turn off 
  digitalWrite(LV_Switch_Off, HIGH);

  // Scary warning message
  delay(2000); 
  Msg(lcd,"Invalid","code!", 5000);

  // Back Door - if power is still being supplied to the system 
  // through the USB port, then after 2 minutes (update: 30s), open the latch
  // to allow access. 
  delay(30000);
  servo.write(OPEN_ANGLE);
  delay(2000);  // This is essential to allow the servo to move
  Msg(lcd,"Opening"," ", 5000); 

  // And reset the attempt_counter value saved in the EEPROM
  EEPROM.write(EEPROM_OFFSET, 0);

  // Leave the latch open, and quit
  exit(1); 

} 

/* A helper function to display messages of a specified duration */
void Msg(LiquidCrystal &lcd, const char *top, const char *bottom, unsigned long del)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(top);
  lcd.setCursor(0, 1);
  lcd.print(bottom);
  delay(del);
}

/* Helper function to display # of # */
void MsgAttempts(LiquidCrystal &lcd, int ntries, int nmax, unsigned long del)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ntries);
  lcd.print(" of ");
  lcd.print(nmax);
  delay(del);
}

/* Helper function to display miles to go */
void MsgDistance(LiquidCrystal &lcd, int howfar, const char* bottom, unsigned long del)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(howfar);
  lcd.setCursor(0,1);
  lcd.print(bottom);
  delay(del);
}
















