#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define CLK_PIN   18 
#define DATA_PIN  23 
#define CS_PIN    5 

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* ssid     = "yourssid";
const char* password = "yourpw";

String Time, hour, minute;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize the display
  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();

  // Initialize the NTPClient
  timeClient.begin();
  timeClient.setTimeOffset(3600);
}

void loop() {
  obtainTime();
}

void obtainTime() {
  // Update the time from the NTP server
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  currentMillis = millis();

  if (currentMillis - previousMillis > interval)  {
    previousMillis = millis();

    // Get the time as a formatted string
    Time = timeClient.getFormattedTime();
    Serial.println(Time);

    // Extract the hour and minute from the time string
    hour = Time.substring(0, 2);
    minute = Time.substring(3, 5);

    // Build the time string to display
    Time = hour + ":" + minute;
    Serial.println(Time);

    // Display the time string on the dot matrix display
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(Time);
  }
}