#include <HX711.h>                //Библиотека АЦП подходит и для НХ710с
#include <TM1620.h>               //Библиотека для драйвера дисплея
#include <avr/eeprom.h>           //Библиотека для работы с памятью

#define CLK 12                             //CLK микросхемы ТМ1620 18 контакт
#define DATA 13                           //DATA микросхемы ТМ1620 19 контакт
#define STB 11                            //STB микросхемы ТМ1620 20 контакт
#define NUMBS 6                           // Количество сегментов, включая дополнительные светодиоды
#define BRIGHTNESS 3                      // Яркость 0-7

#define BUTTON_TARE A0                    //Кнопка тара 2 Вольта
#define POWER_SUP 6                       //Поддержка питания, подключена через резистор 10 кОм
#define BUTTON_SCALE_UP 5                 //Увеличение шкалы. Чем больше шкала, тем меньший вес. Подтяжка на землю
#define BUTTON_SCALE_DOWN 4               //Уменьшение шкалы. Чем больше шкала, тем меньший вес. Подтяжка на землю

#define SENS_DATA 9                       //DATA АЦП НХ710с 6 контакт
#define SENS_CLK 10                       //CLK АЦП НХ710с 5 контакт
#define COF 0.035274                      //Коэффициент для перевода из фунтов в граммы


HX711 scale;                                                //создаем обьекты весов и драйвера индикатора
TM1620 tm1620(DATA, CLK, STB, NUMBS, true, BRIGHTNESS);     
int sample = 0;                                            //тут будем хранить последнее целочисленное значение кг 
int last = 0;                                             //тут будем хранить предпоследнее целочисленное значение целых кг
int timer = 0;                                            // счетчик таймера
float cof_scale = 35.f;                                    // коэффициэнт шкалы
float gramms = 0.0;                                        //тут храним вес


void setup() {
  pinMode(POWER_SUP,OUTPUT);                                // Инициалзиция и включение поддержки питания
  digitalWrite(POWER_SUP,HIGH);
  
  //eeprom_write_float(1,cof_scale);                        //если EEPROM чист -> убрать коммент (читай ниже)
  pinMode(LED_BUILTIN, OUTPUT);                             //и закомментить эту строчку, после запуска ардуино скопируй ЕЕПРОМ и верни как было
  cof_scale = eeprom_read_float(1);                         //читает ЕЕПРОМ и пишет в переменную
  pinMode(BUTTON_SCALE_UP,INPUT_PULLUP);                    // Инициализация кнопок увеличения/уменьшения коэффициента шкалы
  pinMode(BUTTON_SCALE_DOWN,INPUT_PULLUP);
  digitalWrite(LED_BUILTIN,LOW);                            //выключаем встроенный светодиод
  pinMode(BUTTON_TARE,INPUT);                               // Инициализация кнопки тара
  scale.begin(SENS_DATA, SENS_CLK);                         //Инициализация АЦП НХ710с, его перезагрузка, для очистки реестра, 
  reboot();                                                 //установка коэффициента шкалы и тарирование
  scale.set_scale(cof_scale);
  scale.tare();
} 

void display_print(int a, int b, int  c, int d){            //Драйвер вывода на дисплей и отключение неиспользуемых нулей
   
  if (a > 0)tm1620.setDisplayDigit(a,5,false);
  if (a+b>0)tm1620.setDisplayDigit(b,2,false);
  tm1620.setDisplayDigit(c,3,true);
  tm1620.setDisplayDigit(d,4,false);
  delay(500);
  }
void reboot(){                                                //Перезагрузка АЦП
    scale.power_down();
    delay(500);
    scale.power_up();
  }
  
void def(float in){                                           //Разбиение числа по розрядам, включение минуса, 
  int var, a,b,c,d;                                           //отправка на вывод на дисплей
    if (in < 0 ) tm1620.setSegments(8,0);
    else  tm1620.setSegments(0,0);
    in *= 10;
    var = (int) in;
    var = abs(var);
    a = var / 1000;
    b = (var % 1000)/100;
    c = (var % 100)/10;
    d = (var % 10);
    display_print(a,b,c,d);
  }
    
void loop()
{
       
      float f = analogRead(BUTTON_TARE);                              //Реализация кнопки TARE(есть ли на выводе 2 Вольта)
      f = (f/1024)*50;                                                
    if (f >=20){                                                      
      tm1620.clearDisplay();
      tm1620.setSegments(32,1);
      display_print(0,0,0,0);
      scale.tare();
      tm1620.setSegments(64,1);
      delay(1000);
      tm1620.setSegments(0,1);
      }
    if (digitalRead(BUTTON_SCALE_UP) == LOW){                            //Реализация кнопки шкала +, с записью в ЕЕПРОМ
      cof_scale = cof_scale + 0.1;
      cof_scale = constrain(cof_scale, 10.0, 100.0);
      scale.set_scale(cof_scale);
      eeprom_write_float(1,cof_scale);
      tm1620.clearDisplay();
      def(cof_scale*10);                                                 //Вывод коэффициента шкалы после с учетом изменений
      }
     if (digitalRead(BUTTON_SCALE_DOWN) == LOW){                          //Реализация кнопки шкала - с записью в ЕЕПРОМ
      cof_scale = cof_scale - 0.01;
      cof_scale = constrain(cof_scale, 10.0, 100.0);
      scale.set_scale(cof_scale);
      eeprom_write_float(1,cof_scale);
      tm1620.clearDisplay();
     def(cof_scale*10);                                                 //Вывод коэффициента шкалы после с учетом изменений
      }
    gramms = scale.get_units(10)*COF;                                     //Читаем данные с АЦП
    gramms = constrain(gramms, -999, 1999);                               
    sample = (int)gramms;
    if (sample == last){                                                  //Логика таймера простоя
      delay(500);
      timer++;
      last = sample;
    }
      else {
        timer = 0;
        tm1620.setSegments(0,1);
        last = sample;
      }
    if (timer >= 100) digitalWrite(POWER_SUP,LOW);                        //Выключаем по таймеру
    if (gramms >= 1000) {                                                 //Вывод больше тонны, отдельно для облегчения программы
      tm1620.clearDisplay();
      tm1620.setSegments(0,1);
      tm1620.setDisplayDigit(1,0,false);
      def((gramms - 1000));
    }
    else if (sample == 0){                                                //Значение равно нулю
      tm1620.clearDisplay();
      tm1620.setSegments(32,1);                                           //Зажигаем светодиод зеро
      display_print(0,0,0,0);
    }
        else {                                                          
          tm1620.clearDisplay();        
          if (timer != 0) tm1620.setSegments(4,1);                     //Зажигаем светодиод стабильности
          def(gramms);                                                 //Вывод на дисплей
        } 
} 
