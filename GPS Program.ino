#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>             // Software Serial Library so we can use other Pins for communication with the GPS module
#include <TinyGPS++.h>                  // Tiny GPS Plus Library
#include <Adafruit_ssd1306syp.h>        // Adafruit oled library for display

#define FIREBASE_HOST "test-c249c-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "jWdlmUlYcCiE99Zx30496eubeJjK58jp7z7V2Ss6"
#define WIFI_SSID "Redmi Note 11 Pro 5G"
#define WIFI_PASSWORD "ggwp1234"

Adafruit_ssd1306syp display(4,5);       // OLED display (SDA to Pin 4), (SCL to Pin 5)

FirebaseData firebaseData;              //Define FirebaseESP8266 data object

FirebaseJson json;

const int RXPin = 12, TXPin = 13;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{

  Serial.begin(9600);
  display.initialize();                                 // Initialize OLED display  
  display.clear();                                      // Clear OLED display
  display.setTextSize(1);                               // Set OLED text size to small
  display.setTextColor(WHITE);                          // Set OLED color to White
  display.setCursor(0,0);                               // Set cursor to 0,0
  display.println("Nodemcu GPS Tracker");  
  display.print("Version: ");
  display.println(TinyGPSPlus::libraryVersion());
  //display.println("Connecting to WiFi...");
  display.update();
  neo6m.begin(9600);
  
  wifiConnect();

  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase OK.");

}

void loop() {
  
  smartdelay_gps(1000);

  if(gps.location.isValid()) 
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    display.clear();
    display.setCursor(0,0); 
    display.print("Latitude  : ");
    display.println(gps.location.lat(), 5);
    display.print("Longitude : ");
    display.println(gps.location.lng(), 5);
    display.update();
    
    //Send to Serial Monitor for Debugging
    //Serial.print("LAT:  ");
    //Serial.println(latitude);  // float to x decimal places
    //Serial.print("LONG: ");
    //Serial.println(longitude);
    
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude))
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude))
      {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
  }
  else
  {
    Serial.println("No valid GPS data found.");
    display.println("No valid GPS data found.");
    display.update();
  }
  
  delay(1000);
}

static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  display.println("Connected with IP: ");
  display.println(WiFi.localIP());
  display.update();
}

void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}

void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}

void firebaseReconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
