// Library yang dibutuhkan
#include "WiFi.h"
#include <HTTPClient.h>


// Wifi 
const char* ssid = "WiFi SSID";  
const char* password = "password123"; 

// URL SpreadSheets
String Web_App_URL = "...";

// Variable Sensor dan suhu
String Status_Read_Sensor = "";
float Temp = 0;

// Delay
unsigned long waktuSaatIni = millis();
unsigned long waktuAkhir = 0;
boolean start = true;

void setup() {
  // Menjalankan Serial Monitor
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  // Menghubungkan Wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("------------");
  Serial.println("WiFi connected");
  Serial.println("------------");

  delay(100);
  
}
void loop() {
  // Membaca sensor
  Status_Read_Sensor = "Success";
  waktuSaatIni = millis();
  
  // Mengirim data ke Google SpreadSheets akan berjalan hanya ketika WiFi Terhubung
  if ( start == true || waktuSaatIni - waktuAkhir >= 5000 && WiFi.status() == WL_CONNECTED) {
    waktuSaatIni  = millis();
    waktuAkhir    = waktuSaatIni;
    start = false;

    // Buat String URL untuk mengirim atau menulis data ke Google Sheets
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&srs=" + Status_Read_Sensor;
    Send_Data_URL += "&temp=" + String(Temp);

    // Proses Pengiriman Data ke Google SpreadSheets menggunakan URL di atas
    Serial.println();
    Serial.println("-------------");
    Serial.println("Mengirim data ke Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

    //::::::::::::::::::Proses Pengiriman Data ke Google SpreadSheets menggunakan URL di atas
      // 1. Menginisialisasi HTTPClient sebagai http
      HTTPClient http;
  
      // menjalankan HTTP GET request
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  
      // Mendapatkan status code dari HTTP 
      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);
  
      // Mendapatkan respon dari SpreadSheets setelah mengirim data 
      String payload;
      if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Informasi : " + payload);    
      }
      
      http.end();
    //::::::::::::::::::
    Serial.println("-------------");
    Temp+=10;
  }else if (WiFi.status() != WL_CONNECTED){
    ESP.restart();
  }
}
