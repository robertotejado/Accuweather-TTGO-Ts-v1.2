#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>                               // Hardware-specific library
//#include <MPU9250_asukiaaa.h>// Acelerometro y magnetometro , sensores
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <string>
#include <iostream>


const static String daysOfWeek[] = {
    "Domingo",
    "Lunes",
    "Martes",
    "Miercoles",
    "Jueves",
    "Viernes",
    "Sabado"
    
};



const char* ssid = "";
const char* password =  "";
 
const String endpoint = "http://dataservice.accuweather.com/forecasts/v1/daily/5day/XXXXXXXX.json?apikey=XXXXXXXXXXXXXXXXXXXX&language=es&metric=true"; // pronostico a 5 dias



const char* Headline_EffectiveDate = ""; // "2019-08-04T08:00:00+02:00"
long Headline_EffectiveEpochDate = 0; // 1564898400
const char* Headline_Text = ""; // "Agradable el domingo"

const char* DailyForecasts_0_Date = "0000-00-00T00:00:00+00:00";   // "2019-08-02T07:00:00+02:00"
long DailyForecasts_0_EpochDate = 0000000000;  // 1564722000
float DailyForecasts_0_Temperature_Minimum_Value = 0;   // 12.2
const char* DailyForecasts_0_Temperature_Minimum_Unit = ""; // "C"
int DailyForecasts_0_Temperature_Maximum_Value = 0; // 25
const char* DailyForecasts_0_Temperature_Maximum_Unit = ""; // "C"
const char* DailyForecasts_0_Day_IconPhrase = ""; // "Mayormente soleado"
bool DailyForecasts_0_Day_HasPrecipitation = false; // false
const char* DailyForecasts_0_Night_IconPhrase = ""; // "Mayormente despejado"
bool DailyForecasts_0_Night_HasPrecipitation = false; // false


const char* DailyForecasts_1_Date = ""; // "2019-08-03T07:00:00+02:00"
long DailyForecasts_1_EpochDate = 0000000000; // 1564808400
float DailyForecasts_1_Temperature_Minimum_Value = 0; // 15.2
const char* DailyForecasts_1_Temperature_Minimum_Unit = "C"; // "C"
int DailyForecasts_1_Temperature_Maximum_Value = 0; // 31
const char* DailyForecasts_1_Temperature_Maximum_Unit = ""; // "C"
const char* DailyForecasts_1_Day_IconPhrase = ""; // "Parcialmente soleado"
bool DailyForecasts_1_Day_HasPrecipitation = false; // false
const char* DailyForecasts_1_Night_IconPhrase = ""; // "Parcialmente nublado"
bool DailyForecasts_1_Night_HasPrecipitation = false; // false

const char* DailyForecasts_2_Date = ""; // "2019-08-04T07:00:00+02:00"
long DailyForecasts_2_EpochDate = 0000000000; // 1564894800
float DailyForecasts_2_Temperature_Minimum_Value = 0; // 13.9
const char* DailyForecasts_2_Temperature_Minimum_Unit = ""; // "C"
float DailyForecasts_2_Temperature_Maximum_Value = 0; // 24.6
const char* DailyForecasts_2_Temperature_Maximum_Unit = ""; // "C"
const char* DailyForecasts_2_Day_IconPhrase = ""; // "Nubes y claros"
bool DailyForecasts_2_Day_HasPrecipitation = false; // false
const char* DailyForecasts_2_Night_IconPhrase = ""; // "Nubes y claros"
bool DailyForecasts_2_Night_HasPrecipitation = false; // false

const char* DailyForecasts_3_Date = ""; // "2019-08-05T07:00:00+02:00"
long DailyForecasts_3_EpochDate = 0000000000; // 1564981200
int DailyForecasts_3_Temperature_Minimum_Value = 0; // 13
const char* DailyForecasts_3_Temperature_Minimum_Unit = ""; // "C"
float DailyForecasts_3_Temperature_Maximum_Value = 0; // 25.5
const char* DailyForecasts_3_Temperature_Maximum_Unit = "C"; // "C"
const char* DailyForecasts_3_Day_IconPhrase = ""; // "Parcialmente soleado"
bool DailyForecasts_3_Day_HasPrecipitation = false; // false
const char* DailyForecasts_3_Night_IconPhrase = ""; // "Nubes y claros"
bool DailyForecasts_3_Night_HasPrecipitation = false; // false

#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     9
Adafruit_ST7735 tft = Adafruit_ST7735(16, 17, 23, 5, 9); // CS,A0,SDA,SCK,RESET  fÃ¼r ttgo
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);    // fÃ¼r UNO und 1.8 Display
//float r, x1, ya, z1, x2, y2, z2, x3, y3, x, y, z3;               //

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

//#define zmax 2        
//#define komma 1        
//#define diffmax 1.0 


 
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

 const size_t capacity = 5*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(5) + 6*JSON_OBJECT_SIZE(2) + 20*JSON_OBJECT_SIZE(3) + 5*JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(9) + 3200;
DynamicJsonBuffer jsonBuffer(capacity);

JsonObject& root = jsonBuffer.parseObject(payload);

JsonObject& Headline = root["Headline"];
 Headline_EffectiveDate = Headline["EffectiveDate"]; // "2019-08-04T08:00:00+02:00"
 Headline_EffectiveEpochDate = Headline["EffectiveEpochDate"]; // 1564898400
 Headline_Text = Headline["Text"]; // "Agradable el domingo"

JsonArray& DailyForecasts = root["DailyForecasts"];

JsonObject& DailyForecasts_0 = DailyForecasts[0];
 DailyForecasts_0_Date = DailyForecasts_0["Date"]; // "2019-08-02T07:00:00+02:00"
 DailyForecasts_0_EpochDate = DailyForecasts_0["EpochDate"]; // 1564722000
JsonObject& DailyForecasts_0_Temperature_Minimum = DailyForecasts_0["Temperature"]["Minimum"];
 DailyForecasts_0_Temperature_Minimum_Value = DailyForecasts_0_Temperature_Minimum["Value"]; // 12.2
 DailyForecasts_0_Temperature_Minimum_Unit = DailyForecasts_0_Temperature_Minimum["Unit"]; // "C"
JsonObject& DailyForecasts_0_Temperature_Maximum = DailyForecasts_0["Temperature"]["Maximum"];
 DailyForecasts_0_Temperature_Maximum_Value = DailyForecasts_0_Temperature_Maximum["Value"]; // 25
 DailyForecasts_0_Temperature_Maximum_Unit = DailyForecasts_0_Temperature_Maximum["Unit"]; // "C"
JsonObject& DailyForecasts_0_Day = DailyForecasts_0["Day"];
 DailyForecasts_0_Day_IconPhrase = DailyForecasts_0_Day["IconPhrase"]; // "Mayormente soleado"
 DailyForecasts_0_Day_HasPrecipitation = DailyForecasts_0_Day["HasPrecipitation"]; // false
JsonObject& DailyForecasts_0_Night = DailyForecasts_0["Night"];
 DailyForecasts_0_Night_IconPhrase = DailyForecasts_0_Night["IconPhrase"]; // "Mayormente despejado"
 DailyForecasts_0_Night_HasPrecipitation = DailyForecasts_0_Night["HasPrecipitation"]; // false

JsonObject& DailyForecasts_1 = DailyForecasts[1];
 DailyForecasts_1_Date = DailyForecasts_1["Date"]; // "2019-08-03T07:00:00+02:00"
 DailyForecasts_1_EpochDate = DailyForecasts_1["EpochDate"]; // 1564808400
JsonObject& DailyForecasts_1_Temperature_Minimum = DailyForecasts_1["Temperature"]["Minimum"];
 DailyForecasts_1_Temperature_Minimum_Value = DailyForecasts_1_Temperature_Minimum["Value"]; // 15.2
 DailyForecasts_1_Temperature_Minimum_Unit = DailyForecasts_1_Temperature_Minimum["Unit"]; // "C"
JsonObject& DailyForecasts_1_Temperature_Maximum = DailyForecasts_1["Temperature"]["Maximum"];
 DailyForecasts_1_Temperature_Maximum_Value = DailyForecasts_1_Temperature_Maximum["Value"]; // 31
 DailyForecasts_1_Temperature_Maximum_Unit = DailyForecasts_1_Temperature_Maximum["Unit"]; // "C"
JsonObject& DailyForecasts_1_Day = DailyForecasts_1["Day"];
 DailyForecasts_1_Day_IconPhrase = DailyForecasts_1_Day["IconPhrase"]; // "Parcialmente soleado"
 DailyForecasts_1_Day_HasPrecipitation = DailyForecasts_1_Day["HasPrecipitation"]; // false
JsonObject& DailyForecasts_1_Night = DailyForecasts_1["Night"];
 DailyForecasts_1_Night_IconPhrase = DailyForecasts_1_Night["IconPhrase"]; // "Parcialmente nublado"
 DailyForecasts_1_Night_HasPrecipitation = DailyForecasts_1_Night["HasPrecipitation"]; // false


JsonObject& DailyForecasts_2 = DailyForecasts[2];
 DailyForecasts_2_Date = DailyForecasts_2["Date"]; // "2019-08-04T07:00:00+02:00"
 DailyForecasts_2_EpochDate = DailyForecasts_2["EpochDate"]; // 1564894800
JsonObject& DailyForecasts_2_Temperature_Minimum = DailyForecasts_2["Temperature"]["Minimum"];
 DailyForecasts_2_Temperature_Minimum_Value = DailyForecasts_2_Temperature_Minimum["Value"]; // 13.9
 DailyForecasts_2_Temperature_Minimum_Unit = DailyForecasts_2_Temperature_Minimum["Unit"]; // "C"
JsonObject& DailyForecasts_2_Temperature_Maximum = DailyForecasts_2["Temperature"]["Maximum"];
 DailyForecasts_2_Temperature_Maximum_Value = DailyForecasts_2_Temperature_Maximum["Value"]; // 24.6
 DailyForecasts_2_Temperature_Maximum_Unit = DailyForecasts_2_Temperature_Maximum["Unit"]; // "C"
JsonObject& DailyForecasts_2_Day = DailyForecasts_2["Day"];
 DailyForecasts_2_Day_IconPhrase = DailyForecasts_2_Day["IconPhrase"]; // "Nubes y claros"
 DailyForecasts_2_Day_HasPrecipitation = DailyForecasts_2_Day["HasPrecipitation"]; // false
JsonObject& DailyForecasts_2_Night = DailyForecasts_2["Night"];
 DailyForecasts_2_Night_IconPhrase = DailyForecasts_2_Night["IconPhrase"]; // "Nubes y claros"
 DailyForecasts_2_Night_HasPrecipitation = DailyForecasts_2_Night["HasPrecipitation"]; // false


JsonObject& DailyForecasts_3 = DailyForecasts[3];
 DailyForecasts_3_Date = DailyForecasts_3["Date"]; // "2019-08-05T07:00:00+02:00"
 DailyForecasts_3_EpochDate = DailyForecasts_3["EpochDate"]; // 1564981200
JsonObject& DailyForecasts_3_Temperature_Minimum = DailyForecasts_3["Temperature"]["Minimum"];
 DailyForecasts_3_Temperature_Minimum_Value = DailyForecasts_3_Temperature_Minimum["Value"]; // 13
 DailyForecasts_3_Temperature_Minimum_Unit = DailyForecasts_3_Temperature_Minimum["Unit"]; // "C"
JsonObject& DailyForecasts_3_Temperature_Maximum = DailyForecasts_3["Temperature"]["Maximum"];
 DailyForecasts_3_Temperature_Maximum_Value = DailyForecasts_3_Temperature_Maximum["Value"]; // 25.5
 DailyForecasts_3_Temperature_Maximum_Unit = DailyForecasts_3_Temperature_Maximum["Unit"]; // "C"
JsonObject& DailyForecasts_3_Day = DailyForecasts_3["Day"];
 DailyForecasts_3_Day_IconPhrase = DailyForecasts_3_Day["IconPhrase"]; // "Parcialmente soleado"
 DailyForecasts_3_Day_HasPrecipitation = DailyForecasts_3_Day["HasPrecipitation"]; // false
JsonObject& DailyForecasts_3_Night = DailyForecasts_3["Night"];
 DailyForecasts_3_Night_IconPhrase = DailyForecasts_3_Night["IconPhrase"]; // "Nubes y claros"
 DailyForecasts_3_Night_HasPrecipitation = DailyForecasts_3_Night["HasPrecipitation"]; // false

//////////////////////////////////////////////////////////

  // Print values last day

  Serial.println(DailyForecasts_3_Date);
  Serial.println(DailyForecasts_3_Temperature_Minimum_Value);
  Serial.println(DailyForecasts_3_Temperature_Maximum_Value);
  Serial.println(DailyForecasts_3_Temperature_Maximum_Unit);
  Serial.println(DailyForecasts_3_Day_IconPhrase);
  Serial.println(DailyForecasts_3_Day_HasPrecipitation);
  Serial.println(DailyForecasts_3_Night_IconPhrase);
  Serial.println(DailyForecasts_3_Night_HasPrecipitation);

 
  
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
   tft.setTextSize(0); //  The space occupied by a character when the text size is ‘1’ using the standard font is six pixels wide by eight pixels high.


/*//Sensores
#ifdef _ESP32_HAL_I2C_H_
  // for esp32
  Wire.begin(19, 18); //sda, scl
#else
  Wire.begin();
#endif
 */


   delay(5000); //cada 5 segundos
}
 
void loop() {



String hora="";
String minuto="";
String segundo="";
String titulo= "Accuweather ";
//String frase ="Es la hora   de las   Tortas!";
tft.fillScreen(ST7735_BLACK); //Rellena Pantalla en Negro
tft.setCursor(5, 5);
tft.setTextColor(ST7735_WHITE);
tft.println(titulo+""); 
tft.setTextColor(ST7735_GREEN);
//tft.print(Headline_EffectiveDate); 
//tft.print(Headline_EffectiveEpochDate); 
char bufferdate[32];
sprintf(bufferdate, "%02d.%02d.%02d", day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate), year(Headline_EffectiveEpochDate)); 
const String index = getDayOfWeekIndex(day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate),  year(Headline_EffectiveEpochDate));
const String epoch_headline= index;
tft.println( epoch_headline );
tft.print(bufferdate);tft.print(" "); 
tft.println(Headline_Text);  
tft.setTextColor(ST7735_YELLOW);
//tft.print( DailyForecasts_0_Date); 
//tft.print(DailyForecasts_0_EpochDate ); 
char bufferdate_0[32];
sprintf(bufferdate_0, "%02d.%02d.%02d", day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate), year(DailyForecasts_0_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_0);
const String index_0 = getDayOfWeekIndex(day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate),  year(DailyForecasts_0_EpochDate));
tft.println( index_0);
tft.print("Temp. Min: "); 
tft.print( DailyForecasts_0_Temperature_Minimum_Value); tft.print(" "); 
tft.print( DailyForecasts_0_Temperature_Minimum_Unit); tft.print(" "); 
tft.print("Temp. Max: "); 
tft.print(DailyForecasts_0_Temperature_Maximum_Value ); tft.print(" "); 
tft.print(DailyForecasts_0_Temperature_Maximum_Unit ); tft.print(" "); 
tft.println(DailyForecasts_0_Day_IconPhrase ); 
//tft.print(DailyForecasts_0_Day_HasPrecipitation ); 
if (DailyForecasts_0_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  
tft.println(DailyForecasts_0_Night_IconPhrase ); 
//tft.println(DailyForecasts_0_Night_HasPrecipitation ); 
if (DailyForecasts_0_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
 
tft.setTextColor(ST7735_MAGENTA);
//tft.print( DailyForecasts_1_Date); 
//tft.print(DailyForecasts_1_EpochDate );
char bufferdate_1[32];
sprintf(bufferdate_1, "%02d.%02d.%02d", day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate), year(DailyForecasts_1_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_1); 
const String index_1 = getDayOfWeekIndex(day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate),  year(DailyForecasts_1_EpochDate));
tft.println( index_1);
tft.print("Temp. Min: "); 
tft.print( DailyForecasts_1_Temperature_Minimum_Value);  tft.print(" "); 
tft.print( DailyForecasts_1_Temperature_Minimum_Unit); tft.print(" "); 
tft.print("Temp. Max: "); 
tft.print(DailyForecasts_1_Temperature_Maximum_Value );  tft.print(" "); 
tft.print(DailyForecasts_1_Temperature_Maximum_Unit ); tft.print(" "); 
tft.println(DailyForecasts_1_Day_IconPhrase ); 
//tft.print(DailyForecasts_1_Day_HasPrecipitation ); 
if (DailyForecasts_1_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  
tft.println(DailyForecasts_1_Night_IconPhrase ); 
//tft.println(DailyForecasts_1_Night_HasPrecipitation ); 
if (DailyForecasts_1_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
  

tft.setTextColor(ST7735_BLUE);
//tft.print( DailyForecasts_2_Date); 
//tft.print(DailyForecasts_2_EpochDate );
char bufferdate_2[32];
sprintf(bufferdate_2, "%02d.%02d.%02d", day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate), year(DailyForecasts_2_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_2);  
const String index_2 = getDayOfWeekIndex(day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate),  year(DailyForecasts_2_EpochDate));
tft.println(index_2);
tft.print("Temp. Min: ");
tft.print( DailyForecasts_2_Temperature_Minimum_Value); tft.print(" ");
tft.print( DailyForecasts_2_Temperature_Minimum_Unit); tft.print(" "); 
tft.print("Temp. Max: ");
tft.print(DailyForecasts_2_Temperature_Maximum_Value ); tft.print(" ");
tft.print(DailyForecasts_2_Temperature_Maximum_Unit ); tft.print(" "); 
tft.println(DailyForecasts_2_Day_IconPhrase ); 
//tft.print(DailyForecasts_2_Day_HasPrecipitation ); 
if (DailyForecasts_2_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
tft.println(DailyForecasts_2_Night_IconPhrase ); 
//tft.print(DailyForecasts_2_Night_HasPrecipitation ); 
if (DailyForecasts_2_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
  
tft.setTextColor(ST7735_RED);
//tft.print( DailyForecasts_3_Date); 
//tft.print(DailyForecasts_3_EpochDate );
char bufferdate_3[32];
sprintf(bufferdate_3, "%02d.%02d.%02d", day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate), year(DailyForecasts_3_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_3);  
const String index_3 = getDayOfWeekIndex(day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate),  year(DailyForecasts_3_EpochDate));
tft.println( index_3);
tft.print("Temp. Min: ");
tft.print( DailyForecasts_3_Temperature_Minimum_Value); tft.print(" ");
tft.print( DailyForecasts_3_Temperature_Minimum_Unit); tft.print(" "); 
tft.print("Temp. Max: ");
tft.print(DailyForecasts_3_Temperature_Maximum_Value ); tft.print(" ");
tft.print(DailyForecasts_3_Temperature_Maximum_Unit ); tft.print(" "); 
tft.println(DailyForecasts_3_Day_IconPhrase ); 
//tft.print(DailyForecasts_3_Day_HasPrecipitation ); 
if (DailyForecasts_3_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
tft.println(DailyForecasts_3_Night_IconPhrase ); 
//tft.print(DailyForecasts_3_Night_HasPrecipitation ); 
if (DailyForecasts_3_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }

tft.setTextColor(ST7735_CYAN);
tft.print(" "); 

  
//delay(1000);


 
  // Print values first day

  
  
  Serial.println(DailyForecasts_0_Date);
  Serial.println(DailyForecasts_0_Temperature_Minimum_Value);
  Serial.println(DailyForecasts_0_Temperature_Maximum_Value);
  Serial.println(DailyForecasts_0_Temperature_Maximum_Unit);
  Serial.println(DailyForecasts_0_Day_IconPhrase);
  Serial.println(DailyForecasts_0_Day_HasPrecipitation);
  Serial.println(DailyForecasts_0_Night_IconPhrase);
  Serial.println(DailyForecasts_0_Night_HasPrecipitation);
  
delay(1000*60*60); //retraso 1 hora para evitar requests has been exceeded de la API de Accuweather

 
}//Fin loop


//obtiene dia de la semana 
String getDayOfWeekIndex(int d, int m, int y) {
   int n =0;
    n = ((14 - m) / 12);
    y = (y - n);
    m = (m + (12 * n) - 2);
    return daysOfWeek[((d + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7)];
}
