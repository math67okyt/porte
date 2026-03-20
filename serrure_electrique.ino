#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LOCK_PIN 7   // Broche pour relais/servo
#define LED_VERTE 4
#define LED_ROUGE 5

MFRC522 rfid(SS_PIN, RST_PIN);

// UID autorisé 
byte authorizedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMODE(LED_VERTE, OUTPUT);

  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW); // serrure fermée

  Serial.println("Systeme RFID prêt");
}

void loop() {

  // Attente d'une carte
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID : ");
  
  // Affichage UID
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Vérification UID
  if (checkUID()) {
    Serial.println("Accès autorisé");
    digitalWrite(LED_VERTE, HIGH);
    ouvrirSerrure();
  } else {
    Serial.println("Accès refusé");
    digitalWrite(LED_ROUGE, HIGH);
  }

  rfid.PICC_HaltA();
}

// Vérifie si badge est autorisé
bool checkUID() {

  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}

// Ouvre la serrure 5 secondes
void ouvrirSerrure() { 
  digitalWrite(LOCK_PIN, HIGH); // ouvrir
  delay(5000);                 // 5 s
  digitalWrite(LOCK_PIN, LOW);  // fermer
}
