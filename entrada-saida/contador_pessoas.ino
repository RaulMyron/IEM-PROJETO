/*
* Projeto: Contador de Pessoas
* 
* Este projeto tem como objetivo evitar aglomerações em ambientes como 
* supermercados e lojas para ajudar na luta contra o Covid-19
*/

// LCD Padrão
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// LCD I2C 
//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPin = 7;
int ledState = 0;
int period = 500; 
unsigned long ledTime = 0;

int ldrPin = A0; 
int ldrPin2 = A1;
int ldrValue = 0; // luminosidade lida pelo LDR
int ldrValue2 = 0; // luminosidade lida pelo LDR2
int trigger = 350; // valor de luminosidade que máximo para detectar que o laser foi interceptado

// o próximo passo é automatizar o valor "trigger"
// de acordo com a luz ambiente

int people = 0;
int maxPeople = 5;
bool hasPersonChanged = false;
bool hasReachedMax = false;
bool hadReachedMax = false;

String order = ""; // A ordem em que os lasers foram interceptados
String lastChar = ""; // o último charactere de "order" para comparação

void printLcd(); // Mensagem padrão impressa no Display
void reachedMax(); // Mensagem impressa quando a capacidade máxima de pessoas for atingida
void blinkLed();
void readSensors();
void calibrate(); // falta implementar

void setup() {
  // LCD Padrão
  lcd.begin(16, 2);

  // LCD I2C
  //lcd.init();
  //lcd.backlight();
  
  pinMode(ledPin,OUTPUT); 
  Serial.begin(9600);
  printLcd();
}

void loop() {
  // Lê os sensores
  readSensors();
  delay(50);

  // Entrada
  if(order.equals("1-2")){
    people++;
    order = "";
    lastChar = "";
    hasPersonChanged = true;
  }
  
  // Saída
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
    
    // caso queira que o display mostre o número de pessoas dentro
    // mesmo após atingir a capacidade máxima, comente o "printLCD()"
    // acima e tire o comentário do "printLCD()" abaixo
    
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

  
  // Caso ocorra uma interceptação acidental
  else if(order.length() == 2 && order.charAt(1) != '-'){
    order.remove(0, 1);
  }

  // Caso "order" tenha 3 caracteres e não configura uma entrada nem
  // uma saída, apaga "order" e "lastChar" e não muda o número de
  // pessoas presentes no momento
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
