#include "CyrenaicaSDcard.h"

using namespace SDCARD;

ESPSDFS::ESPSDFS(FSImplPtr impl): FS(impl), _pdrv(0xFF) {}
ESPSDFS::~ESPSDFS(){} /* Public Desstructer */


#define SD_CS 5 // Define CS pin for the SD card module

bool ESPSDFS::begin(uint8_t ssPin, SPIClass &spi, uint32_t frequency, const char * mountpoint, uint8_t max_files){

    if(_pdrv != 0xFF) {

        return true;

    }

    spi.begin();

    _pdrv = sdcard_init(ssPin, &spi, frequency);

    if(_pdrv == 0xFF) {

        return false;

    }

    if(!sdcard_mount(_pdrv, mountpoint, max_files, true)){

        sdcard_unmount(_pdrv);
        sdcard_uninit(_pdrv);
        _pdrv = 0xFF;

        return false;

    }

    _impl->mountpoint(mountpoint);
    return true;
}

CF::s_sd_status_t ESPSDFS::init(){

  pinMode(SD_PRESENCE_PIN, INPUT);
  pinMode(SD_PRESNCE_LED, OUTPUT);

  #if SD_LED_INVERS
  digitalWrite(SD_PRESNCE_LED, HIGH);
  #else
  digitalWrite(SD_PRESNCE_LED, LOW);
  #endif

  if(!SD.begin(SD_CS)){

    Serial.println("Card mount Failed");
    return CF::SD_FAIL_MOUNTING_CARD;
  }
    
  Serial.println("Card mounted successfully.");
  return CF::SD_OK;
}

CF::sd_card_type_t ESPSDFS::cardType(){

    if(_pdrv == 0xFF) {

        return CF::CARD_NONE;

    }

    return CF::sd_card_type_t(_pdrv);
}

CF::s_sd_status_t ESPSDFS::detectType(){

    CF::sd_card_type_t type = cardType();

    switch (type){

    case CARD_NONE:
      _sd_type_text =  (char*) "No SD card attached";
    break;

    case CARD_SD:
      _sd_type_text = (char*) "SDSC";
    break;

    case CARD_MMC:
      _sd_type_text = (char*) "MMC";
    break;

    case CARD_SDHC:
      _sd_type_text = (char*) "SDHC";
    break;

    default:
      _sd_type_text = (char*) "UNKNOWN";
      return CF::SD_FAIL_TYPE_UNKNOXN;
    break;


    }

    return CF::SD_OK;
}

uint64_t ESPSDFS::cardSize(){

    if(_pdrv == 0xFF) {

        return 0;

    }

    size_t sectors = sdcard_num_sectors(_pdrv);
    size_t sectorSize = sdcard_sector_size(_pdrv);

    return (uint64_t)sectors * sectorSize;
}

uint64_t ESPSDFS::totalBytes(){

	FATFS* fsinfo;
	DWORD fre_clust;

	if(f_getfree("0:",&fre_clust,&fsinfo)!= 0) return 0;

    uint64_t size = ((uint64_t)(fsinfo->csize))*(fsinfo->n_fatent - 2)

    #if _MAX_SS != 512

        *(fsinfo->ssize);

    #else

        *512;

    #endif
    _sd_total_size = size;
	return size;
}

uint64_t ESPSDFS::usedBytes(){

	FATFS* fsinfo;
	DWORD fre_clust;

	if(f_getfree("0:",&fre_clust,&fsinfo)!= 0) return 0;

	  uint64_t size = ((uint64_t)(fsinfo->csize))*((fsinfo->n_fatent - 2) - (fsinfo->free_clst))
    
    #if _MAX_SS != 512

        *(fsinfo->ssize);

    #else

        *512;

    #endif

	return size;
}

void ESPSDFS::sdDetect(int pin){

  int SD_PRESENT = digitalRead(pin);

  #if SD_LED_INVERS
  
    if(SD_PRESENT == LOW){
      digitalWrite(SD_PRESNCE_LED, LOW);
    }
    else
    {
      digitalWrite(SD_PRESNCE_LED, HIGH);
    }
  
  #else

    if(SD_PRESENT == LOW){
        digitalWrite(SD_PRESNCE_LED, HIGH);
      }
      else
      {
        digitalWrite(SD_PRESNCE_LED, LOW);
      }
  #endif
  
  
}

CF::s_sd_status_t ESPSDFS::sdCardInfo(){

    Config.__sd_card_info.sys_sd_card_type_info = cardType();

    if(detectType() == CF::SD_OK){
        Config.__sd_card_info.sys_sd_card_detected_type = _sd_type_text;
    }

    uint64_t cardSize      = SD.cardSize() / 1048576 / 1024;  //GB
    uint64_t totalCardSize = SD.totalBytes() / (1024 * 1024); //MB
    uint64_t usedCardSize  = SD.usedBytes() / (1024 * 1024);  //MB
    uint64_t freeCardSize  = (totalCardSize - usedCardSize);  //MB

    _sd_size       = cardSize;
    _sd_total_size = totalCardSize;
    _sd_used_size  = usedCardSize;
    _sd_free_size  = freeCardSize;

    Config.__sd_card_info.sys_sd_card_size_info       = _sd_size;
    Config.__sd_card_info.sys_sd_card_total_size_info = _sd_total_size;
    Config.__sd_card_info.sys_sd_card_used_size_info  = _sd_used_size;
    Config.__sd_card_info.sys_sd_card_free_size_info  = _sd_free_size;

    Serial.print("Mounted SD Card Type: ");
    Serial.println(Config.__sd_card_info.sys_sd_card_detected_type);

    Serial.printf("SD Card Size: %llu GB\n", cardSize);       
    Serial.printf("SD Used Card Size: %llu MB\n", usedCardSize);           
    Serial.printf("SD Free Card Size: %llu MB\n", freeCardSize);

    return CF::SD_OK;

}

void ESPSDFS::listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
      return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } 
    else 
    {
              
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }

    file = root.openNextFile();
  }
}

void ESPSDFS::createDir(fs::FS &fs, const char * path){ 
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  }
  else 
  {
    Serial.println("mkdir failed");
  }
}

void ESPSDFS::removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
      Serial.println("Dir removed");
  } 
  else 
  {
    Serial.println("rmdir failed");
  }
}

void ESPSDFS::readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void ESPSDFS::writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
      Serial.println("Failed to open file for writing");
      return;
  }
  if(file.print(message)){
      Serial.println("File written");
  } 
  else {
    Serial.println("Write failed");
  }
  file.close();
}

void ESPSDFS::appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    Serial.println("Message appended");
  } 
  else 
  {
    Serial.println("Append failed");
  }
  file.close();
}

void ESPSDFS::renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } 
  else 
  {
    Serial.println("Rename failed");
  }
}

void ESPSDFS::deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } 
  else 
  {
    Serial.println("Delete failed");
  }
}

void ESPSDFS::testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } 
  else 
  {
    Serial.println("Failed to open file for reading");
  }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
