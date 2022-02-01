// -> MoonMakers <-

// -> time module <-
#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(A4, A5, A2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// -> LCD 16x2 <-

#include <LiquidCrystal.h>

// -> DC Motor <-

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

// -> Fotorresistor <-

const int LDRPin = A0;
int input = 0;

// -> LCD buttons 16x2 <-

const int ButtonTop = A1;
const int ButtonBottom = 13;
const int ButtonOk = 10;

int statusOkButton = 0; //0 -> no se ha echo nada, 1 -> recalibrar, 2 -> abrier puerta, 3 -> cerrar puerta,
int valueOkButton = HIGH;
int statusTopButton = HIGH;
int statusBottomButton = HIGH;

// -> time module <-

int TiempoAbrirPuerta = 1;
int TiempoCerrarPuerta = 23;


int dayStatus = 0; // 0 -> no definido, 1 -> Dia , 2-> noche

// -> time module <-

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

  pinMode(13, INPUT);
  pinMode(10, INPUT);
  // -> LCD 16x2 <-

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //Serial.begin(9600);
  // Print a message to the LCD.
  lcd.print("   MoonMakers   ");

  delay(2000);
  lcd.clear();

  Rtc.Begin();

  // -> modulo time <-
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

void loop() {

  // -> time module <-

  RtcDateTime now = Rtc.GetDateTime();

 // printDateTime(now);
  Serial.println();

  if (statusOkButton == 3) {

    VerificarTiempo(now);

    //input = analogRead(LDRPin);

    //Serial.print("-> LDRPin :");
    //Serial.println(input);

    // noche
    //if (input < 30 && activate == false && time6Hours == true && ( dayStatus == 1 ||  dayStatus == 0) ) {
    if (activate == false && ( dayStatus == 1 ||  dayStatus == 0) ) {
      Serial.print("--->  if 1 noche");
      enableMotors();
      move(forward, 180);
      delay(waitTime);
      activate = true;
      fullStop();
    }

    // dia
    //if ( input > 30 && activate == true && time6Hours == true && ( dayStatus == 2 ||  dayStatus == 0)  ) {
    if (activate == true && ( dayStatus == 2 ||  dayStatus == 0)  ) {
      Serial.print("--->  if 2 Dia");
      enableMotors();
      move(backward, 180);
      delay(waitTime);
      activate = false;
      fullStop();

    }


    delay(250);

  } else {

    lcd.clear();

    String textA = "            A que hora se abra la puerta";
    String textB = "            A que hora se cierra la puerta";

    int tam_textoA = textA.length();
    int tam_textoB = textB.length();

    BottonsTime(0);

    if (statusOkButton == 2) {
      for (int i = tam_textoB; i > 0 ; i--) {
        String texto = textB.substring(i - 1);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(texto);

        BottonsTime(2);

        lcd.setCursor(0, 1);
        lcd.print(TiempoCerrarPuerta);

        if (statusOkButton == 3 ) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Listo Hora   ");
          lcd.setCursor(0, 1);
          lcd.print("   establecida  ");

          delay(5000);
          break;
        }

        delay(500);
      }
    }

    BottonsTime(0);

    if (statusOkButton == 0 || statusOkButton == 1) {
      for (int i = tam_textoA; i > 0 ; i--) {
        String texto = textA.substring(i - 1);

        // Limpiamos pantalla
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(texto);
        BottonsTime(1);

        lcd.setCursor(0, 1);
        lcd.print(TiempoAbrirPuerta);

        if (statusOkButton >= 1 ) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Listo Hora   ");
          lcd.setCursor(0, 1);
          lcd.print("   establecida  ");

          delay(5000);
          break;
        }

        delay(400);
      }
    }

    BottonsTime(0);


  }

  delay(200);
}


void BottonsTime(int varTime) {
  statusTopButton = digitalRead(ButtonTop);
  statusBottomButton = digitalRead(ButtonBottom);
  valueOkButton = digitalRead(ButtonOk);

  if (statusTopButton == LOW) {
    Serial.println("-> statusTopButton > " + varTime);

    if (varTime == 1) {
      if (TiempoAbrirPuerta <= 22) {
        TiempoAbrirPuerta += 1;
      }

    } else {
      if (TiempoAbrirPuerta <= 22) {
        TiempoCerrarPuerta += 1;
      }

    }

  }

  if (statusBottomButton == LOW) {
    Serial.println("-> statusBottomButton < " + varTime);

    if (varTime == 1) {
      if (TiempoAbrirPuerta >= 2) {
        TiempoAbrirPuerta -= 1;
      }

    } else {
      if (TiempoAbrirPuerta >= 2) {
        TiempoCerrarPuerta -= 1;
      }

    }

  }

  if (valueOkButton == LOW) {
    Serial.print("-> valueOkButton OK -> ");
    Serial.println(statusOkButton);

    if (statusOkButton == 2) {
      statusOkButton = 3;

    }

    if (statusOkButton == 0 || statusOkButton == 1) {
      statusOkButton = 2;

    }
  }

}


// -> DC Motor

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

// -> time module
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt , bool valueReturn) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );

   
  Serial.print(datestring);

  return datestring;
}
void VerificarTiempo(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);

  if ( TiempoAbrirPuerta == dt.Hour() ) {
    dayStatus = 1;
  }

  if ( TiempoCerrarPuerta == dt.Hour() ) {
    dayStatus = 2;
  }


}
