void readSensor(){

  uint32_t currentMillis = millis();
  
  // Variabel Kontrol Tampilan LCD
//  static uint32_t volumeDisplayMillis = 0;
  constexpr uint32_t lastReadTime = 0;
  constexpr uint8_t readInterval = 100;
  
  // Membaca potensiometer tanpa menghentikan program lain
  if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;

    uint32_t sum = 0;
    // 50 sampel tanpa delay di dalam loop for
    for (uint8_t i = 0; i < 60; i++) {
        sum += analogRead(potPin);
    }
    uint16_t averageRaw = sum / 60;

     currentVolume = map(averageRaw, 0, 4095, 0, 30);
    
    // Tetap gunakan pengecekan perubahan agar tidak spamming DFPlayer
    if (abs(currentVolume - lastVolume) >= 1) {
      lastVolume = currentVolume;

      dfplayer.volume(currentVolume);
   
      volumeDFPlayer = currentVolume;
    }
  }
}
