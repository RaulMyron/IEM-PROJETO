#include <LiquidCrystal.h>

int cm = 0;
int motor = 13;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
  
}

void setup()
{
  Serial.begin(9600);
  pinMode(motor, OUTPUT);
  lcd.begin(16, 2);
  //lcd.setCursor(0, 0);
  //lcd.print("Lave suas maos");
  //lcd.setCursor(0,1);
  //lcd.print("Aproxime-as");
}

void loop()
{
  
  cm = 0.01723 * readUltrasonicDistance(8, 8);
  delay(1000);
  Serial.print(cm);
  Serial.println("cm");

  if(cm<=30){
 	lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lavando...");
    digitalWrite(motor, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Use mascara! :)");
    delay(5000);
    lcd.clear();
    lcd.print("Retire suas maos :)");
    digitalWrite(motor, LOW);
   	delay(2000);
  	}
  else{
    digitalWrite(motor, LOW);
    lcd.clear();
  	lcd.setCursor(0, 0);
  	lcd.print("Lave suas maos");
 	lcd.setCursor(0,1);
  	lcd.print("Aproxime-as");
    delay(500);
  }
}
