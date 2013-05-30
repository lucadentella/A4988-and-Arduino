#include <LiquidCrystal.h>
#include <TimerOne.h>

// buttons code 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// directions
#define FORWARD   HIGH
#define BACKWARD  LOW

// debounce time (milliseconds)
#define DEBOUNCE_TIME  200

// PINs for Pololu controller
#define PIN_STEP  2
#define PIN_DIR   3

// lookup table speed - ticks (interrupts)
const int speed_ticks[] = {-1, 600, 300, 200, 150, 120, 100, 86, 75, 67, 60, 55, 50, 46, 43};

// global variables
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int actual_speed;
int actual_direction;

int ticks;
int tick_count;

int button;
boolean debounce;
int previous_time;

// custom LCD square symbol for progress bar
byte square_symbol[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

// string constants
char forward_arrow[] = "-->";
char backward_arrow[] = "<--";

void setup() {

  // init the timer1, interrupt every 0.1ms
  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr);
  
  // init LCD and custom symbol  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.createChar(0, square_symbol);
  
  // pins direction
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  
  // initial values
  actual_speed = 0;
  actual_direction = FORWARD;
  tick_count = 0;
  ticks = -1;
  debounce = false;

  digitalWrite(PIN_DIR, actual_direction);  
  updateLCD();
}
  
void loop() {
  
  // check if debounce active
  if(debounce) {
    button = btnNONE;
    if(millis() > previous_time + DEBOUNCE_TIME) debounce = false;
  } else button = read_buttons();
  
  // if a button is pressed, start debounce time
  if(button != btnNONE) {
    
    previous_time = millis();
    debounce = true;  
  }
    
  // check which button was pressed
  switch(button) {
    
    case btnUP:
      increase_speed();
      break;
    case btnDOWN:
      decrease_speed();
      break;
    case btnLEFT:
      change_direction(BACKWARD);
      break;
    case btnRIGHT:
      change_direction(FORWARD);
      break;
    case btnSELECT:
      emergency_stop();
      break;
  }
  
  // finally update the LCD
  updateLCD();
}

// increase speed if it's below the max (70)
void increase_speed() {
  
  if(actual_speed < 70) {
    actual_speed += 5;
    tick_count = 0;
    ticks = speed_ticks[actual_speed / 5];
  }
}

// decrease speed if it's above the min (0)
void decrease_speed() {
  
  if(actual_speed > 0) {
    actual_speed -= 5;
    tick_count = 0;
    ticks = speed_ticks[actual_speed / 5];
  }
}

// change direction if needed
void change_direction(int new_direction) {
  
  if(actual_direction != new_direction) {
    actual_direction = new_direction;
    digitalWrite(PIN_DIR, actual_direction);
  }
}

// emergency stop: speed 0
void emergency_stop() {
  actual_speed = 0;
  tick_count = 0;
  ticks = speed_ticks[actual_speed / 5];
}

// update LCD
void updateLCD() {
  
  // print first line:
  // Speed: xxxRPM --> (or <--)
  lcd.setCursor(0,0);
  lcd.print("Speed: ");
  lcd.print(actual_speed);
  lcd.print("RPM ");

  lcd.setCursor(13,0);
  if(actual_direction == FORWARD) lcd.print(forward_arrow);
  else lcd.print(backward_arrow);
  
  // print second line:
  // progress bar [#####         ]
  // 15 speed steps: 0 - 5 - 10 - ... - 70
  lcd.setCursor(0,1);
  lcd.print("[");
  
  for(int i = 1; i <= 14; i++) {
    
    if(actual_speed > (5 * i) - 1) lcd.write(byte(0));
    else lcd.print(" ");
  }
  
  lcd.print("]");
}

// timer1 interrupt function
void timerIsr() {

  if(actual_speed == 0) return;
  
  tick_count++;
  
  if(tick_count == ticks) {  
    
    // make a step
    digitalWrite(PIN_STEP, HIGH);
    digitalWrite(PIN_STEP, LOW);
    
    tick_count = 0;
  }
}

// read buttons connected to a single analog pin
int read_buttons() {
  
 int adc_key_in = analogRead(0);
 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
}