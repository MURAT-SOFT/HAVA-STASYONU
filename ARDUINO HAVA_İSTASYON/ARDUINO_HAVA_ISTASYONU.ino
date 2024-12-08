//***********************************************************************************
//          C++  HAVA ISTASYONU  HG-1  MURAT_IRKAN  27-01-2022
//               DONANIM GÜNCELLEMESI DS1307 CHIP 13-02-2022 V.1.02
//               GÜNCELLEME CIHAZA MICRO-SD KART EKLENDI.! 16-02-2022
//***********************************************************************************
#include <Wire.h> // BAGLANTI
#include <SD.h> // MIKRO-SD KART  SICAKLIK BASINÇ VE NEM BILGILERI KAYIT EDILECEK.!
#include <SPI.h> // SPI BAGLANTI KÜTÜPHANESI
#include <Adafruit_Sensor.h> // SENSOR TANIMLARI KÜTÜPHANESI
#include <Adafruit_BME280.h>// BOSCH BME280 SENSOR
#include <TFT.h>  // ST7735R VEYA S MODELI Arduino tft-lcd 1.8 INÇ LCD EKRAN
#include <Adafruit_I2CDevice.h> // I2C BAGLANTI KÜTÜPHANESI
#include <RealTimeClockDS1307.h>  // DS1307 GERÇEK ZAMANLAMA CHIP
// spi BAGLANTI PROTOKOL BOSCH BME280 PIN baglantisi
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
// BITMAP ANIMASYON DEGISKEN
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
//*****************************
// Sitronix st7735r-s pin baglantisi
#define cs   10
#define dc   9
#define rst  8
#define LOG_INTERVAL  1000
// ST7735R EKRAN BAGLANTISI   OLUSTUR.!
TFT TFT_EKRAN = TFT(cs, dc, rst);
//**************************************
String str = "sicaklik";
int SAAT, DAKIKA, SANIYE;
int yil,ay,gun;
#define kayit_saniyesi 1000 // KARTA YAZIM SÜRESI
uint32_t syncTime = 0; // time of last sync()
#define DENIZ_SEVIYESI_BASINCI = (1013.25) // SABIT DENIZ SEVIYESI BASINÇ DEGERI
BME280 bosch_bme280; //  SU ANDA I2C BAGLANTI KURUYORUZ
//digital pin 10 for the SD cs lineconst 
int chipSelect = 10;

// the logging file
File logfile;
//ADAFRUIT BOSCH BME280 SENSOR SPI BAGLANTI METODLARI 
//Adafruit_BME280 bme(BME_CS); // hardware SPI DONANIM SPI BAGLANTISI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // yazilim SPI

unsigned long GECIKME_ZAMANI; // SISTEMDE GEÇIKME DEGISKENI
// ds1307 SAAT AYARI DEGISKENLERI
const int ARTTIR=12;
const int AZALT=11;
const int SEC=10;
const int SET=9;
int ayar=0, saat_dk=0;
//DS1307 SAAT AYAR DEGISKENLERI SONU*******************************************
void setup() {
   Wire.begin();
 RTC.start();
    while(!Serial);    // serial EKRAN ÇALISIYOR.!
    Serial.println("BOSCH BME280  test");
Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
 //if (!SD.begin(chipSelect)) {
   // error("MICRO SD KART, HATASI");
 // }
TFT_EKRAN.basla();
  // EKRANIN TAMAMEN SILIYORUZ    BEYAZA   BOYA
     unsigned durum;
TFT_EKRAN.background(0, 245, 200);
TFT_EKRAN.setCursor(1,1);
TFT_EKRAN.setTextSize(3);
//TFT_EKRAN.print("HAVA ISTASYON");
     durum = bosch_bme280.begin();  
    // status = bme.begin(0x76, &Wire2)
    if (!durum) {
        Serial.println("BME280 sensor BULUNAMADI.,i2c adreslerini kontrol edin.!");
        Serial.print("sensor kimligi : 0x"); Serial.println(bosch_bme280.sensorID(),16);
        Serial.print("        ID of 0xFF adres iyi degil, a BMP 180 or BMP 085\n");
        Serial.print("        ID  0x56-0x58 ayar  BMP 280,\n");
        Serial.print("        ID o0x60 ayar BME 280.\n");
        Serial.print("        ID of 0x61 ayar  BME 680.\n");
     
 while (1) delay(10);
    }
   Serial.println("-- normal Test --");
Serial.println();

}
void loop() { 
TFT_EKRAN.background(0, 0, 0);
saat_oku();
TFT_EKRAN.invertDisplay(false);
degerleri_yaz();
KURE();
//CIZGI();
daire_ciz(7,ST7735_GREEN);// DAIRE DESENLERI CIZILIYOR.! 
delay(300);
}
void degerleri_yaz() {
TFT_EKRAN.background(0, 0, 0);
TFT_EKRAN.setTextSize(2);
TFT_EKRAN.setCursor(1,5);
TFT_EKRAN.print("Hava istasyon");
TFT_EKRAN.setCursor(1,45);
TFT_EKRAN.print("YUKSEKLIK");
TFT_EKRAN.setCursor(10,70);
TFT_EKRAN.println((float)bosch_bme280.readAltitude(1),1);
delay(1000);
TFT_EKRAN.background(245,10 , 255);
 TFT_EKRAN.setTextSize(2);
 TFT_EKRAN.setCursor(1,1);
//TFT_EKRAN.print("HAVA ISTASYON");
TFT_EKRAN.setCursor(1,1);
TFT_EKRAN.setTextSize(2);
TFT_EKRAN.print("SICAKLIK");
TFT_EKRAN.setTextSize(2);
TFT_EKRAN.setCursor(5,20);
TFT_EKRAN.println((float)bosch_bme280.readTemperature(),1);
TFT_EKRAN.setCursor(2,38);
TFT_EKRAN.print("BASINC");
TFT_EKRAN.setCursor(5,57);
TFT_EKRAN.println((float)bosch_bme280.readPressure(),1);
TFT_EKRAN.setCursor(5,78);
TFT_EKRAN.print("NEM:");
TFT_EKRAN.setCursor(5,95);
TFT_EKRAN.println((float)bosch_bme280.readHumidity(),1);
TFT_EKRAN.invertDisplay( true);
delay(1500);
TFT_EKRAN.invertDisplay( false);
delay(500);
TFT_EKRAN.invertDisplay( true);
delay(500);
TFT_EKRAN.invertDisplay( false);
delay(500);
TFT_EKRAN.invertDisplay( true);
delay(100);
TFT_EKRAN.invertDisplay( false);
delay(100);
daire_ciz(8,ST7735_SARI);
}

void KURE(void){
int x1 = TFT_EKRAN.width()/2;
int y1 = TFT_EKRAN.height()/2;
TFT_EKRAN.background(0, 0, 0);
 for (int r1=0; r1<64; r1++) {
  TFT_EKRAN.fillCircle(x1, y1, r1,ST7735_KIRMIZI);  
   }  
for (int r2=0; r2<32; r2++) { 
 TFT_EKRAN.fillCircle(x1, y1, r2,ST7735_WHITE);
delay(20);
 }
 }

void CIZGI(void) {
  for (uint8_t i=0; i<4; i++) {
   TFT_EKRAN.fillScreen(ST7735_KIRMIZI);
 TFT_EKRAN.drawLine(TFT_EKRAN.width()/2,TFT_EKRAN.height()/2,0,0,ST7735_WHITE);    

TFT_EKRAN.setRotation(TFT_EKRAN.getRotation()+1);
TFT_EKRAN.fillScreen(ST7735_SARI);
}


}
unsigned long daire_ciz(uint8_t cap, uint16_t renk) {
  unsigned long start;
  int x, y, w = TFT_EKRAN.width(), h = TFT_EKRAN.height(), r2 = cap * 2;
  TFT_EKRAN.fillScreen(ST7735_KIRMIZI);
  start = micros();
  for(x=cap; x<w;x+=r2) {
    for(y=cap; y<h; y+=r2) {
      TFT_EKRAN.fillCircle(x, y, cap, renk);
    }
 }
  return micros() - start;
}


void saat_ayar()
{
  if(digitalRead(SET)==HIGH)
  {
    while((digitalRead(SET)==HIGH))
    { 
      saat_oku();
     // LCD_saat();
    }
    ayar++;
    if(ayar==2)
    {
      ayar=0;
      saat_dk=0;
    }
  }
  if(ayar==1)
  {
    if(digitalRead(SEC)==HIGH)
    {
      while((digitalRead(SEC)==HIGH))
      { 
        saat_oku();
       // LCD_saat();
      }
      saat_dk++;
      if(saat_dk==2)
      {
        saat_dk=0;
      }
    }
    if((digitalRead(ARTTIR)==HIGH)&&(saat_dk==0))
    {
      int saat=RTC.getHours();
      while((digitalRead(ARTTIR)==HIGH))
      { 
        saat_oku();
      //  LCD_saat();
      }
      saat++;
      if(saat>23)
      {
        saat=0;
      }
      RTC.setHours(saat);
      RTC.setSeconds(0);
      RTC.setClock();
    }
    if((digitalRead(AZALT)==HIGH)&&(saat_dk==0))
    {
      int saat=RTC.getHours();
      while((digitalRead(AZALT)==HIGH))
      { 
        saat_oku();
     //   LCD_saat();
      }
      saat--;
      if(saat<0)
      {
        saat=23;
      }
      RTC.setHours(saat);
      RTC.setSeconds(0);
      RTC.setClock();
    }
    if((digitalRead(ARTTIR)==HIGH)&&(saat_dk==1))
    {
      int dk=RTC.getMinutes();
      while((digitalRead(ARTTIR)==HIGH))
      { 
        saat_oku();
        //LCD_saat();
      }
      dk++;
      if(dk>59)
      {
        dk=0;
      }
      RTC.setMinutes(dk);
      RTC.setSeconds(0);
      RTC.setClock();
    }
    if((digitalRead(AZALT)==HIGH)&&(saat_dk==1))
    {
      int dk=RTC.getMinutes();
      while((digitalRead(AZALT)==HIGH))
      { 
        saat_oku();
       // LCD_saat();
      }
      dk--;
      if(dk<0)
      {
        dk=59;
      }
      RTC.setMinutes(dk);
      RTC.setSeconds(0);
      RTC.setClock();
    }
  }
}
void saat_oku()
{
 RTC.readClock();

SAAT=RTC.getHours();
DAKIKA=RTC.getMinutes();
SANIYE=RTC.getSeconds();
yil = RTC.getYear();
ay = RTC.getMonth();
gun = RTC.getDay();

TFT_EKRAN.setTextSize(2);
TFT_EKRAN.setCursor(12,1);
TFT_EKRAN.print("Saat/Tarih");
TFT_EKRAN.setCursor(14,35);
TFT_EKRAN.println(SAAT);
TFT_EKRAN.setCursor(34,35);
TFT_EKRAN.print(":");
TFT_EKRAN.setCursor(49,35);
TFT_EKRAN.println(DAKIKA);
// yil ay gün
TFT_EKRAN.setCursor(1,55);
TFT_EKRAN.print("Yil:");
TFT_EKRAN.setCursor(48,55);
TFT_EKRAN.println(yil);
TFT_EKRAN.setCursor(1,74);
TFT_EKRAN.print("Ay:");
TFT_EKRAN.setCursor(45,74);
TFT_EKRAN.println(ay);
TFT_EKRAN.setCursor(1,98);
TFT_EKRAN.print("Gun");
TFT_EKRAN.setCursor(48,98);
TFT_EKRAN.println(gun);
//TFT_EKRAN.setCursor(55,2);
//TFT_EKRAN.println(seconds,1);  
delay(300);
TFT_EKRAN.invertDisplay( true);
delay(200);
TFT_EKRAN.invertDisplay( false);
delay(1000);
CIZGI_ANIMASYON(ST7735_GREEN);
RESIM(logo,16,16);
}

void kayit(){
 // yeni dosya olusturuyoruz.!
  char filename[] = "LOG.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // eyer log dosyasi yoksa sd kart olusturuyoruz.!
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // çikis islemi.!
//if (! logfile) {
  //  error("log dosyasi olusmadi.!"); hata fonksiyonu mesaji ekle.!
    //           }
  }
}
}
unsigned long CIZGI_ANIMASYON(uint16_t renk) {
  unsigned long BASLA, t;
  int           x1, y1, x2, y2,
                w = TFT_EKRAN.width(),
                h = TFT_EKRAN.height();

  TFT_EKRAN.background(ST7735_BLACK);
  yield();
  
  x1 = y1 = 0;
  y2    = h - 1;
  BASLA = micros();
  for(x2=0; x2<w; x2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  t     = micros() - BASLA; // fillScreen doesn't count against timing

  yield();
  TFT_EKRAN.background(ST7735_BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  BASLA = micros();
  for(x2=0; x2<w; x2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  t    += micros() - BASLA;

  yield();
  TFT_EKRAN.background(ST7735_BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  BASLA = micros();
  for(x2=0; x2<w; x2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  t    += micros() - BASLA;

  yield();
  TFT_EKRAN.background(ST7735_BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  BASLA = micros();
  for(x2=0; x2<w; x2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) TFT_EKRAN.drawLine(x1, y1, x2, y2, renk);

  yield();
  return micros() - BASLA;
//RESIM(logo,16,16);
}


void RESIM(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(TFT_EKRAN.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      TFT_EKRAN.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo, w, h,ST7735_WHITE);
    }
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
     TFT_EKRAN.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo, w, h,ST7735_BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > TFT_EKRAN.height()) {
        icons[f][XPOS] = random(TFT_EKRAN.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}
