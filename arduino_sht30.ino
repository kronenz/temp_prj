#include <ESP8266HTTPClient.h>

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <WEMOS_SHT3X.h>
// Replace with your network details
const char* ssid = "tpsystem";
const char* password = "forwiz8020";
const char* deviceid = "tp01";

// 웹 서버 설정
const char *serverAddress = "192.168.0.2"; // 웹 서버 주소
const int serverPort = 80; // HTTP 포트 (일반적으로 80)

// 시간 간격 설정 (10초)
unsigned long interval = 10000; // 10초
unsigned long previousMillis = 0;

// LED 핀 설정
const int ledPin = 14;

HTTPClient http;
WiFiClient client;
// LED 깜박이는 함수
void blinkLed() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void blinkSuccess() { 
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
}
 
void blinkFailed(){
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    delay(1000);
}

SHT3X sht30(0x45);
void sendPostRequest(float h, float t, float f);
// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
 
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

// runs over and over again
void loop() {
  // Listenning for new clients
  float h = 0;
  float t = 0;
  float f = 0;
  int sht_get=1; 
  delay(1000);

 
  unsigned long currentMillis = millis();

  // 10초 간격으로 HTTP POST 요청 보내기
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int sht_get=1; 
    sht_get=sht30.get();
    if(sht_get==0){
      h = sht30.humidity;
      t = sht30.cTemp;
      f = sht30.fTemp;
      
      Serial.print("Temperature in Celsius : ");
      Serial.println(t);
      
      Serial.print("Temperature in Fahrenheit : ");
      Serial.println(f);
      Serial.print("Relative Humidity : ");
      Serial.println(h);
      Serial.println();
    }
    // HTTP POST 요청 보내기
    sendPostRequest(h,t,f);
  }
}

void sendPostRequest(float h, float t, float f) {

  // HTTP POST 요청 설정
  http.begin(client, "http://" + String(serverAddress) + "/api/temp/5sec"); // 엔드포인트 URL 설정
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  uint localip = client.localIP().v4();
  // POST 데이터 설정
  String postData = &device_ip="+ String(localip) + "&device_name=" + String(deviceid) + "humidity=" + String(h) + "&temperature_celsius=" + String(t) + "&temperature_fahrenheit=" + String(f);

  // HTTP POST 요청 보내기
  int httpResponseCode = http.POST(postData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
    blinkSuccess();
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
    blinkFailed();
  }

  http.end();

  
    digitalWrite(ledPin, LOW);
    delay(1000);
}