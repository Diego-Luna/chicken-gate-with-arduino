// MoonMakers
#include <ThreeWire.h>  
#include <RtcDS1302.h>

#include <LiquidCrystal.h>

const int pinPWMA = 6;
const int pinAIN2 = 7;
const int pinAIN1 = 8;
const int pinSTBY = 9; // 12

const int waitTime = 2000;   //espera entre fases
const int speed = 200;      //velocidad de giro

const int pinMotorA[3] = { pinPWMA, pinAIN2, pinAIN1 };

enum moveDirection {
  forward,
  backward
};

bool activate = false;

const int LDRPin = A0;
int input = 0;

const int ButtonTop = A3;
const int ButtonBottom = A2;
const int ButtonOk = A1;

int statusOkButton = 0; //0 -> no se ha echo nada, 1 -> recalibrar, 3 -> abrier puerta, 4 -> cerrar puerta,
int valueOkButton = HIGH;
int statusTopButton = HIGH;
int statusBottomButton = HIGH;

int TiempoAbrirPuerta = 0;
int TiempoCerrarPuerta = 0;

unsigned long myOriginalTime = 0;
unsigned long myTime1 = 0;
unsigned long myTime2 = 0;

unsigned long mySeconds = 0;
unsigned long myMinutes = 0;
unsigned long myHours = 0;

bool time6Hours = true;

int dayStatus = 0; // 0 -> no definido, 1 -> Dia , 2-> noche

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);

  pinMode(pinAIN2, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinPWMA, OUTPUT);

  pinMode(ButtonTop, INPUT);
  pinMode(ButtonBottom, INPUT);
  pinMode(ButtonOk, INPUT);

  pinMode(LDRPin, INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  // Print a message to the LCD.
  lcd.print("   MoonMakers   ");

  delay(2000);
  lcd.clear();
}

void loop() {

  valueOkButton = digitalRead(statusOkButton);

  if (statusOkButton == 2 || statusOkButton == 3) {

    input = analogRead(LDRPin);

    Serial.print("-> LDRPin :");
    Serial.println(input);

    timeRead();

    // noche
    if (input < 30 && activate == false && time6Hours == true && ( dayStatus == 1 ||  dayStatus == 0) ) {
      Serial.print("--->  if 1 noche");
      enableMotors();
      move(forward, 180);
      delay(waitTime);
      activate = true;
      fullStop();

      myTime2 = myTime1;
      time6Hours = false;
      dayStatus = 2;
    }

    // dia
    if ( input > 30 && activate == true && time6Hours == true && ( dayStatus == 2 ||  dayStatus == 0)  ) {
      Serial.print("--->  if 2 Dia");
      enableMotors();
      move(backward, 180);
      delay(waitTime);
      activate = false;
      fullStop();

      myTime2 = myTime1;
      time6Hours = false;
      dayStatus = 1;
    }


    delay(250);

  } else {
    lcd.clear();

    String textA = "A que hora se abra la puerta";
    String textB = "A que hora se cierra la puerta";

    int tam_textoA = textA.length();
    int tam_textoB = textB.length();

    if (statusOkButton < 3) {
      for (int i = tam_textoA; i > 0 ; i--) {
        String texto = textA.substring(i - 1);

        // Limpiamos pantalla
        lcd.clear();

        //Situamos el cursor
        lcd.setCursor(0, 0);

        // Escribimos el texto
        lcd.print(texto);

        BottonsTime(1);

        lcd.setCursor(0, 1);
        lcd.print(TiempoAbrirPuerta);
        // Esperamos
        delay(250);
      }
    }

    if (statusOkButton == 3) {
      for (int i = tam_textoB; i > 0 ; i--) {
        String texto = textB.substring(i - 1);

        // Limpiamos pantalla
        lcd.clear();

        //Situamos el cursor
        lcd.setCursor(0, 0);

        // Escribimos el texto
        lcd.print(texto);

        BottonsTime(2);

        lcd.setCursor(0, 1);
        lcd.print(TiempoCerrarPuerta);
        // Esperamos
        delay(250);
      }
    }


  }


}


void BottonsTime(int varTime) {
  statusTopButton = digitalRead(ButtonTop);
  statusBottomButton = digitalRead(ButtonBottom);
  valueOkButton = digitalRead(ButtonOk);

  if (statusTopButton == LOW) {
    Serial.println("-> statusTopButton > " + varTime);

    if (varTime = 1) {
      TiempoAbrirPuerta += 1;
    } else {
      TiempoCerrarPuerta += 1;
    }

  }

  if (statusBottomButton == LOW) {
    Serial.println("-> statusBottomButton < " + varTime);

    if (varTime = 1) {
      TiempoAbrirPuerta -= 1;
    } else {
      TiempoCerrarPuerta -= 1;
    }

  }

  if (valueOkButton == LOW) {
    if (varTime <= 2 && varTime != 4) {
      statusOkButton = 3;
    }

    if (varTime <= 3) {
      statusOkButton = 4;
    }
  }

}

// time
void timeRead() {
  myOriginalTime = millis();

  if (myTime2 == 0) {
    myTime1 = myOriginalTime;
  } else {
    Serial.println("----------------------------------------------");
    Serial.print("-- En el else if, time2: ");
    Serial.print( myTime2);
    Serial.print(" myOriginalTime: ");
    Serial.println(myOriginalTime);
    Serial.println("----------------------------------------------");
    myTime1 = myOriginalTime - myTime2;

    delay(250);
  }

  // dia
  if (input > 30 && dayStatus == 0) {

    Serial.println();
    Serial.print("- Dia - Dia - Dia - Dia - Dia - Dia - Dia- Dia - Dia - Dia - Dia - Dia -");
    Serial.println();
    dayStatus = 1;
  }

  // noche
  if (input < 30 && dayStatus == 0) {
    Serial.println();
    Serial.print("* noche * noche * noche * noche * noche * noche * noche * noche * noche *");
    Serial.println();
    dayStatus = 2;
  }

  if (myHours >= 6) {
    time6Hours = true;
  }


  mySeconds = ((float) myTime1 / 1000);
  myMinutes = mySeconds / 60;
  myHours = myMinutes / 60;


  Serial.println();
  Serial.print("_> myTime1: ");
  Serial.println(myTime1);
  Serial.println();

  Serial.println();
  Serial.print("_> mySeconds: ");
  Serial.println(mySeconds);
  Serial.println();

  Serial.println();
  Serial.print("_> myMinutes: ");
  Serial.println(myMinutes);
  Serial.println();


  Serial.println();
  Serial.print("_> myHours: ");
  Serial.println(myHours);
  Serial.println();
}


//Funciones que controlan el vehiculo
void move(int direction, int speed)
{
  if (direction == forward)
  {
    moveMotorForward(pinMotorA, speed);
    //moveMotorForward(pinMotorB, speed);
  }
  else
  {
    moveMotorBackward(pinMotorA, speed);
    //moveMotorBackward(pinMotorB, speed);
  }
}


void fullStop()
{
  disableMotors();
  stopMotor(pinMotorA);
  //stopMotor(pinMotorB);
}

//Funciones que controlan los motores

void moveMotorForward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], HIGH);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], speed);
}

void moveMotorBackward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], HIGH);

  analogWrite(pinMotor[0], speed);
}

void stopMotor(const int pinMotor[3])
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], 0);
}

void enableMotors()
{
  digitalWrite(pinSTBY, HIGH);
}

void disableMotors()
{
  digitalWrite(pinSTBY, LOW);
}
