#define RFID_ENABLED
#define BUZZER_ENABLED

#ifdef RFID_ENABLED
  #include <SPI.h>
  #include <MFRC522.h>
  #include <SoftwareSerial.h>
  
  #define PIN_RFID_RST                          9
  #define PIN_RFID_SDA                          10
  #define PIN_BLE_RXD                           3
  #define PIN_BLE_TXD                           4
  
  #define RFID_MS                               250
  unsigned long rfidLastTime                    = 0;
  
  MFRC522 mfrc522(PIN_RFID_SDA, PIN_RFID_RST);   // Create MFRC522 instance.
  
  byte readCard[4];   // Stores scanned ID read from RFID Module
  
  SoftwareSerial btSerial(PIN_BLE_RXD, PIN_BLE_TXD); // RX | TX
#endif

#ifdef BUZZER_ENABLED
  #define PIN_BUZZER                            2
  #define HIGH_SOUND                            3001
  #define LOW_SOUND                             1000
  #define SOUND_STEP                            500
  #define SOUND_MS                              50
  int soundToneTarget                           = 0;
  int soundToneCurr                             = 0;
  unsigned long soundToneLastTime;
#endif

unsigned long currTime;

#ifdef BUZZER_ENABLED
void scanSound() {
  tone(PIN_BUZZER, 2000, 100);
  delay(100);
  tone(PIN_BUZZER, 2000, 100);
}

void initSound() {
  tone(PIN_BUZZER, 3000, 1000);
}

void lockSound() {
  soundToneCurr = HIGH_SOUND;
  soundToneTarget = LOW_SOUND;
}

void unlockSound() {
  soundToneCurr = LOW_SOUND;
  soundToneTarget = HIGH_SOUND;
}

void buzzerLoop() {
  if (currTime - soundToneLastTime > SOUND_MS) {
    soundToneLastTime = currTime;
    if (soundToneCurr != soundToneTarget) {
      tone(PIN_BUZZER, soundToneCurr, SOUND_MS);
      if (soundToneCurr < soundToneTarget) {
        if (soundToneTarget - soundToneCurr <= SOUND_STEP) {
          soundToneCurr = soundToneTarget;
        }
        else {
          soundToneCurr+=SOUND_STEP;
        }
      }
      else if (soundToneCurr > soundToneTarget) {
        if (soundToneCurr - soundToneTarget <= SOUND_STEP) {
          soundToneCurr = soundToneTarget;
        }
        else {
          soundToneCurr-=SOUND_STEP;
        }
      }
    }
  }
}
#endif

#ifdef RFID_ENABLED
void printBytes( byte toPrint[] ) {
  Serial.print("0x");
  for ( uint8_t i = 0; i < 4; i++ ) {
    Serial.print(toPrint[i], HEX);
  }
  Serial.println("");
}

void initRFID() {
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

void copyBytes( byte* src, byte* dst ) {
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    dst[k] = src[k];
  }
}
///////////////////////////////////////// Check Bytes   ///////////////////////////////////
bool checkTwo ( byte a[], byte b[] ) {   
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] ) {     // IF a != b then false, because: one fails, all fail
       return false;
    }
  }
  return true;  
}

bool tryReadRFID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return false;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.print("Scanned PICC's UID:");
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  printBytes(readCard);
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}

void rfidLoop() {
  if (currTime - rfidLastTime < RFID_MS) {
    return;
  }

  if (tryReadRFID()) {
    Serial.println("Sending to master arduino... >> ");
    sendBLEData(readCard);
#ifdef BUZZER_ENABLED
//    scanSound();
    lockSound();
#endif
  }
}

void sendBLEData(const byte* data) {
  Serial.print("Sending data: ");

  for(int i = 0 ; i < 4; ++i) {
    btSerial.write(data[i]);
  }
  delay(500);
}

void bleLoop() {
  if (btSerial.available()) {
    char reply[100];
    int i = 0;
    while(btSerial.available()) {
      reply[i] = btSerial.read();
      i += 1;
    }
    reply[i] = '\0';
    Serial.print(reply); 
  }
}

void initBLE() {
  btSerial.begin(115200);
  delay(100);
  btSerial.println("U,9600,N");
  btSerial.begin(9600);  // HC-05 default speed in AT command more
}

#endif


void setup() {
  Serial.begin(9600);
  while(!Serial) {
    Serial.println("Waiting for Serial port...");
  }
  Serial.println("Serial READY!\n");
  
#ifdef RFID_ENABLED
  initRFID();
  initBLE();
#endif
#ifdef BUZZER_ENABLED
  pinMode(PIN_BUZZER, OUTPUT);
  initSound();
#endif
}

void loop() {
  currTime = millis();

#ifdef RFID_ENABLED
  rfidLoop();
  bleLoop();
#endif
#ifdef BUZZER_ENABLED
  buzzerLoop();
#endif
}
