/*
  Build status stop light code - Wifi
  V 2.0

  This program can set the stop light from eiter serial commands or a web page.
  This program was initially developed for a MakerGO ESP32 C6 SuperMini

  For the web page this supports Access Point and Client modes in parallel.
  You can trigger state switches via get request and there is a JSON API on the /state subpage.
  The subpage links for states and actions are the same as the serial codes

  For serial controls there are different messages for the states as well as actions.
  States:
  code: i; Initialization animation
  code: o; Turn off all of the LEDs
  code: b; Flash the yellow LED, but dim the transitions
  code: s; Light up only the green LED
  code: u; Light up only the yellow LED
  code: f; Light up only the red LED
  code: p; Light up both yellow and red
  code: n; Flash the yellow light, but with no dimming
  code: w; Flash the red and green LEDs at the same time
  code: a; Alternate between yellow and the other LEDs
  code: r; Rolling animation from red to green
  code: c; Run a normal stop light animation
  code: $; Flash the LEDs at random times and brightness levels
  Actions:
  code: e; Enables the sleep mode feature to turn the light off after 1 hour. This is persisted to the EEPROM.
  code: d; Disables the sleep mode feature to turn the light off after 1 hour. This is persisted to the EEPROM.
 */

#include <WiFi.h>
#include <EEPROM.h>
#include "state_machine.h"
#include "pins.h"
#include "wifi_config.h"
#include "wifi_msg.h"

// Serial speed definition
#define SERIAL_SPEED 38400

#define SLEEP_STATE_ADDR 0x000 // Location of the sleep state in the EEPROM that is read on start

int incoming_byte = 0;         // for incoming serial data
NetworkServer server(80);      // Webserver for WiFi controls

void setup()
{
  // initialize the serial interface
  Serial.begin(SERIAL_SPEED);

  // setup the pins of the leds
  pinMode(LED_RD, OUTPUT);
  pinMode(LED_YE, OUTPUT);
  pinMode(LED_GN, OUTPUT);

  // read from an unconnected analog input to initialize the random function
  randomSeed(analogRead(0));

  // Initialize EEPROM
  if (!EEPROM.begin(16)) {
    Serial.println("failed to initialize EEPROM");
    analogWrite(LED_RD, 255);
    analogWrite(LED_YE, 255);
    analogWrite(LED_GN, 255);
    active_state = STATE_OFF;
    delay(5000);
  }
  // initialize sleep_active from EEPROM
  incoming_byte = EEPROM.read(SLEEP_STATE_ADDR);
  if(incoming_byte > 1) {
    // Initialize the EEPROM with true if the value is not valid
    EEPROM.write(SLEEP_STATE_ADDR, true);
    EEPROM.commit();
    incoming_byte = 1;
  }
  sleep_active = incoming_byte;

  delay(10);
  Serial.println("############################################################");

  // Set up the WiFi Access Point if it is configured
#ifdef WIFI_ENABLE_AP
  Serial.println();
  Serial.println("Configuring access point...");

  WiFi.AP.begin();
  WiFi.AP.config(ap_ip, ap_ip, ap_mask, ap_leaseStart, ap_dns);
  WiFi.AP.create(ssid_ap, password_ap);
  if (!WiFi.AP.waitStatusBits(ESP_NETIF_STARTED_BIT, 1000)) {
    Serial.println("Failed to start AP!");
    return;
  }
  Serial.print("AP IP address: ");
  Serial.println(ap_ip);
#endif

  // Connect to a WiFi network if configured
#ifdef WIFI_ENABLE_CLIENT
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  // Start the Webserver
  Serial.println();
  server.begin();
  Serial.println("Server started");
  Serial.println();
  Serial.flush();
  Serial.readString(); // Clear read buffer
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    String requested_page = "";
    uint8_t message_type = MSG_ERROR; // Enables error banner on response page
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            if (requested_page.equals("state")) {
              // Send JSON response when state page is requested
              send_json_status(client);
            }
            else {
              // Send HTML response for all other requests
              send_html_response(client, requested_page, message_type);
            }
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            // Only process the get line from the request
            if (currentLine.startsWith("GET /")) {
              // Ignore initial part of the get request and remove everything after the first space in the string
              requested_page = currentLine.substring(5);
              int space_idx = requested_page.indexOf(' ');
              if (space_idx >= 0) {
                requested_page = requested_page.substring(0,space_idx);
              }
              // Convert all requests to lower page
              requested_page.toLowerCase();
              Serial.print("    Requested Page: '");
              Serial.print(requested_page);
              Serial.println("'");

              if(requested_page.equals("state") || requested_page.equals(""))
              {
                // Disable error banner on state page and main page
                message_type = MSG_OFF;
              }
              else if(requested_page.equals("e"))
              {
                // Set banner to success banner and enable the sleep mode
                message_type = MSG_SLEEP_UPDATE;
                if (!sleep_active)
                {
                  sleep_active = true;
                  EEPROM.write(SLEEP_STATE_ADDR, sleep_active);
                  EEPROM.commit();
                }
                analogWrite(LED_RD, 0);
                analogWrite(LED_YE, 0);
                for(uint8_t ctr = 0; ctr < 8; ctr++) {
                    analogWrite(LED_GN, ((ctr + 1)%2)*255);
                    delay(125);
                }
              }
              else if(requested_page.equals("d"))
              {
                // Set banner to success banner and disable the sleep mode
                message_type = MSG_SLEEP_UPDATE;
                if (sleep_active)
                {
                  sleep_active = false;
                  EEPROM.write(SLEEP_STATE_ADDR, sleep_active);
                  EEPROM.commit();
                }
                analogWrite(LED_YE, 0);
                analogWrite(LED_GN, 0);
                for(uint8_t ctr = 0; ctr < 8; ctr++) {
                    analogWrite(LED_RD, ((ctr + 1)%2)*255);
                    delay(125);
                }
              }
              else if(requested_page.length() == 1 && CHAR_STATE_MAP.contains(requested_page.charAt(0)))
              {
                // Check if requested subpage can be mapped to a valid state and switch to that state
                set_state(CHAR_STATE_MAP.at(requested_page.charAt(0)));
                message_type = MSG_STATE_UPDATE;
              }
              else
              {
                // Show error message about unknown state
                message_type = MSG_UNKNOWN_CMD;
              }
            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  if (Serial.available() > 0) {
    // read a byte from the serial interface
    incoming_byte = Serial.read();
    Serial.print("Received Serial msg:");
    Serial.println(char(incoming_byte));
    // convert to lower case
    if (incoming_byte >= 'A' && incoming_byte <= 'Z')
        incoming_byte += 'a' - 'A';

    // Check none state commands
    if(incoming_byte == 'd'){
      sleep_active = false;
      EEPROM.write(SLEEP_STATE_ADDR, sleep_active);
      analogWrite(LED_YE, 0);
      analogWrite(LED_GN, 0);
      for(uint8_t ctr = 0; ctr < 8; ctr++) {
          analogWrite(LED_RD, ((ctr + 1)%2)*255);
          delay(125);
      }
    }
    else if(incoming_byte == 'e') {
      sleep_active = true;
      EEPROM.write(SLEEP_STATE_ADDR, sleep_active);
      analogWrite(LED_RD, 0);
      analogWrite(LED_YE, 0);
      for(uint8_t ctr = 0; ctr < 8; ctr++) {
          analogWrite(LED_GN, ((ctr + 1)%2)*255);
          delay(125);
      }
    }
    // Check for state changes
    else if(CHAR_STATE_MAP.contains(incoming_byte))
    {
      set_state(CHAR_STATE_MAP.at(incoming_byte));
    }
  }

  run_state_machine();
}
