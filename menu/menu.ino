#include <OneWire.h>
#include <GyverOLED.h>
#include "IRremote.h"

#define LATCH_REG 6
#define DATA_REG 4
#define CLK_REG 5

#define MAX_MENU 16
#define AM_OUT A0


IRrecv irrecv(2);
decode_results results;
GyverOLED oled;

const char arr_01[] PROGMEM = "Temp A:";
const char arr_02[] PROGMEM = "Temp B:";
const char arr_03[] PROGMEM = "Temp C:";
const char arr_04[] PROGMEM = "Temp D:";
const char arr_05[] PROGMEM = "Temp E:";
const char arr_06[] PROGMEM = "Temp F:";
const char arr_07[] PROGMEM = "Temp G:";
const char arr_08[] PROGMEM = "Temp H:";
const char arr_09[] PROGMEM = "Temp I:";
const char arr_10[] PROGMEM = "Temp J:";
const char arr_11[] PROGMEM = "Temp K:";
const char arr_12[] PROGMEM = "Temp L:";
const char arr_13[] PROGMEM = "Temp M:";
const char arr_14[] PROGMEM = "Temp N:";
const char arr_15[] PROGMEM = "Temp O:";
const char arr_16[] PROGMEM = "Temp O:";
const char* const arr[MAX_MENU] PROGMEM = {arr_01, arr_02, arr_03, arr_04, arr_05,
                                           arr_06, arr_07, arr_08, arr_09, arr_10,
                                           arr_11, arr_12, arr_13, arr_14, arr_15, arr_16
                                          };
float num_arr[MAX_MENU] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8.0, 9.0, 10.0, 1.10, 1.2, 10.3, 1.41, 1.50, 0.0};

uint8_t pointer = 0, first = 0;
uint8_t incoming [10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t counter_inc = 0;
long unsigned int timer = 0;
long unsigned int timer_01 = 0;
boolean check = false;

void setup() {
  pinMode(LATCH_REG, OUTPUT);
  pinMode(CLK_REG, OUTPUT);
  pinMode(DATA_REG, OUTPUT);
  pinMode(AM_OUT, INPUT);
  oled.init(OLED128x64, 800);
  oled.setContrast(1);  // ярк ость 0-255
  oled.clear();
  oled.home();
  oled.scale1X();
  oled.inverse(0);
  irrecv.enableIRIn();
  print_menu(pointer, first);
 }
ISR(TIMER1_A) {  // пишем  в сериал
 
}
void loop() {
  
  if ( irrecv.decode( &results )) {
    switch (results.value) {
      
      case 0xE318261B:      //1
        incoming[counter_inc] = 1;
        counter_inc++;
        oled.print(1);
        break;
        
      case 0x511DBB:        //2
        incoming[counter_inc] = 2;
        counter_inc++;
        oled.print(2);
        break;
        
      case 0xEE886D7F:      //3
        incoming[counter_inc] = 3;
        counter_inc++;
        oled.print(3);
        break;
        
      case 0x52A3D41F:      //4
        incoming[counter_inc] = 4;
        counter_inc++;
        oled.print(4);
        break;
        
      case 0xD7E84B1B:      //5
        incoming[counter_inc] = 5;
        counter_inc++;
        oled.print(5);
        break;
        
      case 0x20FE4DBB:      //6
        incoming[counter_inc] = 6;
        counter_inc++;
        oled.print(6);
        break;
        
      case 0xF076C13B:      //7
        incoming[counter_inc] = 7;
        counter_inc++;
        oled.print(7);
        break;
        
      case 0xA3C8EDDB:      //8
        incoming[counter_inc] = 8;
        counter_inc++;
        oled.print(8);
        break;
        
      case 0xE5CFBD7F:      //9
        incoming[counter_inc] = 9;
        counter_inc++;
        oled.print(9);
        break;
        
      case 0x97483BFB:      //0
        incoming[counter_inc] = 0;
        counter_inc++;
        oled.print(0);
        break;
      case 0xC101E57B:      //*

        break;
      case 0xF0C41643:      //#
        counter_inc = 0;
        oled.setCursor(5, 2);
        oled.print("                ");
        oled.setCursor(5, 2);
        break;
      case 0x3D9AE3F7:                    //up
        if (pointer > 0) {
          if (pointer == first)first--;
          pointer--;
          print_menu(pointer, first);
        }
        break;
      case 0x1BC0157B:                    //dawn
        if ( pointer != MAX_MENU - 1) {
          if (first + 7 == pointer)first++;
          pointer++;
          print_menu(pointer, first);
        }
        break;
      case 0x8C22657B:
        break;//left
        
      case 0x449E79F:         //right
         break;
         
      case 0x488F3CBB:        //ok

  
        break;

    }

    irrecv.resume(); // принимаем следующую команду
  }
}

void print_menu(uint8_t p, uint8_t f) {
  for (uint8_t i = 0; i < 8; i++) {
    char pa[20];
    strcpy_P(pa, pgm_read_word(&(arr[i + f])));
    oled.setCursor(0, i);
    oled.print("                    ");
    oled.setCursor(0, i);
    if (i + f == p)oled.inverse(1);
    oled.print(pa);
    oled.setCursor(15, i);
    oled.print(num_arr[(i + f)], 1);
    if (i + f == p)oled.inverse(0);
  }
}

void refresh_data() {
  for (uint8_t i = 0; i < 8; i++) {
    oled.setCursor(15, i);
    oled.print("     ");
    oled.setCursor(15, i);
    if (i + first == pointer)oled.inverse(1);
    oled.print(num_arr[i + first], 1);
    if (i + first == pointer)oled.inverse(0);
  }
  oled.setCursor(10, 1);
  oled.print(timer_01);
}

void sens_circle() {
  for (uint8_t i = 0; i < 16; i++) {
    digitalWrite(LATCH_REG, LOW);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, i);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, i);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, i);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, i);
    digitalWrite(LATCH_REG, LOW);
    data_arr[i] = avarage(A0);
    data_arr[i + 32] = avarage(A1);
    data_arr[i + 63] = avarage(A2);
    data_arr[i + 94] = avarage(A3);
  }
  uint8_t j = 0;
  for (uint8_t i = 0; i < 16; i++) {
    j = (i << 4) + 16;
    digitalWrite(LATCH_REG, LOW);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    digitalWrite(LATCH_REG, LOW);
    data_arr[i + 16] = avarage(A0);
    data_arr[i + 47] = avarage(A1);
    data_arr[i + 78] = avarage(A2);
  }
}
uint8_t avarage(uint8_t a) {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 10; i++)sum += analogRead(a);
  a = sum / 10;
  return a;
}


/* E318261B 1
   511DBB   2
   EE886D7F 3
   52A3D41F 4
   D7E84B1B 5
   20FE4DBB 6
   F076C13B 7
   A3C8EDDB 8
   E5CFBD7F 9
   97483BFB 0
   C101E57B
   F0C41643 #
   3D9AE3F7 up
   1BC0157B dawn
   8C22657B left
   449E79F  right
   488F3CBB ok
*/
