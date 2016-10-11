////////
// Libraries Arduino
//
// Library: Remote debug - debug over telnet - for Esp8266 (NodeMCU)
// Author: Joao Lopes
//
// Attention: This library is only for help development. Please not use this in production
//
// First sample to show how to use it - basic one
//
// Example of use:
//
//        if (Debug.ative(Debug.<level>)) { // <--- This is very important to reduce overheads and work of debug levels
//            Debug.printf("bla bla bla: %d %s\n", number, str);
//            Debug.println("bla bla bla");
//        }
//
//
///////

// Libraries

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>


//libraries for OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// Remote debug over telnet - not recommended for production, only for development

#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug
RemoteDebug Debug;

// SSID and password
const char* ssid = "ORCE";
const char* password = "1f000000f1";

// Host mDNS

#define HOST_NAME "remotedebug-sample"

// Time

uint32_t mLastTime = 0;
uint32_t mTimeSeconds = 0;

// Buildin Led ON ?

boolean mLedON = false;

////// Setup

void CustomCommandsFunction(){
	Debug.printf("Last TelnetCommand: %s\r\n",Debug.getLastCommand().c_str());

	if(Debug.getLastCommand()=="R-On"){
		pinMode( 12, OUTPUT );
		digitalWrite(12,true);
		Debug.println("Pin12 - Hight");
	}
	if(Debug.getLastCommand()=="R-Off"){
		pinMode( 12, OUTPUT );
		digitalWrite(12,false);
		Debug.println("Pin12 - Low");
	}
	//Debug.printf("Chip id %x08\r\n",ESP.getChipId());
}

void setup() {

    // Initialize the Serial (educattional use only, not need in production)

    Serial.begin(115200);

    /**************************
    	WIFI setup
     *
     */
    // WiFi connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Register host name in WiFi and mDNS

    String hostNameWifi = HOST_NAME;
    hostNameWifi.concat(".local");

    WiFi.hostname(hostNameWifi);


    /**************************
    	RemoteDebug setup
     *
     */

    if (MDNS.begin(HOST_NAME)) {
        Serial.print("* MDNS responder started. Hostname -> ");
        Serial.println(HOST_NAME);
    }

    MDNS.addService("telnet", "tcp", 23);

    // Initialize the telnet server of RemoteDebug

    Debug.begin(HOST_NAME); // Initiaze the telnet server

    Debug.setResetCmdEnabled(true); // Enable the reset command

    //add available custom commands that will be displayed at on help screen
    String helpCmd =  "LED2-toggle\r\n";
    helpCmd.concat ("R-On -> (Relais On)\r\n");
    helpCmd.concat ("R-Off -> (Relais Off)\r\n");

    Debug.setHelpProjectsCmds(helpCmd);
    Debug.setCallBackProjectCmds(&CustomCommandsFunction);

    //Debug.showTime(true); // To show time

    // Debug.showProfiler(true); // To show profiler - time between messages of Debug
                              // Good to "begin ...." and "end ...." messages

    // This sample (serial -> educattional use only, not need in production)

    Serial.println("* Arduino RemoteDebug Library");
    Serial.println("*");
    Serial.print("* WiFI connected. IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("*");
    Serial.println("* Please use the telnet client (telnet for Mac/Unix or putty and others for Windows)");
    Serial.println("*");
    Serial.println("* This sample will send messages of debug in all levels.");
    Serial.println("*");
    Serial.println("* Please try change debug level in telnet, to see how it works");
    Serial.println("*");

    /**************************
    	ArduinoOTA setup
     *
     */

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void loop()
{
    // Each second

    if ((millis() - mLastTime) >= 1000) {

        // Time

        mLastTime = millis();

        mTimeSeconds++;

        //Debug.printf("Free Heap %d\n\r",ESP.getFreeHeap());

     }

    // Remote debug over telnet

    Debug.handle();
    ArduinoOTA.handle();

    // Give a time for ESP8266

    yield();

}

/////////// End
