#include "persistent_settings.h"
#include "eeprom.h"
#include "display.h"

// Virtual addresses for each stored variable. The EEPROM emulation library
// uses these as keys; the actual Flash locations are managed internally.
// VirtAddVarTab is referenced as extern by eeprom.c and must be defined here.
#define EEPROM_BRIGHTNESS_VIRT_ADDR  0x1000
#define EEPROM_FLIP_SCREEN_VIRT_ADDR 0x1001

uint16_t VirtAddVarTab[NB_OF_VAR] = {0x1000, 0x1001, 0x1002};

void PersistentSettings_Init(void) {
    HAL_FLASH_Unlock();
    EE_Init();

    // Read returns 0 on success; if a variable has never been written the read
    // fails and the local default is left in place
    uint16_t saved_brightness = 25;
    if (EE_ReadVariable(EEPROM_BRIGHTNESS_VIRT_ADDR, &saved_brightness) == 0) {
        if (saved_brightness <= 100) {
            backlight_level = saved_brightness;
        }
    }

    uint16_t saved_flip_screen = 0;
    if (EE_ReadVariable(EEPROM_FLIP_SCREEN_VIRT_ADDR, &saved_flip_screen) == 0) {
        flip_screen_state = saved_flip_screen;
    }

    HAL_FLASH_Lock();
}

void action_save_brightness_to_eeprom(void) {
    HAL_FLASH_Unlock();
    EE_WriteVariable(EEPROM_BRIGHTNESS_VIRT_ADDR, (uint16_t)backlight_level);
    EE_WriteVariable(EEPROM_FLIP_SCREEN_VIRT_ADDR, (uint16_t)flip_screen_state);
    HAL_FLASH_Lock();
}
