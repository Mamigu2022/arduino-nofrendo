/* Arduino Nofrendo
   Please check hw_config.h and display.cpp for configuration details
*/
#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>

#include "hw_config.h"

extern "C"
{
#include <nofrendo.h>
}

int16_t bg_color;
extern Arduino_TFT *gfx;
extern void display_begin();
int i;
void setup()
{
  Serial.begin(115200);
  pinMode(HW_CONTROLLER_GPIO_UP, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_DOWN, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_START, INPUT_PULLUP);
  // turn off WiFi
  esp_wifi_deinit();

  // disable Core 0 WDT
  TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
  esp_task_wdt_delete(idle_0);

  // start display
  display_begin();

  // filesystem defined in hw_config.h
  FILESYSTEM_BEGIN

  // find first rom file (*.nes)
  File root = filesystem.open("/");
  char *argv[1];
  String fiche[20];
  if (!root)
  {
    Serial.println("Filesystem mount failed! Please check hw_config.h settings.");
    gfx->println("Filesystem mount failed! Please check hw_config.h settings.");
  }
  else
  {
    bool foundRom = false;
    int contador, borr ;
    File file = root.openNextFile();
    while (file)
    {
      if (file.isDirectory())
      {
        // skip
      }
      else
      {
        char *filename = (char *)file.name();
        int8_t len = strlen(filename);
        if (strstr(strlwr(filename + (len - 4)), ".nes"))
        {
          //contador++;
          //foundRom = true;
          char fullFilename[256];
          sprintf(fullFilename, "%s%s", FSROOT, filename);
          gfx->setRotation(2);
          argv[0] = fullFilename;
          fiche[i] = fullFilename;
          gfx->println(argv[0]);
          i++;

          //break;
        }
      }

      file = root.openNextFile();
      

    }
    while (!foundRom)
    {
      if (digitalRead(HW_CONTROLLER_GPIO_UP)==0 && (contador >0)) contador = contador - 8;
      if (digitalRead(HW_CONTROLLER_GPIO_DOWN)==0 && (contador < 8 * (i-1) ))contador += 8;

      Serial.println(borr);

      gfx->setTextColor((gfx->color565(255, 255, 255)), (gfx->color565(24, 28, 24)));
      gfx->setCursor(0, borr);
      gfx->print(fiche[borr / 8]);
      //gfx->writeFillRect(200,contador,32,8,0);
      gfx->setTextColor((gfx->color565(24, 28, 24)), (gfx->color565(255, 255, 255)));
      gfx->setCursor(0, contador);
      gfx->print(fiche[contador / 8]);
      //Serial.println(fiche[contador / 8]);
      borr = contador;
      delay(100);
      if (digitalRead(HW_CONTROLLER_GPIO_START)==0  ) foundRom = true;
        
        Serial.print(digitalRead(HW_CONTROLLER_GPIO_START));

        // Serial.println("Failed to find rom file, please copy rom file to data folder and upload with \"ESP32 Sketch Data Upload\"");
        //argv[0] = "/";
        //char *fichero[1];

        fiche[contador / 8].toCharArray(argv[0], fiche[contador / 8].length() + 1);
     } 


      Serial.println("NoFrendo start!\n");
      Serial.print(argv[0]);
      nofrendo_main(1, argv);
      Serial.println("NoFrendo end!\n");
    }
}

    void loop()
    {
    }
