/*
* Project: People Counter
* 
* The objective of this project is to avoid agglomerations in places 
* like Supermarkets and Stores to fight the spread of the Covid-19
*/

// Standard LCD
//#include <LiquidCrystal.h>
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// I2C LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPin = 7;
int ledState = 0;
int period = 500; 
unsigned long ledTime = 0;

int ldrPin = A0; 
int ldrPin2 = A1;
int ldrValue = 0; // luminosity read by the LDR
int ldrValue2 = 0; // luminosity read by the LDR2
int trigger = 350; // Threshold of maximum expected luminosity for when the laser is not hitting the sensor

// the next step is to automate the trigger value
// according to the ambient luminosity

int people = 0;
int maxPeople = 5;
bool hasPersonChanged = false;
bool hasReachedMax = false;
bool hadReachedMax = false;

String order = ""; // The order in which the lasers are being intercepted
String lastChar = ""; // last char of "order" for comparing purposes

void printLcd(); // Standard message print on Display
void reachedMax(); // Message print for when the max capacity is reached
void blinkLed();
void readSensors();
void calibrate(); // not implemented

void setup() {
  // Standard LCD
  //lcd.begin(16, 2);

  // I2C LCD
  lcd.init();
  lcd.backlight();
  
  pinMode(ledPin,OUTPUT); 
  Serial.begin(9600);
  printLcd();
}

void loop() {
  // Read the LDRs
  readSensors();
  delay(50);

  // Someone enters
  if(order.equals("1-2")){
    people++;
    order = "";
    lastChar = "";
    hasPersonChanged = true;
  }
  
  // Someone gets out
  else if(order.equals("2-1") && people > 0){
    people--;
    order = "";
    lastChar = "";
    hasPersonChanged = true;
  }

  if(hasPersonChanged){
    if (people > maxPeople) {
      hasReachedMax = true;
    } else {
      ledState = 0;
      digitalWrite(ledPin, ledState);
      hasReachedMax = false;
      printLcd();
    }
    hasPersonChanged = false; 
    
    // if you want the display to show the number of people inside
    // even after reaching the max capacity, comment the "printLcd()"
    // above and uncomment the one below
    
    //printLcd()
  }

 
  if(hasReachedMax){
    blinkLed();
    if(!hadReachedMax){
      reachedMax();
      hadReachedMax = true;
    }
  } else {
    hadReachedMax = false;
  }
}

void printLcd(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Maximo: ");
  lcd.print(maxPeople);
  lcd.setCursor(0, 1);
  lcd.print("Pessoas: ");
  lcd.print(people);
}

void reachedMax(){ 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Atencao!");
  lcd.setCursor(0,1);
  lcd.print("Limite Atingido!");
}

void blinkLed(){
  if (millis() >= ledTime){
    ledTime = millis() + period;
    ledState = 1 - ledState;
    digitalWrite(ledPin, ledState);
  }
}

void readSensors(){
  ldrValue = analogRead(ldrPin);
  ldrValue2 = analogRead(ldrPin2); // The LDR value is between 0 & 1023
  
  if (ldrValue >= trigger && ldrValue2 < trigger && lastChar != "1"){
    order += "1";
    lastChar = "1";
    Serial.println(order);
  }
  
  // when both lasers are intercepted
  else if(ldrValue >= trigger && ldrValue2 >= trigger && lastChar != "-"){
    order += "-";
    lastChar = "-";
    Serial.println(order);
  }
  
  else if(ldrValue2 >= trigger && ldrValue < trigger && lastChar != "2"){
    order += "2";
    lastChar = "2";
    Serial.println(order);
  }

  
  // If there are some accidental inteceptation
  else if(order.length() == 2 && order.charAt(1) != '-'){
    order.remove(0, 1);
  }

  // If "order" has 3 characters and it's not an entry nor 
  // an exit, erase "order" and "lastChar" and don't change 
  // the current number of people 
  else if(order.length() >= 3){
    if(order != "1-2" || order != "2-1"){
      order = "";
      lastChar = "";
    }
  }
}

void calibrate() {
  //desliga os lasers
  //leio os sensores
  //ligo os lasers
  //leio os sensores com os lasers
}
