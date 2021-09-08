#ifndef SYRENAICAESP32SD_H
#define SYRENAICAESP32SD_H

/* 
    Cyrenaica-IoT
    188 Avenue Saint Exupéry 69500 Bron France
    Email: cyrenaica.iot@gmail.com
    Tel: +3363128877
    By Akram Latrash

    CyrenaicaSDcard.h: Cyrenaica SDcard Module with SD presence detection

    Copyright (c) 2021 Akram Latrash. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    **************************************************************************
                PINS MAP:
        ***********************
        |   ESP32   |    SD   |
        ***********************
        |   GPIO5  ----- CS   | 
        |   GPIO18 ----- SCK  |
        |   GPIO19 ----- MISO |
        |   GPIO23 ----- MOSI |  
        |   5V --------- 5V   |
        |                     |
        |   or you can use    |
        |                     |
        |   3.3V ------- 3.3V |
        ***********************

        GITHUB: https://github.com/Cyrenaica-iot

            
 */

#include "config.h"

namespace SDCARD /* namespace SDCARD */ {

    using namespace std;
    using namespace fs;

    class ESPSDFS : public FS /* class ESPSDFS */ {

        protected:
        uint8_t _pdrv;

        public:
        public:
        char* _sd_type_text;
        uint64_t _sd_size;
        uint64_t _sd_total_size;
        uint64_t _sd_used_size;
        uint64_t _sd_free_size;

        ESPSDFS(); /* Public Constructer */
        ESPSDFS(FSImplPtr impl); /* Public Constructer (overload) */
        ~ESPSDFS(); /* Public Desstructer */

        public:

        bool begin(uint8_t ssPin=SS, SPIClass &spi=SPI, uint32_t frequency=4000000, const char * mountpoint="/sd", uint8_t max_files=5);
        CF::s_sd_status_t init();
        CF::sd_card_type_t cardType();
        CF::s_sd_status_t detectType();
        uint64_t cardSize();
        uint64_t totalBytes();
        uint64_t usedBytes();
        CF::s_sd_status_t sdCardInfo();
        void sdDetect(int pin);

        void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
        void createDir(fs::FS &fs, const char * path);
        void removeDir(fs::FS &fs, const char * path);
        void readFile(fs::FS &fs, const char * path);
        void writeFile(fs::FS &fs, const char * path, const char * message);
        void appendFile(fs::FS &fs, const char * path, const char * message);
        void renameFile(fs::FS &fs, const char * path1, const char * path2);
        void deleteFile(fs::FS &fs, const char * path);
        void testFileIO(fs::FS &fs, const char * path);
        

        protected: /* Protected Variables */
        CF Config;
    };
}

extern SDCARD::ESPSDFS SD;

using namespace SDCARD;

typedef SDCARD::File        SDFile;
typedef SDCARD::ESPSDFS     SDFileSystemClass;
#define SDFileSystem        DSD



#endif