#ifndef CONFIG_H
#define CONFIG_H

/*#include <Arduino.h>
#include <iostream>
#include "vfs_api.h"
#include "sd_diskio.h"
#include "ff.h"
#include <DNSServer.h>
#include <WiFi.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "sd_defines.h"
#include <ArduinoJson.h>*/


#include <Arduino.h>
#include <iostream>
#include "vfs_api.h"
#include "sd_diskio.h"
#include "sd_defines.h"
#include "ff.h"
#include <SPI.h>
#include "config.h"

namespace configuration /* Config namespase */ {

    class CF /* Config class */ {
        public:

        CF(); /* Constructer */
        #define SD_PRESENCE_PIN 34

        #ifndef SD_PRESNCE_LED
        #define SD_PRESNCE_LED 2
        #endif

        #define SD_LED_INVERS false

        /********************************* SD CARD INFO **********************************************/
        typedef enum{
            SD_OK                       = 0,
            SD_FAIL_OPEN_DIR            = 1,
            SD_NOT_DIR                  = 2,
            SD_FAIL_CREATE_RIR          = 3,
            SD_FAIL_REMOVE_DIR          = 4,
            SD_FAIL_OPEN_FILE_READ      = 5,
            SD_FAIL_OPEN_FILE_WRITE     = 6,
            SD_FAIL_FILE_WRITE          = 7,
            SD_FAIL_OPEN_FILE_FOR_AAPND = 8,
            SD_FAIL_FILE_APPEND         = 9,
            SD_FAIL_FILE_RENAME         = 10,
            SD_FAIL_FILE_DEL            = 11,
            SD_FAIL_MOUNTING_CARD       = 12,
            SD_FAIL_TYPE_UNKNOXN        = 13
        } s_sd_status_t;

        typedef enum{
            CARD_NONE    = 255,
            CARD_MMC     = 1,
            CARD_SD      = 2,
            CARD_SDHC    = 3,
            CARD_UNKNOUN = 4

        } sd_card_type_t;

        /* SD card information */
        typedef struct{
            sd_card_type_t sys_sd_card_type_info;
            char* sys_sd_card_detected_type;
            uint64_t sys_sd_card_size_info;
            uint64_t sys_sd_card_total_size_info;
            uint64_t sys_sd_card_used_size_info;
            uint64_t sys_sd_card_free_size_info;
        }i_sd_card_info;

        /********************************* SD CARD INFO **********************************************/

        i_sd_card_info    __sd_card_info;
    };
}

using configuration::CF;
extern configuration::CF CONFILE;

//Livebox-Akram
//DGW9Rnf6h4SGHArsMJ

#endif /* __CONFIG__ */