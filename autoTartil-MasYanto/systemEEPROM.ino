void saveToEEPROM() {
  //Serial.println("Menyimpan data ke EEPROM...");
  uint16_t addr = 0;

  for (uint8_t h = 0; h < HARI_TOTAL; h++) {
    for (uint8_t w = 0; w < WAKTU_TOTAL; w++) {
      EEPROM.put(addr, jadwal[h][w]);
      addr += sizeof(WaktuConfig);
    }
  }

  for (uint8_t i = 0; i < MAX_FILE; i++) {
    EEPROM.put(addr, durasiAdzan[i]);
    addr += sizeof(uint16_t);
  }

  for (uint8_t f = 0; f < MAX_FOLDER; f++) {
    for (uint8_t i = 0; i < MAX_FILE; i++) {
      EEPROM.put(addr, durasiTartil[f][i]);
      addr += sizeof(uint16_t);  // perbaikan: sebelumnya kamu baca uint16_t, padahal simpan uint32_t
      
    }
  }

  EEPROM.put(addr, volumeDFPlayer);
  addr += sizeof(volumeDFPlayer);

  for (uint8_t i = 0; i < WAKTU_TOTAL; i++) {
    EEPROM.put(addr++, jamSholat[i]);
    EEPROM.put(addr++, menitSholat[i]);
  }

  EEPROM.put(addr++, autoTartilEnable ? 1 : 0);

  EEPROM.put(addr++, voiceClock ? 1 : 0);

  // Simpan password
  for (uint8_t i = 0; i < PASSWORD_LEN; i++) {
    EEPROM.put(addr++, password[i]);
  }

#if defined(ESP8266) || defined(ESP32)
  EEPROM.commit();  // WAJIB untuk ESP
#endif

}

void loadFromEEPROM() {
  uint16_t addr = 0;

  for (uint8_t h = 0; h < HARI_TOTAL; h++) {
    for (uint8_t w = 0; w < WAKTU_TOTAL; w++) {
      EEPROM.get(addr, jadwal[h][w]);
      addr += sizeof(WaktuConfig);
      //============ DEBUG =============//
    #if DEBUG
      Serial.print(F("HR:")); Serial.print(h);
      Serial.print(F(" W")); Serial.print(w);
      Serial.print(F(" Aktif:")); Serial.print(jadwal[h][w].aktif);
      Serial.print(F(" Adzan:")); Serial.print(jadwal[h][w].aktifAdzan);
      Serial.print(F(" FileAdzan:")); Serial.print(jadwal[h][w].fileAdzan);
      Serial.print(F(" TartilDulu:")); Serial.print(jadwal[h][w].tartilDulu);
      Serial.print(F(" Folder:")); Serial.print(jadwal[h][w].folder);
      Serial.print(F(" List:"));
      Serial.print(jadwal[h][w].list[0]); Serial.print(F("-"));
      Serial.print(jadwal[h][w].list[1]); Serial.print(F("-"));
      Serial.print(jadwal[h][w].list[2]); Serial.print(F("-"));
      Serial.print(jadwal[h][w].list[3]); Serial.print(F("-"));
      Serial.println(jadwal[h][w].list[4]);
    #endif
      //================================/*/
    }
  }

  for (uint8_t i = 0; i < MAX_FILE; i++) {
    EEPROM.get(addr, durasiAdzan[i]);
    addr += sizeof(uint16_t);
    //============ DEBUG =============//
    #if DEBUG
      Serial.print(F("adzan[")); Serial.print(i);
      Serial.print(F("] = ")); Serial.println(durasiAdzan[i]);
    #endif
    //================================//
  }

  for (uint8_t f = 0; f < MAX_FOLDER; f++) {
    for (uint8_t i = 0; i < MAX_FILE; i++) {
      EEPROM.get(addr, durasiTartil[f][i]);
      addr += sizeof(uint16_t);  // perbaikan: harus cocok dengan penyimpanan
      //============ DEBUG =============//
      #if DEBUG
        Serial.print(F("Tartil[")); Serial.print(f); Serial.print(F("][")); Serial.print(i);
        Serial.print(F("] = ")); Serial.println(durasiTartil[f][i]);
      #endif
      //================================//
    }
  }

  EEPROM.get(addr, volumeDFPlayer);
  addr += sizeof(volumeDFPlayer);
  
 
  for (uint8_t i = 0; i < WAKTU_TOTAL; i++) {
    EEPROM.get(addr, jamSholat[i]); addr += sizeof(uint8_t);
    EEPROM.get(addr, menitSholat[i]); addr += sizeof(uint8_t);
    //============ DEBUG =============//
    #if DEBUG
      Serial.print(F("jamSholat[")); Serial.print(i);
      Serial.print(F("] = ")); Serial.println(jamSholat[i]);
      Serial.print(F("menitSholat[")); Serial.print(i);
      Serial.print(F("] = ")); Serial.println(menitSholat[i]);
    #endif
    //================================/*/
  }

  // Baca status Auto Tartil
  autoTartilEnable = EEPROM.read(addr++) == 1;

  voiceClock = EEPROM.read(addr++) == 1;
  
  // Baca password
  for (uint8_t i = 0; i < PASSWORD_LEN; i++) {
    password[i] = EEPROM.read(addr++);
  }
  password[PASSWORD_LEN - 1] = '\0'; // safety null-terminator
  
  #if DEBUG
    Serial.print(F("password:"));
    Serial.println(password);
    Serial.print(F("VOL:")); Serial.println(volumeDFPlayer);
    Serial.print(F("autoTartilEnable:"));
    Serial.println(autoTartilEnable);
    Serial.print(F("voiceClock:"));
    Serial.println(voiceClock);
  #endif
}
