/*
  VFD.cpp, vacuum fluroescent example for Samsung HCS-12SS59T, V0.9 171112 qrt@qland.de
  Thanks to https://github.com/qrti/VFD-HCS-12SS59T and
  https://notsyncing.net/?p=blog&b=2018.vfdboard and
  to Luca and Marvin and Dennis!
  todo: Reset-Problem, Display geht aus nach beliebiger Zeit
  todo: GPIO16! ist jetzt VFD_RESET
  todo: der Analog-Port kann für eine LDR-Schaltung zur Anpassung
        der Helligkeit verwendet werden
*/
#include <Arduino.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "VFD.h"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "de.pool.ntp.org");

int hour = 17;
int minute = 15;
int second = 0;
int date = 7;
int month = 9;
int year = 2023;
int day = 4;
// Week Days
String weekDays[7] = {"Sonntag", "Montag", "Dienstag", 
"Mittwoch", "Donnerstag", "Freitag", "Samstag"};

// Month names
String months[12] = {"Januar", "Februar", "Maerz", 
"April", "Mai", "Juni", "Juli", "August", 
"September", "Oktober", "November", "Dezember"};
int brigthness = 4;
int i = 0;
int i_of_SHOW_IP_ADDR;
const int nighttimeLight = 2;
const int daytimeLight = 12;
unsigned long prevTime = millis();
unsigned long actTime = 0;
unsigned long tch = 0;                   // Zeit zu der zuletzt umgestellt wurde
unsigned long updateInterval = 660000UL; // NTP Aktualisierungsintervall in Millis
char txLine[80];
bool lineComplete = false;
// Set client an AP name (Mac adress will be added)
String WiFiName = "VFD_UHR_1";
// Replace with your network credentials
// const char *ssid = "RT-Labor-1";
// const char *passwd = "hardies42";
const char *ssid = "hw1_gast";
const char *passwd = "KeineAhnung";
// const char *ssid = "FRITZ!Box 7490";
// const char *passwd = "95642445727877808620";

String localIP;
char buffer[20];

void SerialEvent(void)
{
  if (Serial.available())
  {
    char c = Serial.read();
    Serial.print(c);
    if (c != '\n' && i < 79)
    {
      lineComplete = false;
      txLine[i++] = c;
    }
    else
    {
      txLine[i] = '\0';
      lineComplete = true;
    }
  }
}

void setup()
{
  // String nameforwifi = WiFiName + WiFi.macAddress();
  // const char *wifi_name = nameforwifi.c_str();

  Serial.begin(115200);
  Vfd.init();
  delay(1500);
  pinMode(WAKE_UP, OUTPUT);
  digitalWrite(WAKE_UP, HIGH);

  Serial.println("\n\nVFD initialized!");

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, passwd);
  Serial.println("");
  sprintf(buffer, "SEARCH  WIFI");
  Vfd.write(buffer);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  localIP = (WiFi.localIP().toString());
  sprintf(buffer, "%s", localIP.c_str());
  Vfd.write(buffer);
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600 + 1 * 3600);
  timeClient.setUpdateInterval(updateInterval);

  delay(2500);
}

void loop()
{
  timeClient.update();
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  second = timeClient.getSeconds();
  year = timeClient.getYear();
  month = timeClient.getMonth(); // januar = 1
  date = timeClient.getDate();   // day of the month
  day = timeClient.getDay();     // day of the week

  int screen = minute % 4;
 
  SerialEvent();
  if (lineComplete)
  {
    Serial.println("Input: " + String(txLine));
    lineComplete = false;
    sprintf(buffer, "%s", txLine);
    Vfd.write(buffer);
    Serial.printf("%02d-%02d-%02d \r", hour, minute, second);
    txLine[0] = '\0';
  }

  if (millis() - actTime > 999)
  {
    actTime = millis();

    if (hour > 6 && hour < 22){
      brigthness = daytimeLight;
    }
    else {
      brigthness = nighttimeLight;
    }
    if (second >= 55)
    {
      switch (screen)
      {
      case 0:
        sprintf(buffer, " %02d-%02d-%04d \r", date, month, year);
        Vfd.brightness(brigthness);
        Vfd.write(buffer);
        Serial.print(buffer);
        break;
      case 1:
        sprintf(buffer, " %s   ", weekDays[day].substring(0, 10).c_str());
        Vfd.brightness(brigthness);
        Vfd.write(buffer);
        Serial.print(buffer);
        break;
      case 2:
        sprintf(buffer, " %s   ", months[month - 1].substring(0, 10).c_str());
        Vfd.brightness(brigthness);
        Vfd.write(buffer);
        Serial.print(buffer);
        break;
      case 3:
        sprintf(buffer, "%s\r", "  VFD-clock ");
        Vfd.brightness(brigthness);
        Vfd.write(buffer);
        Serial.print(buffer);
        break;
      default:
        break;
      }
    }
    else
    {
      sprintf(buffer, "  %02d %02d %02d  \r", hour, minute, second);
      Vfd.brightness(brigthness);
      Vfd.write(buffer);
      Serial.print(buffer);
    }
  }
}
