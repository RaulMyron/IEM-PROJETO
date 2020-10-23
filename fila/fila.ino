#include <LiquidCrystal.h>

float cm,duracao, alguem;

//definindo pinos da placa LCD LiquidCrystal lcd(<pino RS>, <pino enable>, <pino D4>, <pino D5>, <pino D6>, <pino D7>)
LiquidCrystal lcd(1, 2, 3, 4, 5, 6);

//Sensores
byte pinoTrans1 = 8;  //pino trig 1
byte pinoRec1 = 7; //pino echo 1
byte pinoTrans2 = 13; //pino trig 2
byte pinoRec2 = 12; //pino echo 2

//LED
int pinoR = 10;
int pinoG = 9;
int pinoB = 11;

void setup()
{
  //Display LCD / Colunas e Linhas
  lcd.begin(16,2);
  
  //LED
  pinMode(pinoR, OUTPUT);
  pinMode(pinoG, OUTPUT);
  pinMode(pinoB, OUTPUT);

  //SENSOR
  pinMode(pinoTrans1, OUTPUT);
  pinMode(pinoTrans2, OUTPUT);
  pinMode(pinoRec1, INPUT);
  pinMode(pinoRec2, INPUT);
  
  //iniciar a porta serial
  Serial.begin(9600);
  
  
}

void loop()
{
  delay(1000);
  lcd.clear();
  delay(500);
  cm = distancia(); //distancia em cm
  alguem = distancia1();
  if (alguem > 0){ //checa se há alguém a frente na fila
     //limpa a tela
    //acender o LED com base na distancia, mudando de cor.
    //distancia de 3m (distanciamento social! :))
    if(cm >0 && cm<=200) { //vermelho caso muito perto
      analogWrite(pinoR, 255);
      analogWrite(pinoG, 0);
      analogWrite(pinoB, 0);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("AFASTE-SE");
      lcd.setCursor(0,1);
      lcd.print("POR FAVOR");
      delay(500);
      
    }
    else if (cm >200 && cm <=300){ //amarelo caso se aproximando
      analogWrite(pinoR, 255);
      analogWrite(pinoG, 255);
      analogWrite(pinoB, 0);
      lcd.clear();
      lcd.print("AFASTE-SE");
      lcd.setCursor(0,1);
      lcd.print("POR FAVOR");
      delay(500);
    }
    else if (cm>300){ //azul caso longe
      analogWrite(pinoR, 0);
      analogWrite(pinoG, 0);
      analogWrite(pinoB, 255);
      lcd.clear();
      delay(500);
    }
    else{ //apagado em qualquer outro caso
      analogWrite(pinoR, 0); 
      analogWrite(pinoG, 0);
      analogWrite(pinoB, 0);
      lcd.clear();
      delay(500);
    }
    //printa no serial a distancia em cm
    Serial.print(cm);
    Serial.println(" cm");
    delay(500);
  }
  else{
    lcd.clear();
    analogWrite(pinoR, 0);
    analogWrite(pinoG, 0);
    analogWrite(pinoB, 0);
    lcd.setCursor(0,0);
    lcd.print("PROXIMO");
    delay(500);
  }
}
//funcao para calcular a distancia:
float distancia() {
  //limpar o pino/aguardar
  digitalWrite(pinoTrans2, LOW);
  delayMicroseconds(1);
  //pulso do ultrassom
  digitalWrite(pinoTrans2, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrans2, LOW);
  //calcula a duracao em ms para ida e volta do pulso
  duracao = pulseIn(pinoRec2, HIGH);
  //matematica basica xd
  float  calcDistancia = (duracao/2) * 0.0343;
  if (calcDistancia>=331){
    calcDistancia=0;
  }
  delay(500);
  return calcDistancia;
}
//funcao para checar se há alguém no outro lugar da fila
float distancia1(){
  //limpar o pino/aguardar
  digitalWrite(pinoTrans1, LOW);
  delayMicroseconds(1);
  //pulso do ultrassom
  digitalWrite(pinoTrans1, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrans1, LOW);
  //calcula a duracao em ms para ida e volta do pulso
  duracao = pulseIn(pinoRec1, HIGH);
  //matematica basica xd
  float  calcDistancia = (duracao/2) * 0.0343;
  if (calcDistancia<=331){
    calcDistancia=1;
  }
  else{
    calcDistancia=0;
  }
  delay(500);
  return calcDistancia;
}
