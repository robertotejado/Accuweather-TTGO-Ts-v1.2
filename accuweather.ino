#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>                               // Hardware-specific library
#include <MPU9250_asukiaaa.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
 

const char* ssid = "";
const char* password =  "";
 

const String endpoint = "http://dataservice.accuweather.com/currentconditions/v1/xxxxxx.json?apikey=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&language=es";
//const String key = "yourAPIkey";

const char* weathertext= " ";
const char* tiempo =0;
int temp =0;
bool hasprecipitation= 0;
long int epochtime =0;
const char* unidad_metrica_temp=" ";
bool isDayTime = 0;



#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     9
Adafruit_ST7735 tft = Adafruit_ST7735(16, 17, 23, 5, 9); // CS,A0,SDA,SCK,RESET  fÃ¼r ttgo
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);    // fÃ¼r UNO und 1.8 Display
float r, x1, ya, z1, x2, y2, z2, x3, y3, x, y, z3;               //

#ifdef _ESP32_HAL_I2C_H_

#define SDA_PIN 19

#define SCL_PIN 18

#endif

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define zmax 2        
#define komma 1        
#define diffmax 1.0 


 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");


////////////////////////////////////////////////////////////////////////

  

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    //http.begin(endpoint + key); //Specify the URL
     http.begin(endpoint); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode); //200 OK! http code
        Serial.println(payload); //payload = contenido

  //StaticJsonBuffer<5000> jsonBuffer;
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10) + 490;
  DynamicJsonBuffer jsonBuffer(capacity);

  //JsonObject& root = jsonBuffer.parseObject(payload);

JsonArray& root = jsonBuffer.parseArray(payload);

// Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseArray() failed");
    return;
  }
  
JsonObject& root_0 = root[0];
  
const char* root_0_LocalObservationDateTime = root_0["LocalObservationDateTime"]; // "2019-07-30T23:31:00+02:00"
long root_0_EpochTime = root_0["EpochTime"]; // 1564522260
const char* root_0_WeatherText = root_0["WeatherText"]; // "Mayormente despejado"
int root_0_WeatherIcon = root_0["WeatherIcon"]; // 34
bool root_0_HasPrecipitation = root_0["HasPrecipitation"]; // false
bool root_0_IsDayTime = root_0["IsDayTime"]; // false

JsonObject& root_0_Temperature_Metric = root_0["Temperature"]["Metric"];
float root_0_Temperature_Metric_Value = root_0_Temperature_Metric["Value"]; // 12.8
const char* root_0_Temperature_Metric_Unit = root_0_Temperature_Metric["Unit"]; // "C"
int root_0_Temperature_Metric_UnitType = root_0_Temperature_Metric["UnitType"]; // 17

JsonObject& root_0_Temperature_Imperial = root_0["Temperature"]["Imperial"];
int root_0_Temperature_Imperial_Value = root_0_Temperature_Imperial["Value"]; // 55
const char* root_0_Temperature_Imperial_Unit = root_0_Temperature_Imperial["Unit"]; // "F"
int root_0_Temperature_Imperial_UnitType = root_0_Temperature_Imperial["UnitType"]; // 18

const char* root_0_MobileLink = root_0["MobileLink"]; // "http://m.accuweather.com/es/es/vitoria-gasteiz/309383/current-weather/309383"
const char* root_0_Link = root_0["Link"]; // "http://www.accuweather.com/es/es/vitoria-gasteiz/309383/current-weather/309383"


  


  // Extract values

  Serial.println(root_0_LocalObservationDateTime);
  Serial.println(root_0_EpochTime);
  Serial.println(root_0_WeatherText);
  Serial.println(root_0_HasPrecipitation);
  Serial.println(root_0_Temperature_Metric_Value);
  Serial.println(root_0_Temperature_Metric_Unit);


  weathertext= root_0_WeatherText;
  tiempo = root_0_LocalObservationDateTime;
  epochtime =  root_0_EpochTime;
  temp = root_0_Temperature_Metric_Value;
  unidad_metrica_temp = root_0_Temperature_Metric_Unit;
  hasprecipitation= root_0_HasPrecipitation;
  isDayTime  = root_0_IsDayTime;
 
  
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  //delay(2000);

  ///////////////////////////////////////////////////////////




  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setTextWrap(true);
  tft.setRotation(4);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_GREEN);
  //tft.setTextSize(2);
   tft.setTextSize(1); //  The space occupied by a character when the text size is ‘1’ using the standard font is six pixels wide by eight pixels high.


#ifdef _ESP32_HAL_I2C_H_
  // for esp32
  Wire.begin(19, 18); //sda, scl
#else
  Wire.begin();
#endif
 


   delay(5000); //cada 5 segundos
}
 
void loop() {

String hora="";
String minuto="";
String segundo="";
String titulo= "Accuweather ";
//String frase ="Es la hora   de las   Tortas!";
tft.fillScreen(ST7735_BLACK);
tft.setCursor(20, 20);
tft.setTextColor(ST7735_WHITE);
tft.println(titulo+""); 
//tft.println("Fecha:"); 
//tft.println(tiempo); 
//conversion epochtime a string
char bufferdate[32];
//sprintf(buffertime, "%02d.%02d.%02d %02d:%02d:%02d", day(epochtime), month(epochtime), year(epochtime), hour(epochtime), minute(epochtime), second(epochtime));
sprintf(bufferdate, "%02d.%02d.%02d", day(epochtime), month(epochtime), year(epochtime)); 
tft.print(" "); 
tft.println(bufferdate);
//Tiempo en H:M:S
tft.setTextColor(ST7735_GREEN);
char buffertime[32];
sprintf(buffertime, "%02d:%02d:%02d", hour(epochtime), minute(epochtime), second(epochtime)); 
tft.print(" "); 
tft.println(buffertime);
tft.setTextColor(ST7735_YELLOW);
//tft.println("Descripcion:");
tft.print(" ");  
tft.println(weathertext); 
tft.setTextColor(ST7735_MAGENTA);
tft.print(" Temp: "); 
tft.print(temp ); tft.print(" "); 
tft.println(unidad_metrica_temp); 
//tft.println("Lluvia:"); 

if (hasprecipitation){
tft.println(" Llueve"); 
}else
{
tft.println(" No llueve"); 
  }
  
if(isDayTime){
tft.println(" De dia"); 
  }else{
tft.println(" De noche");  
    }

  
//delay(1000);


 
  Serial.println(F("Response Loop:"));
  Serial.println("Fecha:"); 
  Serial.println(buffertime);
  Serial.println("Descripcion:"); 
  Serial.println(weathertext);
  Serial.println("Temperatura:");
  Serial.println(temp);
  Serial.println("Lluvia:");
  Serial.println(hasprecipitation);
delay(1000*60*60); //retraso 1 hora para evitar requests has been exceeded de la API de Accuweather

 
}
