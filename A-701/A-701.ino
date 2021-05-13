#include <OneWire.h>
#include <GyverOLED.h>
#include "IRremote.h"

#define LATCH_REG 6
#define DATA_REG 4
#define CLK_REG 5

#define NUM_SENSORS 110
#define NUM_MAIN_MENU 4
#define MENU "   МЕНЮ"

IRrecv irrecv(2);
decode_results results;
GyverOLED oled;
const char main_menu_01 [] PROGMEM = "  Вывод данных.   ";
const char main_menu_02 [] PROGMEM = "  Уставки.        ";
const char main_menu_03 [] PROGMEM = "  Набор точек.    ";
const char main_menu_04 [] PROGMEM = "  Вывод графика.  ";
uint8_t danger[110][2];
const char* const menu_arr[NUM_MAIN_MENU] PROGMEM = {main_menu_01, main_menu_02, main_menu_03, main_menu_04};

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
const char arr_16[] PROGMEM = "Temp P:";
const char* const arr[NUM_SENSORS] PROGMEM = {arr_01, arr_02, arr_03, arr_04, arr_05,
                                              arr_06, arr_07, arr_08, arr_09, arr_10,
                                              arr_11, arr_12, arr_13, arr_14, arr_15, arr_16
                                             };
float num_arr[NUM_SENSORS];

uint8_t pointer = 0, first = 0, lvl_menu = 0, sub_pointer = 0, sub_first = 0;
uint8_t incoming [3] = {0, 0, 0}, sens_monitor[8] = {0,0,0,0,0,0,0,0};
uint8_t counter_inc = 0, sub_menu = 0;
long unsigned int timer = 0;
long unsigned int timer_01 = 0;
boolean check = false, stage = false, num_lock = true;

void setup() {
  for (uint8_t i = 0; i < NUM_SENSORS; i++) num_arr[i] = i;
  pinMode(LATCH_REG, OUTPUT);
  pinMode(CLK_REG, OUTPUT);
  pinMode(DATA_REG, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  oled.init(OLED128x64, 800);
  oled.setContrast(1);
  oled.clear();
  oled.home();
  oled.scale1X();
  oled.inverse(0);
  irrecv.enableIRIn();
  print_menu(pointer, first, lvl_menu);
  for (uint8_t i = 0; i < 110; i ++) {
    danger[i][0] = 75;
    danger[i][1] = 100;
  }
}

void loop() {

  if ( irrecv.decode( &results )) {
    switch (results.value) {

      case 0xE318261B:      //1
        if (num_lock)press_num(1);
        break;

      case 0x511DBB:        //2
        if (num_lock)press_num(2);
        break;

      case 0xEE886D7F:      //3
        if (num_lock)press_num(3);
        break;

      case 0x52A3D41F:      //4
        if (num_lock)press_num(4);
        break;

      case 0xD7E84B1B:      //5
        if (num_lock)press_num(5);
        break;

      case 0x20FE4DBB:      //6
        if (num_lock)press_num(6);
        break;

      case 0xF076C13B:      //7
        if (num_lock)press_num(7);
        break;

      case 0xA3C8EDDB:      //8
        if (num_lock)press_num(8);
        break;

      case 0xE5CFBD7F:      //9
        if (num_lock)press_num(9);
        break;

      case 0x97483BFB:      //0
        if (num_lock)press_num(0);
        break;

      case 0xC101E57B:      //*
        break;

      case 0xF0C41643:      //#
        if (lvl_menu != 0)lvl_menu = 0;
        pointer = 0;
        first = 0;
        stage = false;
        counter_inc = 0;
        print_menu(pointer, first, lvl_menu);
        break;

      case 0x3D9AE3F7:                    //up
        if (pointer > 0) {
          if (pointer == first)first--;
          pointer--;
          print_menu(pointer, first, lvl_menu);
        }
        break;

      case 0x1BC0157B:                    //dawn
        pressed_dawn(lvl_menu);
        break;

      case 0x8C22657B:        //left
        break;

      case 0x449E79F:         //right
        break;

      case 0x488F3CBB:               //ok
        switch (lvl_menu) {
          case 0:
            lvl_menu = 1 + pointer;
            first = 0;
            pointer = 0;
            print_menu(pointer, first, lvl_menu);
            break;
          case 1:
            break;
          case 2:
            if (counter_inc == 0) set_danger();
            else if (!stage) {
              danger[pointer][0] = read_num();
              stage = true;
              set_danger();
            }
            else {
              danger[pointer][1] = read_num();
              stage = false;
              num_lock = false;
              if (danger[pointer][1] < danger[pointer][0]) danger[pointer][1] = danger[pointer][0];
              print_menu(pointer, first, lvl_menu);
            }
            break;
          case 3:
          show_monitor();
            break;
          case 4:
            break;
        }
        break;
    }
    irrecv.resume();
  }
}

void show_monitor(){
  }
  
uint8_t read_num() {
  uint8_t ret_num = 0;
  for (uint8_t i = 0; i < counter_inc; i++ ) {
    ret_num = ret_num * 10 + incoming[i];
  }
  counter_inc = 0;
  return ret_num;
}
void set_danger() {
  num_lock = true;
  oled.clear();
  oled.home();
  oled.print("Установка точки: ");
  oled.println(pointer);
  if (!stage)oled.println("Устанви ДоКрит: ");
  else oled.println("Установи Крит.: ");
}

void press_num(uint8_t in) {
  if (counter_inc < 3) {
    incoming[counter_inc] = in;
    counter_inc++;
    oled.print(in);
  }
}

void pressed_dawn(uint8_t lvl) {
  switch (lvl) {
    case 0:
      if ( pointer != NUM_MAIN_MENU - 1) {
        if (first + 5 == pointer)first++;
        pointer++;
        print_menu(pointer, first, lvl_menu);
      }
      break;
    case 1:
      if ( pointer != NUM_SENSORS - 1) {
        if (first + 7 == pointer)first++;
        pointer++;
        print_menu(pointer, first, lvl_menu);
      }
      break;
    case 2: if ( pointer != NUM_SENSORS - 1) {
        if (first + 6 == pointer)first++;
        pointer++;
        print_menu(pointer, first, lvl_menu);
      }
      break;
  }
}
void print_menu(uint8_t p, uint8_t f, uint8_t lvl) {
  switch (lvl) {
    case 0 :
      oled.clear();
      oled.setCursor(0, 0);
      oled.scale2X();
      oled.print(MENU);
      oled.scale1X();
      for (uint8_t i = 0; i < NUM_MAIN_MENU; i++) {
        char pa[12];
        oled.setCursor(0, 2 + i);
        if (i == p)oled.inverse(1);
        strcpy_P(pa, pgm_read_word(&(menu_arr[i])));
        oled.print(pa);
        if (i == p)oled.inverse(0);
      }
      break;
    case 1:
      for (uint8_t i = 0; i < 8; i++) {
        oled.setCursor(0, i);
        oled.print("                    ");
        oled.setCursor(0, i);
        if (i + f == p)oled.inverse(1);
        oled.print("Точка: ");
        if (i + f < 100)oled.print(" ");
        if (i + f < 10)oled.print(" ");
        oled.print(i + f);
        oled.setCursor(12, i);
        oled.print(num_arr[(i + f)], 1);
        if (i + f == p)oled.inverse(0);

        //        char pa[20];
        //        strcpy_P(pa, pgm_read_word(&(arr[i + f])));
        //        oled.setCursor(0, i);
        //        oled.print("                    ");
        //        oled.setCursor(0, i);
        //        if (i + f == p)oled.inverse(1);
        //        oled.print(pa);
        //        oled.setCursor(15, i);
        //        oled.print(num_arr[(i + f)], 1);
        //        if (i + f == p)oled.inverse(0);
      }
      break;
    case 2:
      oled.clear();
      oled.home();
      oled.print("Точка  ДоКрит  Крит");
      for (uint8_t i = 0; i < 7; i++) {
        oled.setCursor(0, i + 1);
        if (i + f == p)oled.inverse(1);
        if (i + f < 100)oled.print(" ");
        if (i + f < 10)oled.print(" ");
        oled.print(i + f);
        oled.setCursor(7, i + 1);
        oled.print(danger[i + f][0]);
        oled.setCursor(15, i + 1);
        oled.print(danger[i + f][1]);
        if (i + f == p)oled.inverse(0);


      }
      break;
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
    num_arr[i] = avarage(A0);
    num_arr[i + 32] = avarage(A1);
    num_arr[i + 63] = avarage(A2);
    num_arr[i + 94] = avarage(A3);
  }
  uint8_t j = 0;
  for (uint8_t i = 0; i < 16; i++) {
    j = (i << 4) + 16;
    digitalWrite(LATCH_REG, LOW);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    shiftOut(DATA_REG, CLK_REG, MSBFIRST, j);
    digitalWrite(LATCH_REG, LOW);
    num_arr[i + 16] = avarage(A0);
    num_arr[i + 47] = avarage(A1);
    num_arr[i + 78] = avarage(A2);
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
   C101E57B *
   F0C41643 #
   3D9AE3F7 up
   1BC0157B dawn
   8C22657B left
   449E79F  right
   488F3CBB ok
*/
