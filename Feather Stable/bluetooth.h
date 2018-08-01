
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif


boolean even = true;

/*=========================================================================
    APPLICATION SETTINGS
â€‚ â€‚ FACTORYRESET_ENABLEâ€‚   â€‚  Perform a factory reset when running this sketch
â€‚ â€‚
â€‚ â€‚                           Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
â€‚ â€‚                           running this at least once is a good idea.
â€‚ â€‚
â€‚ â€‚                           When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.â€‚ If you are making changes to your
â€‚ â€‚                           Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.â€‚ Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
â€‚ â€‚ â€‚ â€‚
â€‚ â€‚                           Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
    #define REPORTING_PERIOD_MS     1000
/*=========================================================================*/


Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void bluetoothSetup(){
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) ){
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE ){
    /* Perform a factory reset to make sure everything is in a known state */
    //Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) ){
    // Change Mode LED Activity
    Serial.println(F("******************************"));
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    Serial.println(F("******************************"));
  }
}

// upload the data to the connected bluetooth device
void uploadData(float angle, float roll, int front, int back, float FMG, int batteryLevel, int stepcount){
  // need to create the string with all the data
  String dataString;
  if(even){
    dataString = "A," + String((int)angle) + "," + String((int)roll) + "," + String(front) + "," + String(back) + "\n";
    even = false;
  }else{
    dataString = "B," + String((int)FMG) + "," + String(batteryLevel) + "," + String(stepcount) + "\n";
    even = true;
  }
  
  //Serial.println("Uploading data");
  ble.print("AT+BLEUARTTX=");
  ble.println(dataString);
  //Serial.println(dataString);
  
  // check response stastus
  if (! ble.waitForOK() ) {
    Serial.println(F("Failed to send?"));
  }
  //Serial.println("Done uploading");
}






