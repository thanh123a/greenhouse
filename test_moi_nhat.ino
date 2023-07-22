/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6fu71hLf-"
#define BLYNK_TEMPLATE_NAME "Group1"
#define BLYNK_AUTH_TOKEN "WkQUDI63GJ4gnnr0FsURYnz5cMTL2e34"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHTesp.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

BlynkTimer timer;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "V2027";
char pass[] = "1234567890";

DHTesp dht;
float temp, humi;
int number = 0;
int valueSoilMoi, percent;
bool autoMode = false;
boolean online = true;
int temp_input, soil_input;
BLYNK_WRITE(V0) {       //auto mode
  int buttonValue = param.asInt();
  if (buttonValue == 1) {
    // Chế độ tu dong
    autoMode = true;
  } else {
    // Chế độ thu cong
    autoMode = false;
  }
}
BLYNK_WRITE(V7) {     //Tuoi
  if (!autoMode) {
    int toggleState_0 = param.asInt();
    if (toggleState_0 == 0) {
      digitalWrite(D1, LOW);
    } else {
      digitalWrite(D1 , HIGH);
    }
  }
}
BLYNK_WRITE(V1) {     //Quat
  if (!autoMode) {
    int toggleState_1 = param.asInt();
    if (toggleState_1 == 0) {
      digitalWrite(D2, LOW);
    } else {
      digitalWrite(D2 , HIGH);
    }
  }
}
BLYNK_WRITE(V2) {    // LED
  if (!autoMode) {
    int toggleState_2 = param.asInt();
    if (toggleState_2 == 0) {
      digitalWrite(D7, LOW);
    } else {
      digitalWrite(D7 , HIGH);
    }
  }
}
BLYNK_WRITE(V4) {    //Phun suong
  if (!autoMode) {
    int toggleState_3 = param.asInt();
    if (toggleState_3 == 0) {
      digitalWrite(D0, LOW);
    } else {
      digitalWrite(D0 , HIGH);
    }
  }
}
int startHour = 18;
int startMinute = 0;
int stopHour = 21;
int stopMinute = 0;
BLYNK_WRITE(V10) { // Callback khi người dùng thay đổi giá trị thời gian trên ứng dụng Blynk
  TimeInputParam t(param);

  if (t.hasStartTime()) {
    startHour = t.getStartHour();
    startMinute = t.getStartMinute();
    Serial.print("Start time: ");
    Serial.print(startHour);
    Serial.print(":");
    Serial.println(startMinute);
  }

  if (t.hasStopTime()) {
    stopHour = t.getStopHour();
    stopMinute = t.getStopMinute();
    Serial.print("Stop time: ");
    Serial.print(stopHour);
    Serial.print(":");
    Serial.println(stopMinute);
  }
}

void checkLightSchedule() {
  // Lấy giờ hiện tại từ thời gian máy chủ NTP
  int currentHour = timeClient.getHours() + 7;
  int currentMinute = timeClient.getMinutes();
  // Kiểm tra xem có nằm trong khoảng thời gian bật đèn hay không
  if ((currentHour >= startHour && currentMinute >= startMinute) && (currentHour <= stopHour && currentMinute <= stopMinute)) {    
      if (digitalRead(D7) == HIGH) {
        digitalWrite(D7, LOW);
      }
      Serial.println("Light is ON");
  } else {
    if (digitalRead(D7) == LOW) {
        digitalWrite(D7, HIGH);
      }
    Serial.println("Light is OFF");
  }
}
void syncDataSet() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V8);
}

void setup()
{
  // Setup relay mode
  pinMode(D1, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D7, OUTPUT);
  // Set up dht11
  dht.setup(D6, DHTesp::DHT11);
  // Set up manual mode at start
  autoMode = false;
  // Turn off all relay
  digitalWrite(D1, LOW);
  digitalWrite(D0, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D7, LOW);
  // Debug console
  Serial.begin(115200);
  // Connect controller to Wifi and Blynk server
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  timer.setInterval(3000L, syncDataSet);
  timer.setInterval(5000, getTempAndHumi);
  timer.setInterval(5000, getSoilMois);
  timer.setInterval(3000, uploadDataToBlynk);
  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void getTempAndHumi() {
  temp = dht.getTemperature();
  humi = dht.getHumidity();
  Serial.println(temp);
  Serial.println(humi);
}
void getSoilMois() {
  int valueSoilMoi = analogRead(A0);
  percent = 100 - map(valueSoilMoi, 0, 1023, 0, 100);
  Serial.println(percent);
}

BLYNK_WRITE(V9)
{
  int pinValue = param.asInt();
  Serial.print("V3 Slider value is: ");
  Serial.println(pinValue);
  temp_input = pinValue;
}
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  Serial.print("V9 Slider value is: ");
  Serial.println(pinValue);
  soil_input = pinValue;
}
void autoModeControllByTemp(float x, int y, float z, int w) {
  if (x > z) {

    if (digitalRead(D0) == LOW) {
      digitalWrite(D0, HIGH);
    }
    if (digitalRead(D2) == LOW) {
      digitalWrite(D2, HIGH);
    }
  } else {
    if (digitalRead(D0) == HIGH) {
      digitalWrite(D0, LOW);
    }
    if (digitalRead(D2) == HIGH) {
      digitalWrite(D2, LOW);
    }
  }

  if (y >= w) {
    if (digitalRead(D1) == LOW) {
      digitalWrite(D1, HIGH);
    }
  } else {
    if (y != 0) {
      if (digitalRead(D1) == HIGH) {
        digitalWrite(D1, LOW);
      }
    }
  }
  checkLightSchedule();
}
void checkOnOrOff() {
  if (WiFi.status() != WL_CONNECTED || !Blynk.connected()) {
    Serial.println("Lost connection. Switching to offline mode.");
    online = false;
  } else {
    online = true;
  }
}
void uploadDataToBlynk() {
  Blynk.virtualWrite(V5, temp);
  Blynk.virtualWrite(V6, humi);
  Blynk.virtualWrite(V8, percent);
}
void loop() {
  Blynk.run();
  timer.run();
  // up data to Blynk
  //uploadDataToBlynk();

  // check wifi status
  checkOnOrOff();

  // Set mode auto for device
  if (!online) {
    autoModeControllByTemp(temp, percent, temp_input, soil_input);
  }
  else if (online && autoMode) {
    autoModeControllByTemp(temp, percent, temp_input, soil_input);
  }
  timeClient.update(); // Cập nhật thời gian từ máy chủ NTP
}
