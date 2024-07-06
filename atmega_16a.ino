#include <LiquidCrystal.h>
bool f_35khz = true; 
bool f_50hz = true;
unsigned long prievious_time_35KHz = micros();
unsigned long prievious_time_50Hz = millis();
unsigned long prievious_display_update_800ms = millis();
unsigned long prievious_display_volt_update_500ms = millis();
unsigned long prievious_switch_delay_2000ms = millis();
long time_35khz = 14;
long time_50hz = 10;
int A0_input;
int display_switch = 14;
int display_supply = 15;
int invertet_relay = A4;
int inverter_switch = A5;
int usb_mosfet = A3;
int usb_mosfet_switch = A6;
int charging_sen = A7;

bool display_bool = true;
bool inverter_bool = false;
bool usb_bool = false;
const int rs = 13, en = 12, d4 = 3, d5 = 2, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  DDRC = B11000000;
  pinMode(A0, INPUT);
  //DDRB = B00110000;
  pinMode(display_switch, INPUT);
  pinMode(charging_sen, INPUT);
  pinMode(inverter_switch, INPUT);
  pinMode(usb_mosfet_switch, INPUT);
  pinMode(display_supply, OUTPUT);
  pinMode(invertet_relay, OUTPUT);
  pinMode(usb_mosfet, OUTPUT);
  digitalWrite(display_supply, display_bool);
  lcd.begin(16, 4);
  lcd.print("hello, Sourav");
  lcd.setCursor(0, 3);
  lcd.print("    Inverter OFF 0Hz");
}
void loop() {
  unsigned long current_time_35KHz = micros();
  unsigned long current_time_50Hz = millis();
  unsigned long current_display_update_800ms = millis();
  unsigned long current_display_volt_update_500ms = millis();
  unsigned long current_switch_delay_2000ms = millis();
  if(inverter_bool){
    if(current_time_35KHz - prievious_time_35KHz > time_35khz){
      if(f_35khz)
        PORTC = B10000000;
      else
        PORTC = B01000000;
      f_35khz = !f_35khz;
      prievious_time_35KHz = current_time_35KHz;
    }
  }
  if(current_time_50Hz - prievious_time_50Hz > time_50hz){
    if(f_50hz)
      PORTB = B00100000;
    else
      PORTB = B00010000;
    f_50hz = !f_50hz;
    prievious_time_50Hz = current_time_50Hz;
  }
  if(current_display_update_800ms - prievious_display_update_800ms> 800){
    A0_input = analogRead(A0);
    String battery_parsentage = String(map(A0_input, 460, 524, 000, 100));
    if(A0_input>= 531){
      lcd.setCursor(0, 1);
      lcd.print("Charge Full  ");
    }
    else if(A0_input>= 527 && A0_input<= 530){
      lcd.setCursor(0, 1);
      lcd.print(battery_parsentage+"%                ");
    }
    else if(A0_input< 460){
      lcd.setCursor(0, 1);
      lcd.print("battery too Low!!!        ");
    }
    else{
      lcd.setCursor(0, 1);
      lcd.print(battery_parsentage+"%                ");
    }
    prievious_display_update_800ms = current_display_update_800ms;
  }
  if(current_display_volt_update_500ms - prievious_display_volt_update_500ms>500){
    String volt = String(map_f(A0_input, 0, 642, 0.0, 15.46));
    lcd.setCursor(2, 2);
    lcd.print("  "+volt+"V  ");
    prievious_display_volt_update_500ms = current_display_volt_update_500ms;
  }
  if(current_switch_delay_2000ms - prievious_switch_delay_2000ms>2000){
    if(digitalRead(display_switch)){
      display_bool = !display_bool;
      digitalWrite(display_supply, display_bool);
      if(display_bool){
        lcd.display();
      }
      else{
        lcd.noDisplay();
      }
    }
    else if(digitalRead(usb_mosfet_switch)){
      usb_bool = !usb_bool;
      digitalWrite(usb_mosfet, usb_bool);
    }
    else if(digitalRead(inverter_switch)){
      inverter_bool = !inverter_bool;
      digitalWrite(invertet_relay, inverter_bool);
      if(!inverter_bool){
        PORTC = B00000000;
        lcd.setCursor(0, 3);
        lcd.print("    Inverter OFF 0Hz  ");
      }
      else{
        lcd.setCursor(0, 3);
        lcd.print("    Inverter ON 33KHz");
      }
    }
    prievious_switch_delay_2000ms = current_switch_delay_2000ms;
  }
}
float map_f(float input, float input_min, float input_max, float out_min, float out_max){
  return (input - input_min) * (out_max - out_min) / (input_max - input_min) + out_min;
}
