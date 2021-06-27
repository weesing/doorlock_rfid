#include <SPI.h>
#include <MFRC522.h>

#define PIN_RFID_RST 9
#define PIN_RFID_SDA 10
#define PIN_BLE_RXD 3
#define PIN_BLE_TXD 4

#define RFID_MS 250
unsigned long rfidLastTime = 0;

MFRC522 mfrc522(PIN_RFID_SDA, PIN_RFID_RST); // Create MFRC522 instance.

byte readCard[4]; // Stores scanned ID read from RFID Module

#define PIN_BUZZER 2
#define HIGH_SOUND 1251
#define LOW_SOUND 250
#define INIT_SOUND 1000
#define RFID_INIT_SOUND 2000
#define RFID_INIT_MS 1000
#define SOUND_STEP 250
#define SOUND_MS 50
int soundToneTarget = 0;
int soundToneCurr = 0;
unsigned long soundToneLastTime;

unsigned long currTime;

void powerRangers()
{
  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND - 100, 100);
  delay(100);
  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND + 200, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(500);
}

void doraemon()
{
  tone(PIN_BUZZER, INIT_SOUND - 250, 300);
  delay(300);
  tone(PIN_BUZZER, INIT_SOUND, 50);
  delay(100);
  tone(PIN_BUZZER, INIT_SOUND, 150);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND + 255, 150);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND + 700, 200);
  delay(400);
  tone(PIN_BUZZER, INIT_SOUND + 255, 200);
  delay(400);
  tone(PIN_BUZZER, INIT_SOUND + 500, 400);
  delay(400);
}

void zelda()
{
  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(150);
  tone(PIN_BUZZER, INIT_SOUND + 52, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND + 105, 100);
  delay(250);
  tone(PIN_BUZZER, INIT_SOUND + 160, 600);
  delay(600);
}

void finalFantasy()
{
  tone(PIN_BUZZER, INIT_SOUND, 80);
  delay(160);
  tone(PIN_BUZZER, INIT_SOUND, 80);
  delay(160);
  tone(PIN_BUZZER, INIT_SOUND, 80);
  delay(160);
  tone(PIN_BUZZER, INIT_SOUND, 500);
  delay(500);
  tone(PIN_BUZZER, INIT_SOUND - 200, 500);
  delay(500);
  tone(PIN_BUZZER, INIT_SOUND - 110, 500);
  delay(500);
  tone(PIN_BUZZER, INIT_SOUND, 250);
  delay(400);
  tone(PIN_BUZZER, INIT_SOUND - 110, 100);
  delay(100);
  tone(PIN_BUZZER, INIT_SOUND, 500);
}

void superMario()
{
  tone(PIN_BUZZER, INIT_SOUND + 255, 100);
  delay(160);
  tone(PIN_BUZZER, INIT_SOUND + 255, 100);
  delay(320);
  tone(PIN_BUZZER, INIT_SOUND + 255, 100);
  delay(320);
  tone(PIN_BUZZER, INIT_SOUND, 140);
  delay(160);
  tone(PIN_BUZZER, INIT_SOUND + 255, 100);
  delay(320);
  tone(PIN_BUZZER, INIT_SOUND + 500, 200);
  delay(640);
  tone(PIN_BUZZER, INIT_SOUND - 250, 200);
}

void authSound()
{
  // soundToneCurr = HIGH_SOUND;
  // soundToneTarget = LOW_SOUND;
  int rand = (int)(random(5));
  switch (rand)
  {
  case 0:
  {
    powerRangers();
    break;
  }
  case 1:
  {
    doraemon();
    break;
  }
  case 2:
  {
    zelda();
    break;
  }
  case 3:
  {
    finalFantasy();
    break;
  }
  case 4:
  {
    superMario();
    break;
  }
  }
}

void unauthSound() {
  tone(PIN_BUZZER, 100, 100);
  delay(200);
  tone(PIN_BUZZER, 100, 500);
}

void scanSound() {

}

void buzzerLoop()
{
  if (currTime - soundToneLastTime > SOUND_MS)
  {
    soundToneLastTime = currTime;
    if (soundToneCurr != soundToneTarget)
    {
      tone(PIN_BUZZER, soundToneCurr, SOUND_MS);
      if (soundToneCurr < soundToneTarget)
      {
        if (soundToneTarget - soundToneCurr <= SOUND_STEP)
        {
          soundToneCurr = soundToneTarget;
        }
        else
        {
          soundToneCurr += SOUND_STEP;
        }
      }
      else if (soundToneCurr > soundToneTarget)
      {
        if (soundToneCurr - soundToneTarget <= SOUND_STEP)
        {
          soundToneCurr = soundToneTarget;
        }
        else
        {
          soundToneCurr -= SOUND_STEP;
        }
      }
    }
  }
}

void initRFID()
{
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

bool tryReadRFID()
{
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent())
  { //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial())
  { //Since a PICC placed get Serial and continue
    return false;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  for (uint8_t i = 0; i < 4; i++)
  { //
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}

void rfidLoop()
{
  if (currTime - rfidLastTime < RFID_MS)
  {
    return;
  }

  if (tryReadRFID())
  {
    sendBLEData(readCard);

    scanSound();
  }
}

void sendBLEData(const byte *data)
{
  Serial.print("<key>");
  for (int i = 0; i < 4; ++i)
  {
    Serial.print(data[i], HEX);
  }
  Serial.println("");
  delay(100);
}

void (*rebootFunc)(void) = 0; //reboot function @ address 0

void bleLoop()
{
  while (Serial.available())
  {
    byte b[1];
    Serial.readBytes(b, 1);
    char c = (char)(b[0]);
    if (c != '<')
    {
      Serial.print(c);
    }
    else
    {
      // ########## Read start tag ##########
      String command = Serial.readStringUntil('>');

      // ########## Read payload ##########
      String payload = Serial.readStringUntil(';');

      // settings commands
      if (command.equals("reboot"))
      {
        rebootFunc();
      }
      else if (command.equals("auth"))
      {
        Serial.println("RECV_AUTH");
        authSound();
      }
      else if (command.equals("unauth"))
      {
        Serial.println("RECV_UNAUTH");
        unauthSound();
      }
    }
  }
}

void initBLE()
{
  delay(1000);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    Serial.println("SER_WAIT");
  }
  Serial.println("SER_READY");

  pinMode(PIN_BUZZER, OUTPUT);

  initRFID();
  initBLE();

  delay(1000);
  Serial.println(F("<req_rfid_data>"));

  tone(PIN_BUZZER, INIT_SOUND, 100);
  delay(200);
  tone(PIN_BUZZER, INIT_SOUND, 100);
}

void loop()
{
  currTime = millis();

  rfidLoop();
  bleLoop();
  buzzerLoop();
}
