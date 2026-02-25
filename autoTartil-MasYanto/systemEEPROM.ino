void saveToEEPROM() {
  //Serial.println("Menyimpan data ke EEPROM...");
  int addr = 0;

  for (int h = 0; h < HARI_TOTAL; h++) {
    for (int w = 0; w < WAKTU_TOTAL; w++) {
      EEPROM.put(addr, jadwal[h][w]);
      addr += sizeof(WaktuConfig);
    }
  }

  for (int i = 0; i < MAX_FILE; i++) {
    EEPROM.put(addr, durasiAdzan[i]);
    addr += sizeof(uint16_t);
  }

  for (int f = 0; f < MAX_FOLDER; f++) {
    for (int i = 0; i < MAX_FILE; i++) {
      EEPROM.put(addr, durasiTartil[f][i]);
      addr += sizeof(uint16_t);  // perbaikan: sebelumnya kamu baca uint16_t, padahal simpan uint32_t
      
    }
  }

  EEPROM.write(addr, volumeDFPlayer);
  addr += sizeof(volumeDFPlayer);

  for (int i = 0; i < WAKTU_TOTAL; i++) {
    EEPROM.write(addr++, jamSholat[i]);
    EEPROM.write(addr++, menitSholat[i]);
  }

  EEPROM.write(addr++, autoTartilEnable ? 1 : 0);

  EEPROM.write(addr++, voiceClock ? 1 : 0);

  // Simpan password
  for (int i = 0; i < PASSWORD_LEN; i++) {
    EEPROM.write(addr++, password[i]);
  }

#if defined(ESP8266) || defined(ESP32)
  EEPROM.commit();  // WAJIB untuk ESP
#endif

}

void loadFromEEPROM() {
  int addr = 0;

  for (int h = 0; h < HARI_TOTAL; h++) {
    for (int w = 0; w < WAKTU_TOTAL; w++) {
      EEPROM.get(addr, jadwal[h][w]);
      addr += sizeof(WaktuConfig);
      //============ DEBUG =============//
      Serial.print("HR:"); Serial.print(h);
      Serial.print(" W"); Serial.print(w);
      Serial.print(" Aktif:"); Serial.print(jadwal[h][w].aktif);
      Serial.print(" Adzan:"); Serial.print(jadwal[h][w].aktifAdzan);
      Serial.print(" FileAdzan:"); Serial.print(jadwal[h][w].fileAdzan);
      Serial.print(" TartilDulu:"); Serial.print(jadwal[h][w].tartilDulu);
      Serial.print(" Folder:"); Serial.print(jadwal[h][w].folder);
      Serial.print(" List:");
      Serial.print(jadwal[h][w].list[0]); Serial.print("-");
      Serial.print(jadwal[h][w].list[1]); Serial.print("-");
      Serial.print(jadwal[h][w].list[2]); Serial.print("-");
      Serial.print(jadwal[h][w].list[3]); Serial.print("-");
      Serial.println(jadwal[h][w].list[4]);
      //================================/*/
    }
  }

  for (int i = 0; i < MAX_FILE; i++) {
    EEPROM.get(addr, durasiAdzan[i]);
    addr += sizeof(uint16_t);
    //============ DEBUG =============//
 Serial.print("adzan["); Serial.print(i);
 Serial.print("] = "); Serial.println(durasiAdzan[i]);
    //================================//
  }

  for (int f = 0; f < MAX_FOLDER; f++) {
    for (int i = 0; i < MAX_FILE; i++) {
      EEPROM.get(addr, durasiTartil[f][i]);
      addr += sizeof(uint16_t);  // perbaikan: harus cocok dengan penyimpanan
      //============ DEBUG =============//
   Serial.print("Tartil["); Serial.print(f); Serial.print("]["); Serial.print(i);
   Serial.print("] = "); Serial.println(durasiTartil[f][i]);
      //================================//
    }
  }

  EEPROM.get(addr, volumeDFPlayer);
  addr += sizeof(volumeDFPlayer);
  //============ DEBUG =============//
  Serial.print("VOL:"); Serial.println(volumeDFPlayer);
  //================================//
  
  for (int i = 0; i < WAKTU_TOTAL; i++) {
    EEPROM.get(addr, jamSholat[i]); addr += sizeof(uint8_t);
    EEPROM.get(addr, menitSholat[i]); addr += sizeof(uint8_t);
    //============ DEBUG =============//
    Serial.print("jamSholat["); Serial.print(i);
    Serial.print("] = "); Serial.println(jamSholat[i]);
    Serial.print("menitSholat["); Serial.print(i);
    Serial.print("] = "); Serial.println(menitSholat[i]);
    //================================/*/
  }

  // Baca status Auto Tartil
  autoTartilEnable = EEPROM.read(addr++) == 1;
  Serial.print("autoTartilEnable:");
  Serial.println(autoTartilEnable);

  voiceClock = EEPROM.read(addr++) == 1;
  Serial.print("voiceClock:");
  Serial.println(voiceClock);

  // Baca password
  for (int i = 0; i < PASSWORD_LEN; i++) {
    password[i] = EEPROM.read(addr++);
  }
  password[PASSWORD_LEN - 1] = '\0'; // safety null-terminator
  Serial.print("password:");
  Serial.println(password);
}
