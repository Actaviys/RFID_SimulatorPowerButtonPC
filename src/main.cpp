#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define SAVE_BUTTON_PIN 4
#define DELETE_BUTTON_PIN 2
#define BUTT_GND 3 

#define LED_OK 6
#define LED_STATUS 8
#define LED_VCC 7

#define SIMUL_BTN 14

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;


const int MAX_UIDS = 20;
String savedUIDs[MAX_UIDS] = {"3505713"};
int savedCount = 1;

bool saveMode = false;
bool deleteMode = false;
bool lastSaveButtonState = HIGH;
bool lastDeleteButtonState = HIGH;

bool chek_modes = false;
bool flag_led_ok = false;
bool flag_led_status_card = false;

bool flag_push_btn = false; // //

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(SAVE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DELETE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTT_GND, OUTPUT); digitalWrite(BUTT_GND, 0);

  pinMode(LED_OK, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_VCC, OUTPUT); digitalWrite(LED_VCC, 1);

  pinMode(SIMUL_BTN, OUTPUT);

  // digitalWrite(LED_OK, 0);
  // digitalWrite(LED_LOCKING, 0);

  Serial.println("System ready.");
  Serial.println("Press SAVE button to toggle save mode.");
  Serial.println("Press DELETE button to toggle delete mode.");
}




void checkButtons() {
  bool saveBtn = digitalRead(SAVE_BUTTON_PIN);
  bool deleteBtn = digitalRead(DELETE_BUTTON_PIN);

  if (lastSaveButtonState == HIGH && saveBtn == LOW) {
    saveMode = !saveMode;
    deleteMode = false;
    Serial.print("Save mode: ");
    Serial.println(saveMode ? "ON" : "OFF");
  }

  if (lastDeleteButtonState == HIGH && deleteBtn == LOW) {
    deleteMode = !deleteMode;
    saveMode = false;
    Serial.print("Delete mode: ");
    Serial.println(deleteMode ? "ON" : "OFF");
  }

  lastSaveButtonState = saveBtn;
  lastDeleteButtonState = deleteBtn;
}



// ///////
unsigned long ledStartTime = 0;
void triggerLED() {
  ledStartTime = millis();
  chek_modes = true;
}
// ///////



/////
String printHex(byte *buffer, byte bufferSize) {
  String str_byte;
  for (byte i = 0; i < bufferSize; i++) {
    str_byte += String(buffer[i], HEX);
  }
  return str_byte;
}

bool uidExists(String uid, int &index) {
  for (int i = 0; i < savedCount; i++) {
    if (savedUIDs[i] == uid) {
      index = i;
      return true;
    }
  }
  return false;
}

void removeUID(int index) {
  for (int i = index; i < savedCount - 1; i++) {
    savedUIDs[i] = savedUIDs[i + 1];
  }
  savedCount--;
  digitalWrite(LED_STATUS, 0);
  delay(300);
  digitalWrite(LED_STATUS, 1);
  delay(300);
  digitalWrite(LED_STATUS, 0);
}

void readRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  String uidStr = printHex(rfid.uid.uidByte, rfid.uid.size);
  
  int index;
  if (saveMode && savedCount < MAX_UIDS && !uidExists(uidStr, index)) {
    savedUIDs[savedCount++] = uidStr;
    Serial.println("UID saved.");
    saveMode = false;
    flag_led_status_card = true;
    triggerLED();//////
  }
  else if (deleteMode && uidExists(uidStr, index)) {
    removeUID(index);
    Serial.println("UID deleted.");
    deleteMode = false;
    flag_led_status_card = true;
    triggerLED();//////
  }
  else if (uidExists(uidStr, index) && saveMode || deleteMode){
    Serial.print(uidStr);
    if (saveMode){
      Serial.println(" - UID already saved!");
    }
    if (deleteMode){
      Serial.println(" - UID missing");
    }
    saveMode = false;
    deleteMode = false;
  }

  else if (uidExists(uidStr, index)) {
    flag_led_ok = true;
    flag_push_btn = true;
    triggerLED();///////
    Serial.print(uidStr);
    Serial.println(" => UID confirmed.");
  }
  else if (!uidExists(uidStr, index)) {
    Serial.print("RFID Tag UID is missing!!: "); // 
    Serial.println(uidStr);
  }
  else if (uidExists(uidStr, index)) {
    flag_led_ok = true;
    flag_push_btn = true;
    triggerLED();///////
    Serial.print(uidStr);
    Serial.println(" => UID standard confirmed.");
  }
  
  Serial.println("====================================");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
/////////




unsigned long lastRFIDCheck_time = 0;
const unsigned long LED_DURATION = 300; //  мілісекунд // Затримка симуляції натиску
void loop() {
  if (millis() - lastRFIDCheck_time >= 100) { // 
    checkButtons();
    readRFID();
    lastRFIDCheck_time = millis();
  }
  digitalWrite(SIMUL_BTN, flag_push_btn);
  digitalWrite(LED_OK, !flag_led_ok);
  digitalWrite(LED_STATUS, !flag_led_status_card);


  if (chek_modes && millis() - ledStartTime >= LED_DURATION) {
    flag_led_ok = false;
    flag_led_status_card = false;
    flag_push_btn = false;
    chek_modes = false;
  }
}