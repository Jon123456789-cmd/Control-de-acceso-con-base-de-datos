
//RST          D8
//SDA(SS)      D9
//MOSI         D51
//MISO         D50
//SCK          D52

//Huellas - Amarillo D11
//Huellas - Blanco D10

//VSS -
//VDD +5
//A 5+
//K -
//VO MEDIO POT
//RS D2
//RW -
//E D3
//D4 D4
//D5 D5 
//D6 D6
//D7 D7

//Servo 12

//Buzzer 13

//Librerías

#include <Servo.h>
#include <LiquidCrystal.h>
#include <SPI.h>                    
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

//Comunicaciones

SoftwareSerial mySerial(11, 10);  // Comunicación lector de huellas
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
SoftwareSerial s(15, 14);   // Comunicación con NodeMCU
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);   // Comunicación con pantalla LCD

//Variables

Servo servoMotor; 
 
const int RST_PIN = 8;        // Pin 8 para el reset del RC522
const int SS_PIN = 9;        // Pin 9 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Crear instancia del MFRC522
const int buzzer = 13;
int led = 48;
int rojo = 49;
int encendido = 42;
int boton = 46;
int estadoboton = 0;
int boton2 =45;
int estadoboton2 = 0;
int cerrar = 44;
int estadocerrar = 0;
int output = 22;

//IDs válidas RFID 

byte LecturaUID[4];
byte Lander[4]={0xDD, 0x6B, 0xD5, 0xE5};  
byte Jon[4]={0xF0, 0x35, 0xB1, 0xC5};

//Función para comparar dos vectores

bool isEqualArray(byte* arrayA, byte* arrayB, int length)
{
  for (int index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

uint8_t id;
 
void setup() {

  Serial.begin(9600);    // Puerto Serie
  SPI.begin();           // Iniciar SPI
  mfrc522.PCD_Init();    // Iniciar MFRC522
  pinMode (led, OUTPUT);
  pinMode (rojo, OUTPUT);
  pinMode (encendido, OUTPUT);
  pinMode (boton, INPUT);
  pinMode (boton2, INPUT);
  pinMode (cerrar, INPUT);
  pinMode (buzzer, OUTPUT);
  pinMode (output, OUTPUT);
  servoMotor.attach(12);
  servoMotor.write(180);  //Servo en posición de cerrado
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.write("Comprobando");
  lcd.setCursor(0,1);
  lcd.write("Acceso..........");
  digitalWrite (output, HIGH);

//Detectar lector huella

  while (!Serial);  
  delay(100);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Buscando sensor");
  lcd.setCursor(0,1);
  lcd.write(".............................");

  finger.begin(57600);
  delay(2000);
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Sensor detectado");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Comprobando");
    lcd.setCursor(0,1);
    lcd.write("Acceso..........");
  } 
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("No detectado");
    while (1) { delay(1); }
  }
  digitalWrite (encendido, HIGH);   // Led de encendido
}
 
void loop() {

 getFingerprintIDez();
 delay(50);            //don't ned to run this at full speed.

  //Lectura botones

  estadoboton = digitalRead (boton);
  estadoboton2 = digitalRead (boton2);
  estadocerrar = digitalRead (cerrar);
  
  // Detectar tarjeta
  if (mfrc522.PICC_IsNewCardPresent())
  {
    //Seleccionamos una tarjeta
    if (mfrc522.PICC_ReadCardSerial())
    {
      // Comparar ID con las claves válidas
        if (isEqualArray(mfrc522.uid.uidByte,Lander,4)) {
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.write("Bienvenido");
         lcd.setCursor(0,1);
         lcd.write("Lander");
         int zein =1;        
         permitido (zein);
         }
        else if (isEqualArray(mfrc522.uid.uidByte,Jon,4)){
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.write("Bienvenido");
         lcd.setCursor(0,1);
         lcd.write("Jon");
         int zein =2; 
         permitido (zein);
         }
        else  {   // Si la ID no coincide cumple la función denegado
         denegado ();
         }

      // Finalizar lectura actual
      mfrc522.PICC_HaltA();
    }
  }
  delay(250);

 if (estadoboton == LOW){   //Botón con función de leer IDs de tarjetas en la pantalla LCD

  // Detecta tarjeta
  if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
       // Seleccionamos una tarjeta
       if ( mfrc522.PICC_ReadCardSerial()) 
            {
         // Enviamos serialemente su UID
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("Card UID");
         lcd.setCursor(0,1);
            for (byte i = 0; i < mfrc522.uid.size; i++) { 
            // Visualizamos en la pantalla la UID
            lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            lcd.print(mfrc522.uid.uidByte[i], HEX);
            lcd.print(" ");
                  } 
         Serial.println();
         // Terminamos la lectura de la tarjeta  actual
         mfrc522.PICC_HaltA();         
            } 
         delay (1500);
         lcd.setCursor(0,0);
         lcd.write("Comprobando");
         lcd.setCursor(0,1);
         lcd.write("Acceso..........");
         delay(1500);      
        }
      }

  if (estadoboton2 == LOW){  // Botón con la función de registrar nuevas huellas dactilares mediante monitor serie
    
     Serial.println("Ready to enroll a fingerprint!");
     Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
     id = readnumber();
     if (id == 0) {// ID #0 not allowed, try again!
     return;
     }
     Serial.print("Enrolling ID #");
     Serial.println(id);
  
     while (!  getFingerprintEnroll() );  // Función  getFingerprintEnroll
  }

  if (estadocerrar == LOW){  //Botón con la función de cerrar la puerta
    servoMotor.write(180);
    delay (500);
    lcd.setCursor(0,0);
    lcd.write("Comprobando");
    lcd.setCursor(0,1);
    lcd.write("Acceso..........");
  }
      
}

void permitido (int zein){  // Función que se cumple siempre que alguien presenta su acceso permitido
  Serial.println(zein);
  digitalWrite(output, LOW);
  s.write(zein); 
  digitalWrite(led, HIGH);
  // Servo abierta
  servoMotor.write(90);
  // Sonido de buzzer
  tone(buzzer, 2000, 500);
  delay (600);
  digitalWrite(output, HIGH);
  noTone(buzzer);
  delay (200);
  digitalWrite(led, LOW);
  delay (500);
  // Texto para cerrar la puerta en la pantalla
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Cerrar");
  lcd.setCursor(0,1);
  lcd.write("Puerta");
  delay(100);
}

void denegado (){   // Función que se cumple cuando se presenta alguien sin acceso
  // Texto de acceso denegado en la pantalla
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Acceso");
  lcd.setCursor(0,1);
  lcd.write("Denegado");
  digitalWrite(rojo, HIGH);
  // Puerta bloqueada
  servoMotor.write(180);
  // Tres pitidos de buzzer
  tone(buzzer, 4000, 1000);
  delay (300);
  noTone(buzzer);
  delay (300);
  tone(buzzer, 4000, 1000);
  delay (300);
  noTone(buzzer);
  delay (300);
  tone(buzzer, 4000, 1000);
  delay (300);
  noTone(buzzer);
  delay (200);
  digitalWrite(rojo, LOW);
  delay (500);
  lcd.setCursor(0,0);
  lcd.write("Comprobando");
  lcd.setCursor(0,1);
  lcd.write("Acceso..........");
}


// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK){  
    return -1;  
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
  return -1; 
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) { 
    // Huella sin coincidencia, cumple la función denegado
    denegado ();    
    return -1; 
  }
  
  // Encuentra coincidencia con huella registrada
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Bienvenido");
  int zein =3;
  permitido (zein);
  return finger.fingerID; 
}


uint8_t readnumber(void) {  // Función para el registro de huellas
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll() {   // Función para el registro de huellas

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
