#include "CyrenaicaSDcard.h"

void setup(){
    
  Serial.begin(115200);
  while (!Serial) {
    ; 
  }

  pinMode(SD_PRESENCE_PIN, INPUT);
  

  Serial.print("PIN 34 Status: ");
  Serial.println(digitalRead(34));
  Serial.println();
  Serial.println();
  Serial.println();


  Serial.println("Serial started");
  Serial.println("Cyrenaica SDcard module test");

  Serial.println("Cyrenaica - IoT All rights reserved (c) 2021");

  SD.init();
  uint8_t cardType = SD.cardType();

  Serial.print("SD Card Type: ");

  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } 
  else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } 
  else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } 
  else 
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  SD.listDir(SD, "/", 0);
  SD.createDir(SD, "/Cyrenaica");
  SD.listDir(SD, "/", 0);
  SD.removeDir(SD, "/Cyrenaica");
  SD.listDir(SD, "/", 2);
  SD.writeFile(SD, "/Benghazi.txt", "Hello Benghazi\n");
  SD.appendFile(SD, "/Benghazi.txt", "Cyrenaica will be free\n");
  SD.readFile(SD, "/Benghazi.txt");
  SD.renameFile(SD, "/Benghazi.txt", "/Derna.txt");
  SD.readFile(SD, "/Derna.txt");
  SD.testFileIO(SD, "/Cyrenaica_test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  
}

void loop(){

  /* The loop is not needed for this test */
  SD.sdDetect(SD_PRESENCE_PIN);

}
