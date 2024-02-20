#include <SPI.h>
#include <MFRC522.h>
#include <stdio.h>

#define RST_PIN         9           
#define SS_PIN          10          

#define RED_LED         6
#define GREEN_LED       5

MFRC522 mfrc522(SS_PIN, RST_PIN);   

byte buffer[18];
byte block;
byte cardData[64][16];


static byte Uid[4];

MFRC522::StatusCode status;
    
MFRC522::MIFARE_Key key;

#define NR_KNOWN_KEYS   8

char choice;


void setup() {
    Serial.begin(9600);         
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    while (!Serial);            
    SPI.begin();                
    mfrc522.PCD_Init();         

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}


void loop() {
  start();
    
}


void start(){
  choice = Serial.read();
  
  if(choice == '1')
  {
    cloneCard();      
  }
  else if(choice == '2'){
      changeUID();
  }
  else if(choice == '3')
    {
      writeToNewCard();
    }
  else if(choice == '4'){
    dumpCard();
  }
}


void cloneCard(){ 
  digitalWrite(RED_LED, HIGH);
  
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;


    if ( ! mfrc522.PICC_ReadCardSerial())
        return;


    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
   
    writeCardDataToBuffer(&key);
        
    byte tempUid[4];
    for(byte i = 0; i < 4; i++){
      tempUid[i] = mfrc522.uid.uidByte[i];
    }

  byte byteMassHEX[4];

    for(int i = 0; i< 4; i++){
      String tmp = "";
      do
      {
          int r(tempUid[i] % 16);
          if (r > 9) { 
            r += (int)'A' - 10; 
          }
          else {
             r += (int)'0'; 
          };
          tmp = (char)r + tmp;
          tempUid[i] /= 16;
      } while (tempUid[i]);

    byteMassHEX[i] = strtol(tmp.c_str(), NULL, 16);
    }
    
    if(!memcpy(Uid, byteMassHEX, 4 )) {
      Serial.println("UID не скопирован");
      start();
    }
  
    digitalWrite(RED_LED, LOW);

    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(GREEN_LED, LOW);
    
    start();
}


void changeUID(){
  digitalWrite(RED_LED, HIGH);
   if ( ! mfrc522.PICC_IsNewCardPresent())
          return;

   if ( ! mfrc522.PICC_ReadCardSerial())
          return;

  if(mfrc522.MIFARE_SetUid(Uid, byte(4), true))
      {
        Serial.print("Новый uid записан");
      } 
    else
    {
        Serial.print("НЕ получилось, не фортануло \n");
    }


    mfrc522.PICC_HaltA();       
    mfrc522.PCD_StopCrypto1(); 

    digitalWrite(RED_LED, LOW);

    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(GREEN_LED, LOW);

    start();
}

void writeToNewCard(){ 

    digitalWrite(RED_LED, HIGH);

    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

  for(int i = 4; i <= 62; i++){ 
      if(i == 7 || i == 11 || i == 15 || i == 19 || i == 23 || i == 27 || i == 31 || i == 35 || i == 39 || i == 43 || i == 47 || i == 51 || i == 55 || i == 59){
        i++;
      }
      block = i;
      
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("Аутентификация ключом А провалена"));
          return;
      }
      
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("Аутентификация ключом B провалена"));
          return;
      }
      
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(block, cardData[block], 16);
        if (status != MFRC522::STATUS_OK) {
          Serial.print(F("Запись провалена"));
        }
  }
  mfrc522.PICC_HaltA();       
  mfrc522.PCD_StopCrypto1();
  
  digitalWrite(RED_LED, LOW);

  Serial.println("Карта записана!");

  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(GREEN_LED, LOW);

  start();
}


void writeCardDataToBuffer(MFRC522::MIFARE_Key *key)
{
    bool result = false;

    for(byte block = 0; block < 64; block++){

        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("Аутентификация ключом А провалена"));
           return false;
        }

        byte byteCount = sizeof(buffer);
        status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
        if (status != MFRC522::STATUS_OK) {
              Serial.print(F("Чтение провалено"));
              return false;
        }
        else {
            result = true;
            for (int p = 0; p < 16; p++) {
              cardData [block][p] = buffer[p];
            }
          }
    }

    if(result){
      Serial.print("Успех!");
    }else{
      Serial.print("Копирование провалено!");
    }
    
    mfrc522.PICC_HaltA();      
    mfrc522.PCD_StopCrypto1();  
    
    start();
}

void dumpCard(){
  	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}




