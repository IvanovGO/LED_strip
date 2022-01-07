#include <Adafruit_NeoPixel.h>
#include "arduinoFFT.h"
#include <avr/wdt.h>


#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 5 // номер порта к которому подключен модуль
#define count_led 60 // количество светодиодов 
#define _temp 1 //
#define IS_BR if (sig_brake) {sig_brake=false; return;}
#define CHANNEL 0

 typedef struct pix{unsigned char r,g,b;}pix;

 bool sig_brake=false;
 
 Adafruit_NeoPixel  pixelss(count_led, PIN, NEO_GRB + NEO_KHZ800);//для каждой ленты надо создавать экземпляр дайвера
 
//--------------------------------------------------------------------------------------------- 
 arduinoFFT FFT = arduinoFFT();
 const uint16_t samples = 64;
 double vReal[samples];
double vImag[samples];
 
//---------------------------------------------------------------------------------------------
class Ribbon
{public:

pix ribbon[count_led];
//unsigned long ribbon[count_led];
Adafruit_NeoPixel * pixels;
  //----------------------CONSTRUCTOR
  Ribbon(Adafruit_NeoPixel * pix){pixels=pix; pixels->begin(); pixels->clear();for(unsigned int i=0;i<count_led;i++) setRibbon(i,0);}
  //----------------------SHOW
    void Show(){
      wdt_reset();
    pixels->clear();
    for (unsigned int x =0; x<count_led;x++ ) //выводим буфер в ленту
//  pixels.setPixelColor(x,pixels.gamma32(ribbon[x])); 
//        pixels->setPixelColor(x,pixels.ColorHSV(ribbon[x]));  
    pixels->setPixelColor(x,pixels->Color(ribbon[x].r,ribbon[x].g,ribbon[x].b));  
    pixels->show();  }
//------------------------Пишем однословный цвет в буфер (номер диода, цвет)
  void setRibbon(unsigned int i,char col){
 /*ribbon[i].r=(col>>5)*255/7;
  ribbon[i].g=((col>>2)&0x07)*255/7;
  ribbon[i].b=(col&3)*255/3;
 //   ribbon[i]=col;*/
  ribbon[i-1].r=col&0b1111000+16;
  ribbon[i-1].g=col&0b00111100+16;
  ribbon[i-1].b=col&0b00001111+16;  }
  void Clear(){
    for(unsigned int i=0;i<count_led;i++) setRibbon(i,0);
    pixels->clear();
    pixels->show(); 
    }  
  };



Ribbon * r = new Ribbon(&pixelss);//передаем адрес дравера ленты экземпляру ленты

/*----------------------------------------------------------------------
 * 
 * 
 ---------------------------------------------------------------------*/
void setup() {

attachInterrupt(1,button_tap,FALLING);

/*  Serial.begin(115200);
  while(!Serial);
  Serial.println("Ready");
  */
wdt_enable(WDTO_500MS);
//for (unsigned int col=0;col<count_led;col+=0xff/count_led)
//r->setRibbon(col,col*count_led);

}
/*----------------------------------------------------------------------
 * 
 * 
 ---------------------------------------------------------------------*/

void cycle_color(Ribbon * p){//вся лента поочередно всеми цветам
for (unsigned int col=0;col<0xff;col++){
for(unsigned int x=1;x<count_led+1;x++)
 {p->setRibbon(x,col);}delay(_temp);p->Show();IS_BR}  }

void sinus_wave(Ribbon * p) {//бегуая волна цветов по длинне ленты
for (unsigned int w=0;w<360;w++)
for(unsigned int x=1;x<count_led+1;x++){
  p->setRibbon(x,0xff*sin(x*count_led/3.14/2+w/2/3.14));
  p->Show();
  delay(_temp);IS_BR
}}

void snake(Ribbon * p){
  for (unsigned int x=1;x<count_led+1;x++){
   for (unsigned int y=x;y>x-0xf;y--) { p->setRibbon(y,(x-y)*0xf);
                                        p->Show();    
                                        delay(_temp);  IS_BR                               
                                      }   p->Clear(); }}

unsigned char mode=0;

#define MAXMODE 3

void button_tap(){
  mode++;
  if (mode>MAXMODE) mode=0;
  sig_brake=true;
  }
 
void loop() {  
for (unsigned int i=0;i<samples;i++)  {vReal[i]=analogRead(CHANNEL);
                                       vImag[i]=0;}
  FFT.Windowing(vReal,samples,FFT_WIN_TYP_HAMMING,FFT_FORWARD);
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  //FFT.ComplexToMagnitude(vReal, vImag, samples);
  int max=0;
  for (unsigned int i=0;i<samples;i++) {if(abs((int)vReal[i])>max) max=abs((int)vReal[i]);}
  r->Clear();
  for (unsigned int i=0;i<max/50;i++) {
    r->setRibbon(i+1,abs((int)vReal[i]));
     r->Show();
    
     
  }

// for (unsigned int i=0;i<samples;i++) {//Serial.println(i);
//                                        Serial.println(vReal[i]);}
 
/*
switch (mode){
case 0:r->Clear();break;
case 1:cycle_color(r);break;
case 2:sinus_wave(r);break;
case 3:snake(r);break;
}*/
}
