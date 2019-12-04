#include <LiquidCrystal.h>
#include <RunningAverage.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

/*-----------------SETUP--PINS-----------------------------*/
const int ROWERINPUT = 2; // the input pin where the waterrower sensor is connected
const int BUTTONSPIN = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //lcd shield


/*-----------------CONSTANTS-------------------------------*/

const float Ratio = 4.8; // from old script 4.8; meters per rpm = circumference of rotor (D=34cm) -> 1,068m -> Ratio = 0.936 ; WaterRower 7,3 m/St. -> Ratio: 3.156

RunningAverage stm_RA(60); // size of array for strokes/min
RunningAverage mps_RA(5); // size of array for meters/second -> emulates momentum of boat

/*-----------------VARIABLES-----------------------------*/

float start;
float rounds;
volatile unsigned long click_time = 0;
volatile unsigned long last_click_time = 0;
volatile unsigned long old_split = 0;
volatile unsigned long split_time = 0;
volatile unsigned long start_split;
long debouncing_time = 15; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;
volatile int clicks = 0;
volatile int clicks_old = 0;
volatile float data_output = 0;
volatile long rpm = 0;
volatile long old_rpm = 0;
volatile long stm = 0;
volatile int old_strokes = 0;
volatile long stmra = 0;
int accel = 0;
int puffer = 0;
volatile float Ms = 0;
volatile int meters = 0;
volatile int meters_old = 0;
//volatile int trigger = 0;
unsigned long timer1 = 0;
unsigned long timer2 = 0;
unsigned long timer3 = 0;
int stage = 0; // sets case for what parameter to be displayed
int rotation = 0;
int strokes = 0;
int trend = 0;

/*-----------------SETUP-----------------------------------*/
void setup() {
//  Serial.begin(9600);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:
  lcd.clear();
  lcd.home();
//  lcd.print("Starting...");
//  delay(1000);
//  lcd.clear();
//  lcd.setCursor(0, 1);
  lcd.print("WaterRino 1.1");
  delay(4000);
  stm_RA.clear(); // explicitly start clean
  mps_RA.clear(); // explicitly start clean
  pinMode(ROWERINPUT, INPUT_PULLUP);
  digitalWrite(ROWERINPUT, HIGH);
  //delay(1000);
  attachInterrupt(0, rowerdebounceinterrupt, FALLING);
  timer1 = millis();
  timer2 = millis();
  timer3 = millis();
}

void row_start() {
  lcd.clear();
  lcd.print("Ready");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print("Set");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.print(".");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("      ROW!!!     ");
  delay(500);
  lcd.clear();
  reset();
  rowing();
}

void variableParameter(){
    lcd.setCursor(9, 0);
    switch (stage) {
    case 1:
      lcd.print("Rot");
      data_output = clicks;
      break;
    case 2:
      lcd.print("Rpm");
      data_output = rpm;
    break;
    case 3:
        lcd.print("Trend ");
        if (trend == 0){
          lcd.print("c");
        }
        if (trend == 1){
          lcd.print("-");
        }
        if (trend == 2){
          lcd.print("+");
        }
      data_output = 1;
    break;
    case 4:
      lcd.print("St ");
      data_output = strokes;
    break;
    case 5:
      lcd.print("StA");
      data_output = stm;
    break;
    case 6:
      lcd.print("StM");
      data_output = stmra;
    break;
    default:
       lcd.print("Sp5");
       data_output = split_time;
    break;
    }
    if(data_output >= 10) {
      if(data_output >= 100) {
      lcd.print(" ");
      } else {
        lcd.print("  ");
      }
    } else {
    lcd.print("   ");
    }
    lcd.print(data_output);
}

void rowing() {
  while (read_LCD_buttons() != btnLEFT) {
    lcd.setCursor(0, 0);
    lcd.print("M ");
    if(meters >= 10) {
      if(meters >= 100) {
        if(meters >= 1000) {
          lcd.print(" ");          
        } else {
          lcd.print("  ");          
        }
      } else {
        lcd.print("   ");
      }
    } else {
    lcd.print("    ");
    }
    lcd.print(meters);
    // prints the variable parameter as selected by keypad
    variableParameter();
    // set time display
    displayTime();
    lcd.setCursor(9, 1);
    lcd.print("Ms ");
    lcd.print(Ms, 2);
    /* calculate meters/min*/
    if ((millis() - timer1) > 1000) {
      Ms = calcmetersmin();
      //Serial.println(Ms);
      timer1 = millis();
      //lcd.clear();
    }
    /*calculate split times every 500m*/
    if ((meters % 500) == 0 && meters > 0 ) {
      if ((millis() - timer2) > 1000) {
        split();
        lcd.clear();
      }
      timer2 = millis();
    }
    /*calculate moving average of strokes/min*/
    if ((millis() - timer3) > 1000) {
      calcstmra();
      timer3 = millis();
    }
    /* if select button is pressed reset */
    switch (read_LCD_buttons())
    {
      case btnSELECT:
        {
          reset();
          break;
        }
      case btnUP:
        {
          if (stage < 6){
            stage ++;
            delay(500);
            lcd.clear();
          }
          break;
        }
      case btnDOWN:
      {
        if (stage > 0){
          stage --;
          delay(500);
          lcd.clear();
        }
        break;
      }
    }

/*-------SIMULATE ROWING-------------*/
 //     if (random(0, 100) < 70) {
 //        rowerinterrupt();
 //        }
/*-------SIMULATE ROWING-------------*/

  }

}

/*-----debounce------*/
void rowerdebounceinterrupt() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    rowerinterrupt();
    last_micros = micros();
  }
}
/*------------------INTERRUPT-----------------------------------*/
void rowerinterrupt() {
  /*  if (trigger == 0) {
      reset();
      trigger = 1;
    }*/
  if (rotation == 0){ // prevents double-count due to switch activating twice on the magnet passing once
    clicks++;
    rotation = 1;
    calcrpm();
  } else {
    rotation = 0;
  }
  meters = clicks / Ratio;
}
/*------------------INTERRUPT END-----------------------------------*/

void calcrpm() {
  click_time = millis();
  rpm = (float)1000 / (click_time - last_click_time) * 60;
  last_click_time = click_time;
  accel = rpm - old_rpm;
  if ((accel > 5) && (puffer == 0)){ // > 5 to eliminate micro "acceleration" due to splashing water
    strokes ++;
    puffer = 5; // prevents counting two strokes due to still accelerating rotor
  }
  if (puffer > 0){
    puffer --;
  }
  old_rpm = rpm;
  stm = (strokes*60000)/(millis()-start);
}

//calculate and return meters per second
float calcmetersmin() {
  mps_RA.addValue((clicks - clicks_old) / Ratio);
  //float x = (clicks - clicks_old) / Ratio;
  float x = mps_RA.getAverage(); // use of floating average emulates the momentum of a boat
  clicks_old = clicks;
  return x; 
}

//calculate strokes/minute as rolling average
void calcstmra() {
  stm_RA.addValue(strokes-old_strokes);
  old_strokes = strokes;
  stmra = stm_RA.getAverage()*60;
  if (stmra > stm){
    trend = 2;
  }
  if (stmra < stm){
    trend = 1;
  }
  if (stmra == stm){
    trend = 0;
  }
}

//calculate 500m split time (in seconds)
void split() {
  start_split = millis();
  split_time = (start_split - old_split) / 1000;
  old_split = millis();
}

//reset function (set to select button)
void reset() {
  clicks = 0;
  clicks_old = 0;
  meters = 0;
  meters_old = 0;
  click_time = 0;
  old_split = 0;
  split_time = 0;
  Ms = 0;
  timer1 = 0;
  timer2 = 0;
  timer3 = 0;
  start = millis();
  old_split = millis();
  strokes = 0;
  stmra = 0;
  stm = 0;
  rpm = 0;
  stm_RA.clear();
  mps_RA.clear();
  trend = 0;
  lcd.clear();
}

int read_LCD_buttons()
{
  int adc_key_in = 0;
  adc_key_in = analogRead(BUTTONSPIN);      // read the value from the sensor
  delay(20); //switch debounce delay. Increase this delay if incorrect switch selections are returned.
  int k = (analogRead(BUTTONSPIN) - adc_key_in); //gives the button a slight range to allow for a little contact resistance noise
  if (5 < abs(k)) return btnNONE;  // double checks the keypress. If the two readings are not equal +/-k value after debounce delay, it tries again.
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;  // when all others fail, return this...
}

void displayTime()
{
  float h,m,s;
  unsigned long over;
  h=int((millis()-start)/3600000);
  over=long(millis()-start)%3600000;
  m=int(over/60000);
  over=over%60000;
  s=int(over/1000);
  lcd.setCursor (0,1); //go to start of 2nd line
  lcd.print("T ");
  if (m < 10){
    lcd.print("0");
  }
  lcd.print(m,0);
  lcd.print(":");
  if (s < 10){
    lcd.print("0");
  }
  lcd.print(s,0);
}

/*------------------LOOP-----------------------------------*/
void loop() {
  lcd.clear();
  lcd.home();
  lcd.print("Press Right to");
  lcd.setCursor(0, 1);
  lcd.print("Start");
  //while (analogRead (BUTTONSPIN) != 294) {
  while (read_LCD_buttons() != btnRIGHT) {
    //Serial.println(read_LCD_buttons());
  }
  row_start();
}
