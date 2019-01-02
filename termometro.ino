// bibliotecas necessárias
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <SparkFunMLX90614.h>
#include <EEPROM.h>

// portas do arduino
#define btnMinus 6
#define btnPlus 5
#define btnMode 4
#define relay 3

// variaveis globais
int   operation = 1,
      eeSetPoint = 1,
      eeHisterese = 20;

float histerese = 0.1,
      setPoint = 0,
      setPointMin = 0,
      SetPointMax = 250,
      objTemp = 1;

// objetos
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);
IRTherm sensor;

// prototipo das funcoes
void monitor();
void setupT();
void setupH();
void EEPROMWriteDouble(int ee, double value);
double EEPROMReadDouble(int ee);

void setup()
{
  // inicializa eeprom
  if (EEPROM[0] != 8) {
    EEPROM[0] = 8;
    //    EEPROM[1] = setPoint;
    //    EEPROM[20] = histerese;
    EEPROMWriteDouble(eeSetPoint, setPoint);
    EEPROMWriteDouble(eeHisterese, histerese);
  } else {
    setPoint  = EEPROMReadDouble(eeSetPoint);
    histerese = EEPROMReadDouble(eeHisterese);
  }

  // inicializa rele
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  //inicializa botoes
  pinMode(btnMinus, INPUT_PULLUP);
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(btnPlus, INPUT_PULLUP);


  // inicializa sensor de temperatura
  sensor.begin();
  sensor.setUnit(TEMP_C);

  // inicializa display
  //Serial.begin(9600);
  display.begin();
  display.setContrast(50);     //Ajusta o contraste do display
  display.clearDisplay();      //Apaga o buffer e o display
  display.setTextSize(1);      //Seta o tamanho do texto
  display.setTextColor(BLACK); //Seta a cor do texto
  display.setCursor(0, 0);     //Seta a posição do cursor
  display.setTextSize(1);
  display.println("INBRACO");
  display.println("TECNOLOGIA");
  display.println("ORTOPEDICA");
  display.println("Fabricado no Brasil");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.println("CONTROLADOR DE");
  display.println("TEMPERATURA");
  display.println("MEDICAO POR IR");
  display.println("Modelo CTIR201");
  display.display();
  delay(2000);
}

void loop()
{
  if (digitalRead(btnMode) == LOW)
  {
    operation++;
    delay(100);
  }
  switch (operation)
  {
    case 1:
      monitor();
      break;
    case 2:
      setupT();
      break;
    case 3:
      setupH();
      break;
    default:
      operation = 1;
      break;
  }
  delay(10);
}

void monitor()
{
  if (sensor.read())
  {
    objTemp = sensor.object();
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.println("TEMPERATURA");
  display.setTextSize(2);
  display.print(objTemp);
  display.println("*C");
  display.print("Setup: ");
  display.print(setPoint);
  display.display();
  //Serial.println(objTemp);

  // Metodo para ligar ou desligar o rele com base na temperatura
  if (objTemp >= (setPoint + histerese))
  {
    digitalWrite(relay, HIGH);
  }
  if (objTemp <= (setPoint - histerese))
  {
    digitalWrite(relay, LOW);
  }
  delay(800);
}

void setupT()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.println("DEFINA");
  display.println("TEMPERATURA");

  if (digitalRead(btnPlus) == LOW)
  {
    setPoint += 0.1;
    delay(50);
  }

  if (digitalRead(btnMinus) == LOW)
  {
    setPoint -= 0.1;
    delay(50);
  }

  if (setPoint <= 0) {
    setPoint = 0;
  }

  if (setPoint >= 250) {
    setPoint = 250;
  }

  if ((EEPROMReadDouble(eeSetPoint) > setPoint + 0.1) || EEPROMReadDouble(eeSetPoint) < setPoint - 0.1)
  {
    EEPROMWriteDouble(eeSetPoint, setPoint);
  }

  display.setTextSize(2);
  display.print(setPoint);
  display.display();
  delay(200);
}

void setupH()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.println("DEFINA");
  display.println("HISTERESE");

  if (digitalRead(btnPlus) == LOW)
  {
    histerese += 0.1;
    delay(50);
  }

  if (digitalRead(btnMinus) == LOW)
  {
    histerese -= 0.1;
    delay(50);
  }

  if (histerese <= 0)
  {
    histerese = 0;
  }

  if (EEPROMReadDouble(eeHisterese) != histerese)
  {
    EEPROMWriteDouble(eeHisterese, histerese);
  }

  display.setTextSize(2);
  display.print(histerese);
  display.display();
  delay(200);
}

void EEPROMWriteDouble(int ee, double value)
{
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
}

double EEPROMReadDouble(int ee)
{
  double value = 0.0;
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return value;
}
