#include <GyverOLED.h>
#include <SoftwareSerial.h>

#define MINUTE 60000
const String names[8] = {"CM1810  ","CM7209  ","ШКИ-1   ","ШКИ-2   ","ШКЗ     ","НИ-04-01","НИ-04-02","НИ-04-03"};  
GyverOLED screen;
SoftwareSerial Serial_CM1810(10,11); //Rx Tx
SoftwareSerial Serial_CM2709(A0,A1);
int arr[8] ={0,0,0,0,0,0,0,0}, arr_buffer[8];
volatile uint32_t timer[8], com_timer, test_timer = 0; 
void COM_port();
void screen_update();
int minute = 60000;
volatile boolean state_CM1810 = false, state_CM2709 = false;

void setup(){
  Serial.begin(9600);
  while(!Serial){}
  Serial_CM1810.begin(9600);
  Serial_CM2709.begin(9600);
  screen.init(OLED128x64);
  screen.clear();
  screen.home();
  for (uint8_t i = 0; i < 6; i++)pinMode(i+4,INPUT);
  screen_update();
  }

void loop (){
  
  if (Serial_CM1810.available()) {
      String inc_str = Serial_CM1810.readString();
      timer[0] = millis();
     }
   
  if (Serial_CM2709.available() > 0) {
      String inc_str = Serial_CM2709.readString();
      for (uint8_t i = 0; i < inc_str.length(); i++){
        byte bt_in = inc_str[i];
        if (bt_in != 0) timer[1] = millis();} 
       }

  for(uint8_t i = 0; i < 8; i++){
    if (i != 0 && i != 1)
      if(digitalRead(i+2)) 
        timer[i] = millis();
    if(millis() - timer[i] >= MINUTE) arr_buffer[i] = 1;
      else arr_buffer[i] = 2;
    if (arr[i] != arr_buffer[i]){
      arr[i] = arr_buffer[i];
      }
  }
 
  if(millis() - com_timer >= 1000){
    screen_update();
    com_timer = millis();
    COM_port();
   }
}


void screen_update()
{
 screen.home();
 String stat;
 for (uint8_t i = 0; i<8; i++){
  if (arr[i] == 2)  stat = " +"; 
    else if(arr[i] == 1) stat = " -";
            else stat = " 0";
  screen.println(names[i]+stat);
  }

  }
void COM_port(){
  
    for (uint8_t i = 0; i < 8; i++)Serial.print(arr[i]);
    screen.println();
   
  }
  
