#include <LiquidCrystal.h>
//#include <LiquidCrystal_I2C.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//LiquidCrystal_I2C lcd(0x27, 16, 2);

// BlinkLed
int ledPin = 7; // Led no pino 7  
int ledState = 0;  // Esse led comeca a piscar quando ultrapassa a capacidade máxima
int period = 500; // tempo entre o led ligado e desligado
unsigned long long ledTime = 0; // variavel utilizado para a função blinkLed()

// Sensores
int ldrPin1 = A0; // LDR1 no pino analógico 8
int ldrPin2 = A1; 
int ldrValue1 = 0; // Valor que vai ser lido do LDR1
int ldrValue2 = 0; // Valor que vai ser lido do LDR2
int triggerValue1 = 350; // Valor para disparar o sensor -> Valores maiores disparam o sensor
int triggerValue2 = 350; // Valor impreciso 

// Calibragem
int buttonState = 0;
int buttonPin = 6;
unsigned long timePressed = 0;
unsigned long timePressedLimit = 0;
int numClicks = 0;
int ambientLuminosity1 = 0;
int ambientLuminosity2 = 0;
int laserLuminosity1 = 0;
int laserLuminosity2 = 0;

// Lógica de Entrada
int people = 0; // Quantidade de pessoas dentro
int maxPeople = 5; // Capacidade Máxima de pessoas
bool hasNumPersonChanged = false;
bool hasReachedMax = false;
bool hadPreviousReachedMax = false;
unsigned long long firstReachedMax_LCDPrintTime = 0;
bool firstReachedMax = false;
String order = ""; // string contendo ordem de ativação
String lastChar = ""; // último caractere da ordem para comparação

///Protótipos das Funções
void printLcd(); // imprime o padrão do lcd
void reachedMaxPrint(); // dispara quando o limite de pessoas for atingido
void blinkLed();
void readSensors(); // ler os valores dos 2 sensores LDR
void calibrate(); // falta implementar! vai automatizar o trigger
void buttonDetect(); // verifica se o usuario esta pressionando o botao
void printTimer();

void setup() {
  lcd.begin(16, 2);
  //lcd.init();
  //lcd.backlight();
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  printLcd();
}

void loop(){
  // verifica se o usuario quer calibrar os sensores
  buttonDetect();

  // verifica os sensores
  readSensors();
  delay(50);

  // entrada de 1 pessoa 
  if(order.equals("1-2")){
    people++;
    order = ""; 
    lastChar = "";
    hasNumPersonChanged = true;
  }
  // saida de 1 pessoa 
  else if(order.equals("2-1") && people > 0){
    people--;
    order = "";
    lastChar = "";
    hasNumPersonChanged = true;
  }

  if (hasNumPersonChanged) { // Mudou-se o contador
    if (people > maxPeople) { // ultrapassou a capacidade máxima 
      hasReachedMax = true;
      if (hadPreviousReachedMax == false){ // Primeira vez ultrapassando a capacidade máxima
        lcd.clear();
        lcd.setCursor(3,0); // centraliza o texto
        lcd.print("Atencao!!!");
        lcd.setCursor(0,1); 
        lcd.print("Limite Atingido!"); // texto centralizado
        firstReachedMax = true;
        firstReachedMax_LCDPrintTime = millis() + 1000;
      }
      if (firstReachedMax == false) reachedMaxPrint();
      hadPreviousReachedMax = true; 
    } else { // não ultrapassou a capacidade máxima
      ledState = 0;
      digitalWrite(ledPin, ledState); // desliga o led do 7 para parar de piscar
      hasReachedMax = false;
      printLcd();
      hadPreviousReachedMax = false;
    }
    hasNumPersonChanged = false;
  }
  if(hasReachedMax) blinkLed(); // O Led conectado ao pino 7 sempre pisca depois que ultrapasosu a capacidade máxima
  if(firstReachedMax){ // ReachedMaxPrint com 1s de delay na primeira vez que ocorrer a ultrapassagem
    if(millis() > firstReachedMax_LCDPrintTime){
      reachedMaxPrint();
      firstReachedMax = false;
    }
  }
}
  

void buttonDetect(){
  // Como não requer que o botao fique desligado entre os 2 clicks, segurar o botao também funciona
  buttonState = digitalRead(buttonPin);
  if (buttonState == 1){ // o botao foi pressionado
    delay(100);

    if(numClicks == 0){ // primeiro click
      timePressed = millis();
      timePressedLimit = timePressed + 1000; // o segundo click deve ser feito dentro de 0,2s a 1,2s
      numClicks = 1;
    }

    else if (numClicks == 1 && millis() < timePressedLimit){ // segundo click dentro do tempo
      calibrate(); // ativa a calibragem

      // resetar as variaveis
      timePressed = 0;
      timePressedLimit = 0;
      numClicks = 0;
    }

    else if (numClicks == 1 && millis() > timePressedLimit){ // segundo click fora do tempo 
      timePressed = 0;
      timePressedLimit = 0;
      numClicks = 0;
    }
  }
}

void calibrate(){
  // imprime a mensagem indicando o inicio da calibragem
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando");
  lcd.setCursor(0, 1);
  lcd.print("Calibragem");
  printTimer();

  // Calibragem da Luminosidade do Ambiente
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luminosidade");
  lcd.setCursor(0, 1);
  lcd.print("do Ambiente");
  buttonState = 0;
  while(buttonState == 0){ // pausa a execução do código até que seja pressionado o botão;
    buttonState = digitalRead(buttonPin);
  }
  ambientLuminosity1 = analogRead(ldrPin1);
  ambientLuminosity2 = analogRead(ldrPin2);
  //imprimir os valores lidos, pode-se comentar para pular essa parte
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ambiente1: ");
  lcd.print(ambientLuminosity1);
  lcd.setCursor(0, 1);
  lcd.print("Ambiente2: ");
  lcd.print(ambientLuminosity2);
  printTimer();
  
  // Calibragem da Luminosidade do Laser
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luminosidade");
  lcd.setCursor(0, 1);
  lcd.print("do Laser");
  buttonState = 0;
  while(buttonState == 0){ // pausa a execução do código até que seja pressionado o botão;
    buttonState = digitalRead(buttonPin);
  }
  laserLuminosity1 = analogRead(ldrPin1);
  laserLuminosity2 = analogRead(ldrPin2);
  //imprimir os valores lidos, pode-se comentar para pular essa parte
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Laser1: ");
  lcd.print(laserLuminosity1);
  lcd.setCursor(0, 1);
  lcd.print("Laser2: ");
  lcd.print(laserLuminosity2);
  printTimer();

  if (laserLuminosity1 > ambientLuminosity1 && laserLuminosity2 > ambientLuminosity2){// condição para garantir que a luminosidade do laser > ambiente
    // Determina o valor de trigger para cada sensor -> Média aritimética
    triggerValue1 = (laserLuminosity1 + ambientLuminosity1)/2;
    triggerValue2 = (laserLuminosity2 + ambientLuminosity2)/2;
    //Indica o fim da calibragem
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Trigger 1: ");
    lcd.print(triggerValue1);
    lcd.setCursor(0, 1);
    lcd.print("Trigger 2: ");
    lcd.print(triggerValue2);
    printTimer();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibragem");
    lcd.setCursor(0, 1);
    lcd.print("Completa!");
    printTimer();

    printLcd();
    }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibragem");
    lcd.setCursor(0, 1);
    lcd.print("Falhou!");
    delay(1000);
    calibrate();
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

void reachedMaxPrint(){ 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Perigo! Max: ");
  lcd.print(maxPeople);
  lcd.setCursor(0, 1);
  lcd.print("Pessoas: ");
  lcd.print(people);  
}

void blinkLed(){
  if (millis() >= ledTime){
    ledTime = millis() + period;
    ledState = 1 - ledState;
    digitalWrite(ledPin, ledState);
  }
}

void readSensors(){
  ldrValue1 = analogRead(ldrPin1);  // Quanto maior o valor, maior a incidencia de luminosidade
  ldrValue2 = analogRead(ldrPin2);  // Quando menor o valor lido, menor a incidencia de luminosidade
                                    // O valor lido será entre 0 e 1023
  
  // caso o sensor LDR1 seja interceptado e não está recebendo luminosidade o suficiente e o sensor LDR2 esteja recebendo luminosidade
  if (ldrValue1 <= triggerValue1 && ldrValue2 > triggerValue2 && lastChar != "1"){
    order += "1";
    lastChar = "1";
    Serial.println(order);
  }

  // caso os sensores estejam sendo interceptados 
  else if(ldrValue1 <= triggerValue1 && ldrValue2 <= triggerValue2 && lastChar != "-"){
    order += "-";
    lastChar = "-";
    Serial.println(order);
  }
  
  // caso o sensor LDR2 seja interceptado e não está recebendo luminosidade o suficiente e o sensor LDR1 esteja recebendo luminosidade
  else if(ldrValue2 <= triggerValue2 && ldrValue1 > triggerValue1 && lastChar != "2"){
    order += "2";
    lastChar = "2";
    Serial.println(order);
  }

  // caso uma pessoa ative o primeiro sensor e saia
  // e dps outra pessoa ative o segundo sensor, seja pra sair
  // ou simplesmente esbarrando o order passa a contar dessa 
  // nova leitura 
  else if(order.length() == 2 && order.charAt(1) != '-'){
    order.remove(0, 1);
  }

  // se order tiver 3 caracteres e não indicar nem entrada 
  // nem saída, apaga o order e o ultimo digito junto
  // e o numero de pessoas permanece inalterado 
  else if(order.length() >= 3){
    if(order != "1-2" || order != "2-1"){
      order = "";
      lastChar = "";
    }
  }
}

void printTimer(){
  lcd.setCursor(14, 1);
  lcd.print("#");
  lcd.setCursor(15, 1);
  lcd.print(3);
  delay(1000);
  lcd.setCursor(15, 1);
  lcd.print(2);
  delay(1000);
  lcd.setCursor(15, 1);
  lcd.print(1);
  delay(1000);
}
  