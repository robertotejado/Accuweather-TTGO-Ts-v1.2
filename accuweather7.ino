#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>                               

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <string>
#include <iostream>


#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button

// input pullup enabled by default
RBD::Button left_button(35);
RBD::Button middle_button(34);
RBD::Button right_button(39);


//int pulsadorA=35;
//int pulsadorB=34;
//int pulsadorC=39;

unsigned long tiempoPrevio=0;

const static String daysOfWeek[] = {
    "Domingo",
    "Lunes",
    "Martes",
    "Miercoles",
    "Jueves",
    "Viernes",
    "Sabado"
    
};

 

//funcion obtiene dia de la semana 
 String getDayOfWeekIndex(int d, int m, int y) {
   int n =0;
    n = ((14 - m) / 12);
    y = (y - n);
    m = (m + (12 * n) - 2);
    return daysOfWeek[((d + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7)];
}

 char* removeAccented(  char* str ) {
    char *p = str;
    while ( (*p)!=0 ) {
        const char*
        //   "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
        tr = "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";
        unsigned char ch = (*p);
        if ( ch >=192 ) {
            (*p) = tr[ ch-192 ];
        }
        ++p; // http://stackoverflow.com/questions/14094621/
    }
    return str;
}

const char* ssid = "xxxxxxxxxxxxxxxxxxxxxxxxx";
const char* password =  "xxxxxxxxxxxxxxxxxxxxxxxxxx";
WiFiServer server(80);
 
 // delay(100);
  
// Client variables 
char linebuf[80];
int charcount=0;


const String endpoint = "http://dataservice.accuweather.com/forecasts/v1/daily/5day/xxxxxxx.json?apikey=xxxxxxxxxxxxxxxxxxxxxxx&language=es&metric=true"; // pronostico a 5 dias


const char* Headline_EffectiveDate = ""; // "2019-08-04T08:00:00+02:00"
long Headline_EffectiveEpochDate = 0; // 1564898400
const char* Headline_Text = ""; // "Agradable el domingo"
char* Texto_Cabecera ="";
String texto_cabecera = "";
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
   tiempoPrevio =millis();
  Serial.begin(115200);
   while(!Serial) {
  }
 
 
 IniciaWifiyServer();
   

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
 DailyForecasts_2_Night_IconPhrase =DailyForecasts_2_Night["IconPhrase"]; // "Nubes y claros"
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



InicializaPantallaInicial();

  // delay(5000); //cada 5 segundos

  
}
 
void loop() {

    PantallaInicial();

    delay(5000);  

     if(left_button.onPressed()) {
      
       
    boton_izquierda_presionado();
    Serial.println("Left Button Pressed");
    delay(10000);  
    
     
      }
    
    
     if(middle_button.onPressed()) {
       
     boton_medio_presionado();
     Serial.println("Middle Button Pressed");
       delay(10000);  
    
     
      }  
       
    
    if(right_button.onPressed()) {
        
        
     boton_derecha_presionado(); 
      Serial.println("Right Button Pressed");
     delay(10000);  
     
     
      }
    
     if(right_button.onReleased()  ) {
          
     boton_derecha_libre();
     Serial.println("Right Button Released");     
      delay(10000);  
      }
      


   

 
  
   PaginaWeb();  
 // delay(8000);

 
  // Print values first day


  
  Serial.println(DailyForecasts_0_Date);
  Serial.println(DailyForecasts_0_Temperature_Minimum_Value);
  Serial.println(DailyForecasts_0_Temperature_Maximum_Value);
  Serial.println(DailyForecasts_0_Temperature_Maximum_Unit);
  Serial.println(DailyForecasts_0_Day_IconPhrase);
  Serial.println(DailyForecasts_0_Day_HasPrecipitation);
  Serial.println(DailyForecasts_0_Night_IconPhrase);
  Serial.println(DailyForecasts_0_Night_HasPrecipitation);
  
  //delay(1000*60*60); //retraso 1 hora para evitar requests has been exceeded de la API de Accuweather
  //while(millis()-tiempoPrevio < 1000*60*60) {}
  delay(5000);
 
}//Fin loop

void IniciaWifiyServer() {
  
    WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.begin();
  Serial.println("Web server started!");
  }
 
void InicializaPantallaInicial() {
  
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



  
  }

void PantallaInicial(){
  
//  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.setTextSize(0); 
   
String hora="";
String minuto="";
String segundo="";
String titulo= "Accuweather ";
//String frase ="Es la hora   de las   Tortas!";
tft.fillScreen(ST7735_BLACK); //Rellena Pantalla en Negro

tft.setTextColor(ST7735_WHITE);
tft.setTextSize(1);
tft.println(titulo+"");
tft.setTextSize(0); 
tft.setTextColor(ST7735_GREEN);
//tft.print(Headline_EffectiveDate); 
//tft.print(Headline_EffectiveEpochDate); 
char bufferdate[32];
sprintf(bufferdate, "%02d.%02d.%02d", day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate), year(Headline_EffectiveEpochDate)); 
 String index = getDayOfWeekIndex(day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate),  year(Headline_EffectiveEpochDate));
 String epoch_headline= index;
tft.print(" "); tft.print(" "); 
tft.println(epoch_headline );tft.print(" "); 
tft.print(bufferdate);tft.println(" "); 
tft.print(" "); 
tft.println(Headline_Text);  tft.print(" "); 
tft.println("Servidor Web en: " );  tft.print( WiFi.localIP()); 

  }

  
void PaginaWeb() {
  
  
  /////////////////////////////SERVER/////////////////////
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) 
  {
    Serial.println("New client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
 
        if (c == '\n' && currentLineIsBlank) 
        {
          // send a standard http response header
         
client.println("<!DOCTYPE html>");
client.println("<html>");
client.println("<head>");
client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
client.println("<style>");
client.println("* {");
client.println("box-sizing: border-box;");
client.println("}");

client.println("#myInput {");
client.println("  background-image: url('/css/searchicon.png');");
client.println("  background-position: 10px 10px;");
client.println("  background-repeat: no-repeat;");
client.println("  width: 100%;");
client.println("  font-size: 16px;");
client.println("  padding: 12px 20px 12px 40px;");
client.println("  border: 1px solid #ddd;");
client.println("  margin-bottom: 12px;");
client.println("}");

client.println("#myTable {");
client.println("  border-collapse: collapse;");
client.println("  width: 100%;");
client.println("  border: 1px solid #ddd;");
client.println("  font-size: 18px;");
client.println("}");

client.println("#myTable th, #myTable td {");
client.println("  text-align: left;");
client.println("  padding: 12px;");
client.println("}");

client.println("#myTable tr {");
client.println(" border-bottom: 1px solid #ddd;");
client.println("}");

client.println("#myTable tr.header, #myTable tr:hover {");
client.println(" /* background-color: #f1f1f1; ****/");

client.println("}");

client.println("</style>");
client.println(" <link rel=\"stylesheet\" href=\"http://bootswatch.com/4/cyborg/bootstrap.css\">");
client.println(" <link rel=\"stylesheet\" href=\"http://bootswatch.com/_assets/css/custom.min.css\">");
client.println("</head>");
client.println("<body>");
client.println("<div class=\"container\">");

client.println("<img src=\"https://vortex.accuweather.com/adc2010/images/slate/AccuWeather_LogoOrange.svg\" width=\"20%\" height=\"20%\" />");
client.println("  <h1 class=\"page-header\">Accuweather de la </h1>");
client.println("<div id=\"flip-scroll\">");
client.println("<h2>Semana</h2>");
client.println("<p>");
client.println(WiFi.localIP());
client.println("</p>");
client.println("<div class=\"alert alert-error\">");
client.println(" <button class=\"btn btn-primary btn-danger\">");
char bufferfecha[32];
sprintf(bufferfecha, "%02d.%02d.%02d", day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate), year(Headline_EffectiveEpochDate)); 
  String diasemana = getDayOfWeekIndex(day(Headline_EffectiveEpochDate), month(Headline_EffectiveEpochDate),  year(Headline_EffectiveEpochDate));
  String epoch_headline= diasemana;
client.println(" ");
client.println(epoch_headline );  client.print("-");
client.println(bufferfecha);  client.print("-");
 texto_cabecera = String(Headline_Text);
client.println(texto_cabecera);
client.println("</button>");
client.println("</div>");
client.println("<input type=\"text\" id=\"myInput\" onkeyup=\"myFunction()\" placeholder=\"Busca por dia..\" title=\"tipo de dia...\">");

client.println("<table id=\"myTable\" class=\"table table-hover\">");
client.println(" <tr class=\"headerxx\" class=\"table-danger\">");
client.println("    <th style=\"width:60%;\">Dia de la semana</th>");
client.println("  <th style=\"width:60%;\">Fecha</th>");
    
client.println("  <th style=\"width:40%;\">Temp. Min.</th>");
client.println("  <th style=\"width:40%;\">Temp. Max.</th>");

client.println("<th style=\"width:40%;\">Descripcion dia</th>");
client.println("<th style=\"width:40%;\">Precipitacion dia</th>");

client.println("<th style=\"width:40%;\">Descripcion noche</th>");
client.println("<th style=\"width:40%;\">Precipitacion noche</th>");
client.println("  </tr>");
client.println("  <tr>");
client.println("   <td>"); 
const String diasemana_0 = getDayOfWeekIndex(day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate),  year(DailyForecasts_0_EpochDate));
client.println( diasemana_0);
client.println("</td>");
client.println("   <td>");
  char bufferfecha_0[32];
sprintf(bufferfecha_0, "%02d.%02d.%02d", day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate), year(DailyForecasts_0_EpochDate)); 
client.print(" "); 
client.println(bufferfecha_0);


client.println(" </td>");
client.println("   <td>");
client.print( DailyForecasts_0_Temperature_Minimum_Value); client.print(" "); 
client.print( DailyForecasts_0_Temperature_Minimum_Unit); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.print(DailyForecasts_0_Temperature_Maximum_Value ); client.print(" "); 
client.print(DailyForecasts_0_Temperature_Maximum_Unit ); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.println(DailyForecasts_0_Day_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_0_Day_HasPrecipitation){
client.println(" Con lluvia por el dia"); 
}else
{
client.println(" Sin lluvia por el dia"); 
  }
client.println(" </td>");
client.println(" <td>");
client.println(DailyForecasts_0_Night_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_0_Night_HasPrecipitation){
client.println(" Con lluvia por la noche"); 
}else
{
client.println(" Sin lluvia por la noche"); 
  }
client.println(" </td>");
client.println(" </tr>");
 
client.println("  <tr>");
client.println("   <td>"); 
const String diasemana_1 = getDayOfWeekIndex(day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate),  year(DailyForecasts_1_EpochDate));
client.println( diasemana_1);
client.println("</td>");
client.println("   <td>");
char bufferfecha_1[32];
sprintf(bufferfecha_1, "%02d.%02d.%02d", day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate), year(DailyForecasts_1_EpochDate)); 
client.print(" "); 
client.println(bufferfecha_1);
client.println(" </td>");
client.println("   <td>");
client.print( DailyForecasts_1_Temperature_Minimum_Value); client.print(" "); 
client.print( DailyForecasts_1_Temperature_Minimum_Unit); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.print(DailyForecasts_1_Temperature_Maximum_Value ); client.print(" "); 
client.print(DailyForecasts_1_Temperature_Maximum_Unit ); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.println(DailyForecasts_1_Day_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_1_Day_HasPrecipitation){
client.println(" Con lluvia por el dia"); 
}else
{
client.println(" Sin lluvia por el dia"); 
  }
 client.println(" </td>");
client.println(" <td>");
client.println(DailyForecasts_1_Night_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_1_Night_HasPrecipitation){
client.println(" Con lluvia por la noche"); 
}else
{
client.println(" Sin lluvia por la noche"); 
  }
client.println(" </td>");
client.println(" </tr>");
 


client.println("  <tr>");
client.println("   <td>"); 
const String diasemana_2 = getDayOfWeekIndex(day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate),  year(DailyForecasts_2_EpochDate));
client.println( diasemana_2);
client.println("</td>");
client.println("   <td>");
  char bufferfecha_2[32];
sprintf(bufferfecha_2, "%02d.%02d.%02d", day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate), year(DailyForecasts_2_EpochDate)); 
client.print(" "); 
client.println(bufferfecha_2);


client.println(" </td>");
client.println("   <td>");
client.print( DailyForecasts_2_Temperature_Minimum_Value); client.print(" "); 
client.print( DailyForecasts_2_Temperature_Minimum_Unit); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.print(DailyForecasts_2_Temperature_Maximum_Value ); client.print(" "); 
client.print(DailyForecasts_2_Temperature_Maximum_Unit ); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.println(DailyForecasts_2_Day_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_2_Day_HasPrecipitation){
client.println(" Con lluvia por el dia"); 
}else
{
client.println(" Sin lluvia por el dia"); 
  }
client.println(" </td>");
client.println(" <td>");
client.println(DailyForecasts_2_Night_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_2_Night_HasPrecipitation){
client.println(" Con lluvia por la noche"); 
}else
{
client.println(" Sin lluvia por la noche"); 
  }
client.println(" </td>");
client.println(" </tr>");
 

  
client.println("  <tr>");
client.println("   <td>"); 
const String diasemana_3 = getDayOfWeekIndex(day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate),  year(DailyForecasts_3_EpochDate));
client.println( diasemana_3);
 client.println("</td>");
client.println("   <td>");
char bufferfecha_3[32];
sprintf(bufferfecha_3, "%02d.%02d.%02d", day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate), year(DailyForecasts_3_EpochDate)); 
client.print(" "); 
client.println(bufferfecha_3);


client.println(" </td>");
client.println("   <td>");
client.print( DailyForecasts_3_Temperature_Minimum_Value); client.print(" "); 
client.print( DailyForecasts_3_Temperature_Minimum_Unit); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.print(DailyForecasts_3_Temperature_Maximum_Value ); client.print(" "); 
client.print(DailyForecasts_3_Temperature_Maximum_Unit ); client.print(" "); 
client.println(" </td>");
client.println("   <td>");
client.println(DailyForecasts_3_Day_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_3_Day_HasPrecipitation){
client.println(" Con lluvia por el dia"); 
}else
{
client.println(" Sin lluvia por el dia"); 
  }
client.println(" </td>");
client.println(" <td>");
client.println(DailyForecasts_3_Night_IconPhrase ); 
client.println(" </td>");
client.println("  <td>");
if (DailyForecasts_3_Night_HasPrecipitation){
client.println(" Con lluvia por la noche"); 
}else
{
client.println(" Sin lluvia por la noche"); 
  }
client.println(" </td>");
client.println(" </tr>");


  
client.println("</table>");
client.println(" </div> </div>");
  
client.println(" <h4></h4>");
  
client.println("<script>");
client.println("function myFunction() {");
client.println("  var input, filter, table, tr, td, i, txtValue;");
client.println(" input = document.getElementById(\"myInput\");");
client.println("  filter = input.value.toUpperCase();");
client.println(" table = document.getElementById(\"myTable\");");
client.println(" tr = table.getElementsByTagName(\"tr\");");
client.println("  for (i = 0; i < tr.length; i++) {");
client.println("   td = tr[i].getElementsByTagName(\"td\")[0];");
client.println("  if (td) {");
client.println("     txtValue = td.textContent || td.innerText;");
client.println("     if (txtValue.toUpperCase().indexOf(filter) > -1) {");
client.println("       tr[i].style.display = \"\";");
client.println("     } else {");
client.println("       tr[i].style.display = \"none\";");
client.println("    }");
client.println("  }       ");
client.println(" }");
client.println("}");
client.println("</script>");

client.println("</body>");
client.println("</html>");
delay(1000);

          
          break;
        }
        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
          if (strstr(linebuf,"GET /on") > 0)
          {
            Serial.println("LED ON");
            //digitalWrite(led, HIGH);
          }
          else if (strstr(linebuf,"GET /off") > 0)
          {
            Serial.println("LED OFF");
            //digitalWrite(led, LOW);
          }
 
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;
        } 
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
 
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
   
/////////////////////////////////////
  
  } 


void boton_izquierda_presionado(){

  //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.setTextSize(0); 
   
tft.setTextColor(ST7735_YELLOW);
//tft.print( DailyForecasts_0_Date); 
//tft.print(DailyForecasts_0_EpochDate ); 
char bufferdate_0[32];
sprintf(bufferdate_0, "%02d.%02d.%02d", day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate), year(DailyForecasts_0_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_0);
 String index_0 = getDayOfWeekIndex(day(DailyForecasts_0_EpochDate), month(DailyForecasts_0_EpochDate),  year(DailyForecasts_0_EpochDate));
tft.setTextSize(1);tft.print(" "); 
tft.println( index_0);
tft.setTextSize(0);
tft.print(" "); 
tft.println("Temp. Min: "); tft.print(" "); 
tft.print( DailyForecasts_0_Temperature_Minimum_Value); tft.print(" "); 
tft.print( DailyForecasts_0_Temperature_Minimum_Unit); tft.print(" "); 
tft.println("Temp. Max: "); 
tft.print(" "); 
tft.print(DailyForecasts_0_Temperature_Maximum_Value ); tft.print(" "); 
tft.print(DailyForecasts_0_Temperature_Maximum_Unit ); tft.print(" "); 
tft.println(DailyForecasts_0_Day_IconPhrase ); 
//tft.print(DailyForecasts_0_Day_HasPrecipitation ); 
tft.print(" "); 
if (DailyForecasts_0_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  tft.print(" "); 
tft.println(DailyForecasts_0_Night_IconPhrase ); 
//tft.println(DailyForecasts_0_Night_HasPrecipitation ); 
tft.print(" "); 
if (DailyForecasts_0_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
  tft.print(" "); 
 
 //delay(8000);
  Serial.println("Left Button Pressed");

}

void boton_medio_presionado(){

 // tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.setTextSize(0); 
  
tft.setTextColor(ST7735_MAGENTA);
//tft.print( DailyForecasts_1_Date); 
//tft.print(DailyForecasts_1_EpochDate );
char bufferdate_1[32];
sprintf(bufferdate_1, "%02d.%02d.%02d", day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate), year(DailyForecasts_1_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_1); 
 String index_1 = getDayOfWeekIndex(day(DailyForecasts_1_EpochDate), month(DailyForecasts_1_EpochDate),  year(DailyForecasts_1_EpochDate));
tft.setTextSize(1);tft.print(" "); 
tft.println( index_1);
tft.setTextSize(0);
tft.print(" "); 
tft.println("Temp. Min: "); tft.print(" "); 
tft.print( DailyForecasts_1_Temperature_Minimum_Value);  tft.print(" "); 
tft.print( DailyForecasts_1_Temperature_Minimum_Unit); tft.print(" "); 
tft.print(" "); 
tft.println("Temp. Max: "); tft.print(" "); 
tft.print(DailyForecasts_1_Temperature_Maximum_Value );  tft.print(" "); 
tft.print(DailyForecasts_1_Temperature_Maximum_Unit ); tft.print(" "); 
tft.print(" "); 
tft.println(DailyForecasts_1_Day_IconPhrase ); 
//tft.print(DailyForecasts_1_Day_HasPrecipitation ); 
tft.print(" "); 
if (DailyForecasts_1_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  tft.print(" "); 
tft.println(DailyForecasts_1_Night_IconPhrase ); 
//tft.println(DailyForecasts_1_Night_HasPrecipitation ); 
if (DailyForecasts_1_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }

  tft.print(" "); 
   
 //delay(8000);
  Serial.println("Middle Button Pressed");

}

void boton_derecha_presionado(){

 //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.setTextSize(0); 
   
tft.setTextColor(ST7735_BLUE);
//tft.print( DailyForecasts_2_Date); 
//tft.print(DailyForecasts_2_EpochDate );
char bufferdate_2[32];
sprintf(bufferdate_2, "%02d.%02d.%02d", day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate), year(DailyForecasts_2_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_2);  
 String index_2 = getDayOfWeekIndex(day(DailyForecasts_2_EpochDate), month(DailyForecasts_2_EpochDate),  year(DailyForecasts_2_EpochDate));
tft.setTextSize(1);tft.print(" "); 
tft.println(index_2);
tft.setTextSize(0);tft.print(" "); 
tft.println("Temp. Min: ");tft.print(" "); 
tft.print( DailyForecasts_2_Temperature_Minimum_Value); tft.print(" ");
tft.print( DailyForecasts_2_Temperature_Minimum_Unit); tft.print(" "); tft.print(" "); 
tft.println("Temp. Max: ");tft.print(" "); 
tft.print(DailyForecasts_2_Temperature_Maximum_Value ); tft.print(" ");
tft.print(DailyForecasts_2_Temperature_Maximum_Unit ); tft.print(" "); tft.println(" "); 
tft.println(DailyForecasts_2_Day_IconPhrase ); 
//tft.print(DailyForecasts_2_Day_HasPrecipitation ); 
if (DailyForecasts_2_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  tft.print(" "); 
tft.println(DailyForecasts_2_Night_IconPhrase ); 
//tft.print(DailyForecasts_2_Night_HasPrecipitation ); 
tft.print(" "); 
if (DailyForecasts_2_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
  tft.print(" "); 
  
  // delay(8000);
  Serial.println("Right Button Pressed");

}

void boton_derecha_libre(){

 // tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(5, 5); //Cursor position in screen.
  tft.setTextSize(0); 
   
tft.setTextColor(ST7735_RED);
//tft.print( DailyForecasts_3_Date); 
//tft.print(DailyForecasts_3_EpochDate );
char bufferdate_3[32];
sprintf(bufferdate_3, "%02d.%02d.%02d", day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate), year(DailyForecasts_3_EpochDate)); 
tft.print(" "); 
tft.println(bufferdate_3);  
 String index_3 = getDayOfWeekIndex(day(DailyForecasts_3_EpochDate), month(DailyForecasts_3_EpochDate),  year(DailyForecasts_3_EpochDate));
tft.setTextSize(1);tft.print(" "); 
tft.println( index_3);
tft.setTextSize(0);tft.print(" "); 
tft.println("Temp. Min: ");tft.print(" "); 
tft.print( DailyForecasts_3_Temperature_Minimum_Value); tft.print(" ");
tft.print( DailyForecasts_3_Temperature_Minimum_Unit); tft.print(" "); tft.print(" "); 
tft.println("Temp. Max: "); tft.print(" "); 
tft.print(DailyForecasts_3_Temperature_Maximum_Value ); tft.print(" ");
tft.print(DailyForecasts_3_Temperature_Maximum_Unit ); tft.print(" "); tft.print(" "); 
tft.println(DailyForecasts_3_Day_IconPhrase ); tft.print(" "); 
//tft.print(DailyForecasts_3_Day_HasPrecipitation ); 
if (DailyForecasts_3_Day_HasPrecipitation){
tft.println(" Con lluvia al dia"); 
}else
{
tft.println(" Sin lluvia al dia"); 
  }
  tft.print(" "); 
tft.println(DailyForecasts_3_Night_IconPhrase ); 
tft.print(" "); 
//tft.print(DailyForecasts_3_Night_HasPrecipitation ); 
if (DailyForecasts_3_Night_HasPrecipitation){
tft.println(" Con lluvia en noche"); 
}else
{
tft.println(" Sin lluvia en noche"); 
  }
  tft.print(" "); 
  
  
 //delay(8000);


tft.setTextColor(ST7735_CYAN);
tft.print(" "); 

 Serial.println("Right Button Released");

}
