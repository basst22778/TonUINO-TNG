#include "settings.hpp"

#include "constants.hpp"
#include "logger.hpp"

namespace {

const int startAddressAdminSettings = sizeof(folderSettings::folder) * 100;

}

void Settings::writeByteToFlash(uint16_t address, uint8_t value) {
  EEPROM_put(address, value);
}

uint8_t Settings::readByteFromFlash(uint16_t address) {
  return EEPROM.read(address);
}

void Settings::clearEEPROM() {
  LOG(settings_log, s_info, F("clearEEPROM"));
  for (uint16_t i = 0; i < sizeof(folderSettings::folder) * 100 + sizeof(this); i++) {
    writeByteToFlash(i, 0);
  }
}

void Settings::writeSettingsToFlash() {
  LOG(settings_log, s_debug, F("writeSettingsToFlash"));
  EEPROM_put(startAddressAdminSettings, *this);
}

void Settings::resetSettings() {
  LOG(settings_log, s_debug, F("resetSettings"));
  cookie               = cardCookie;
  version              =  2;
  maxVolume            = 25;
  minVolume            =  5;
  initVolume           = 15;
  eq                   =  1;
  locked               = false;
  standbyTimer         =  0;
  invertVolumeButtons  = true;
  shortCuts[0]         =  { 0, pmode_t::none, 0, 0 };
  shortCuts[1]         =  { 0, pmode_t::none, 0, 0 };
  shortCuts[2]         =  { 0, pmode_t::none, 0, 0 };
  shortCuts[3]         =  { 0, pmode_t::none, 0, 0 };
  adminMenuLocked      =  0;
  adminMenuPin[0]      =  1;
  adminMenuPin[1]      =  1;
  adminMenuPin[2]      =  1;
  adminMenuPin[3]      =  1;
  pauseWhenCardRemoved = false;
#ifdef NeoPixels
  neoPixelHue = 0;
  neoPixelBaseValue = 95;
  neoPixelNightLightValue = 255;
#endif

  writeSettingsToFlash();
}

void Settings::migrateSettings(int oldVersion) {
  if (oldVersion == 1) {
    LOG(settings_log, s_info, F("migradeSettings 1 -> 2"));
    version              = 2;
    adminMenuLocked      = 0;
    adminMenuPin[0]      = 1;
    adminMenuPin[1]      = 1;
    adminMenuPin[2]      = 1;
    adminMenuPin[3]      = 1;
    pauseWhenCardRemoved = false;
    writeSettingsToFlash();
  }
  const unsigned int t = pauseWhenCardRemoved;
  if (t == 0xff)
    pauseWhenCardRemoved = false;
}

void Settings::loadSettingsFromFlash() {
  LOG(settings_log, s_debug, F("loadSettingsFromFlash"));
  EEPROM_get(startAddressAdminSettings, *this);
  if (cookie != cardCookie)
    resetSettings();
  migrateSettings(version);

  LOG(settings_log, s_info, F("Version: "                ), version);
  LOG(settings_log, s_info, F("Max Vol: "                ), maxVolume);
  LOG(settings_log, s_info, F("Min Vol: "                ), minVolume);
  LOG(settings_log, s_info, F("Init Vol: "               ), initVolume);
  LOG(settings_log, s_info, F("EQ: "                     ), eq);
  LOG(settings_log, s_info, F("Locked: "                 ), locked);
  LOG(settings_log, s_info, F("Sleep Timer: "            ), standbyTimer);
  LOG(settings_log, s_info, F("Inverted Vol Buttons: "   ), invertVolumeButtons);
  LOG(settings_log, s_info, F("Admin Menu locked: "      ), adminMenuLocked);
  LOG(settings_log, s_info, F("Admin Menu Pin: "         ), adminMenuPin[0], adminMenuPin[1], adminMenuPin[2], adminMenuPin[3]);
  LOG(settings_log, s_info, F("Pause when card removed: "), pauseWhenCardRemoved);
  #ifdef NeoPixels
  LOG(settings_log, s_info, F("NeoPixel Hue: "), neoPixelHue);
  LOG(settings_log, s_info, F("NeoPixel Value: "), neoPixelBaseValue);
  LOG(settings_log, s_info, F("NeoPixel night light Value: "), neoPixelNightLightValue);
  #endif
}

void Settings::writeFolderSettingToFlash(uint8_t folder, uint16_t track) {
  if (folder < 100)
    writeByteToFlash(folder, min(track, 0xffu));
}

uint16_t Settings::readFolderSettingFromFlash(uint8_t folder) {
  return (folder < 100)? readByteFromFlash(folder) : 0;
}
