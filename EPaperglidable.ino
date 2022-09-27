
#include <M5EPD.h> //get specific libaries according to here: http://docs.m5stack.com/en/quick_start/m5paper/arduino
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //https://arduinojson.org/
#include <string>
#include "time.h"

#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PW "your-wifi-pw"
#define API_KEY "your-api-key" // get yours on https://paraglidable.com/, you currently have to set all ten locations as the code won't work with less (I guess, never tried)

M5EPD_Canvas canvas(&M5.EPD);
WiFiClient client;
HTTPClient http;

const char *ntpServer =
    "pool.ntp.org";                  // Set the connect NTP server.
const long gmtOffset_sec = 3600;     // UTM+1
const int daylightOffset_sec = 3600; //+1

const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n";
void setup()
{
  M5.begin(false, false, true, true, true); // touch, SD, Serial, ADC, I2C
  M5.EPD.SetRotation(90);
  M5.EPD.Clear(true);

  canvas.createCanvas(540, 960);
  canvas.setTextSize(3);

  canvas.drawString("Connecting to WiFi", 10, 10);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU);

  WiFi.begin(WIFI_SSID, WIFI_PW);
  int wifi_counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
    wifi_counter++;
    if (wifi_counter > 10)
      M5.shutdown();
  }
  canvas.drawString("Connected to WiFi  ", 10, 10);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Send request
  http.useHTTP10(true);
  String api_url = "https://paraglidable.com/apps/api/get.php?key=apikey&amp;format=JSON&amp;version=1";
  api_url.replace("apikey", API_KEY);
  http.begin(api_url, root_ca);
  int httpResponseCode = http.GET();
  Serial.print("Response: ");
  Serial.println(httpResponseCode);

  // Parse response
  DynamicJsonDocument doc(24576);
  DeserializationError error = deserializeJson(doc, http.getStream());

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    M5.shutdown();
    return;
  }
  JsonObject documentRoot = doc.as<JsonObject>();
  String date[10];
  int i = 0;
  for (JsonPair keyValue : documentRoot)
  {
    date[i] = keyValue.key().c_str();
    Serial.println(date[i]);
    i++;
  }
  canvas.fillCanvas(0x0000);
  { // Output current time.
    struct tm timeinfo;
    canvas.setTextSize(2);
    if (!getLocalTime(&timeinfo))
    { // Return 1 when the time is successfully
      // obtained.
      canvas.drawString("Failed to obtain time", 10, 30);
      return;
    }
    char timeStringBuff[50]; // 50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%d. %B %Y %H:%M:%S", &timeinfo);
    // print like "const char*"
    canvas.drawString(timeStringBuff, 170, 40);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU);
    canvas.setTextSize(3);
  }
  // Disconnect
  http.end();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  // Read values
  // String name = doc[date[0]][0]["name"];
  // Serial.println(name);
  /*String name[10]; //funktioniert in der Schleife nicht, deshalb einzeln :(
  for (int i = 0; i < 10; i++){
    name[i] = doc[date[0]][i]["name"];
    canvas.drawString(name[i], 10, 10 + 30*i);
  }*/

  String name0 = doc[date[0]][0]["name"];
  canvas.drawString(name0, 10, 40 + 90 * 0);
  String name1 = doc[date[0]][1]["name"];
  canvas.drawString(name1, 10, 40 + 90 * 1);
  String name2 = doc[date[0]][2]["name"];
  canvas.drawString(name2, 10, 40 + 90 * 2);
  String name3 = doc[date[0]][3]["name"];
  canvas.drawString(name3, 10, 40 + 90 * 3);
  String name4 = doc[date[0]][4]["name"];
  canvas.drawString(name4, 10, 40 + 90 * 4);
  String name5 = doc[date[0]][5]["name"];
  canvas.drawString(name5, 10, 40 + 90 * 5);
  String name6 = doc[date[0]][6]["name"];
  canvas.drawString(name6, 10, 40 + 90 * 6);
  String name7 = doc[date[0]][7]["name"];
  canvas.drawString(name7, 10, 40 + 90 * 7);
  String name8 = doc[date[0]][8]["name"];
  canvas.drawString(name8, 10, 40 + 90 * 8);
  String name9 = doc[date[0]][9]["name"];
  canvas.drawString(name9, 10, 40 + 90 * 9);
  // name[0] = doc[date[0]][0]["name"].as<String>;
  // canvas.drawString(name[0], 10, 10 + 30*0);

  for (i = 0; i < 10; i++)
  {
    canvas.drawString("fly:", 10, 70 + 90 * i);
    canvas.drawString("XC:", 10, 100 + 90 * i);
  }

  String dateR[10];
  for (i = 0; i < 4; i++)
  {
    dateR[i] = date[i];
    dateR[i].replace("2022-", "");
    dateR[i].replace("2023-", "");
    dateR[i].replace("2024-", ""); //I know this a dirty way of removing the year from the dates, will be fine for the next 2+ years
    canvas.drawString(dateR[i], 100 + 100 * i, 10);
    for (int j = 0; j < 10; j++)
    {
      float f = doc[date[i]][j]["forecast"]["fly"];
      int fl = (int)(f * 100);
      String fly = String(fl) + "%";
      float x = doc[date[i]][j]["forecast"]["XC"];
      int xc = (int)(x * 100);
      String XC = String(xc) + "%";
      canvas.drawString(fly, 100 + 100 * i, 70 + 90 * j);
      canvas.drawString(XC, 100 + 100 * i, 100 + 90 * j);
    }
  }

  canvas.setTextSize(2);
  canvas.drawString(String(M5.getBatteryVoltage()), 10, 10);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU);
  M5.update();
  Serial.println("ENDE");
  delay(1 * 1000);
  if (M5.BtnL.isPressed()) //if the Slider is held upwards while the refresh happens the next boot isn't planned 
  //(however if one is already planned it won't be overwritten)
  {
    Serial.println("shuting down permanently");
    M5.shutdown();
  }
  else
  {
    Serial.println("shutting down... see you in 4h");
    M5.shutdown(4 * 3600);
  }
  M5.update();
  Serial.println("Hallo noch wer da?");
}

void loop()
{
  // put your main code here, to run repeatedly:
  //this only runs on USB-Power as the shutdown command doesn't work when plugged in
}
