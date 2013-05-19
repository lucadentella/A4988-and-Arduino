#include <LiquidCrystal.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define PIN_STEP  2
#define PIN_DIR   3
#define FORWARD   HIGH
#define BACKWARD  LOW

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int actual_direction;
int step_count;

void setup() {
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Step controller");
  
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  
  actual_direction = FORWARD;
  digitalWrite(PIN_DIR, actual_direction);
  step_count = 0;

  update_lcd();
}

void loop() {
  
  int button = read_buttons();
  if(button == btnLEFT || button == btnRIGHT) {
    
    if((actual_direction == FORWARD && button == btnLEFT) ||
      (actual_direction == BACKWARD && button == btnRIGHT)) change_direction();
    make_step();
    update_lcd();
    delay(200);
  }
}

void update_lcd() {
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  lcd.setCursor(7, 1);
  lcd.print("--");
  
  if(actual_direction == FORWARD) {
    for(int i = 0; i < step_count; i++) lcd.print("-");
    lcd.print(">");
  } else {
    for(int i = 0; i < step_count; i++) {
      lcd.setCursor(6 - i, 1);
      lcd.print("-");
    }
    lcd.setCursor(7 - step_count - 1, 1);
    lcd.print("<");    
  }
  
  
}

void make_step() {
  
  digitalWrite(PIN_STEP, HIGH);
  digitalWrite(PIN_STEP, LOW);
  step_count++;
  if(step_count == 7) step_count = 0;
}

void change_direction() {
  
  if(actual_direction == FORWARD) actual_direction = BACKWARD;
  else actual_direction = FORWARD;
  
  digitalWrite(PIN_DIR, actual_direction);
  step_count = -1;
}

int read_buttons() {
  
 int adc_key_in = analogRead(0);
 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
}
