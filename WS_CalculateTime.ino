
//======================== new program =========================//
void islam() {
  RtcDateTime now = Rtc.GetDateTime();
  
  cekJadwalPanel(now.Hour(), now.Minute());

  static bool lastJumat = false;
  
  JUMAT = cekJadwalJumat(now.Hour(), now.Minute(), now.DayOfWeek()); 
   
//  // Transisi Jumat
//  if (JUMAT && !lastJumat) {
//    show = ANIM_JUMAT1;
//    lastJumat = true;
//  } 
//  else if (!JUMAT && lastJumat) {
//    // Saat Jumat selesai, kembalikan ke jam normal (tapi pastikan tidak sedang Tarhim)
//    if (!PRE_IQOMAH) show = ANIM_CLOCK;
//    lastJumat = false;
//  }
  
   // 3. LOGIKA BUNYI JAM TEPAT
  static int8_t lastHalfPlay = -1;
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastHalfPlay && config.stateBuzzerClock) {
    lastHalfPlay = now.Hour();
    stateBuzzWar = 1;
  }

  // 4. LOGIKA PEMBARUAN JADWAL HARIAN
  static int8_t lastDayCalc = -1;
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() != lastDayCalc){
    lastDayCalc = now.Hour();
    butuhHitungJadwal = true; 
  }

  // Restart otomatis (opsional, dari kodemu)
  if (now.Minute() == 0 && now.Second() == 0 && now.Hour() == 0){
    Serial.println("restart");
    // ESP.restart(); // (Aktifkan jika memang alat diset untuk restart harian)
  }

  // 5. EKSEKUSI RUMUS KALKULASI JWS
  if(butuhHitungJadwal){
  
    for(uint8_t i = 0; i < 2; i++) {
      JWS.Update(config.zonawaktu, config.latitude, config.longitude, config.altitude, now.Year(), now.Month(), now.Day());
      JWS.setIkhtiSu = dataIhty[0];
      JWS.setIkhtiDzu = dataIhty[1];
      JWS.setIkhtiAs = dataIhty[2];
      JWS.setIkhtiMa = dataIhty[3];
      JWS.setIkhtiIs = dataIhty[4];
      JWS.setIkhtiIm = dataIhty[5];
      Hijir.Update(now.Year(), now.Month(), now.Day(), config.Correction);
    }
    butuhHitungJadwal = false; 
  }
}

// digunakan untuk menghitung hari pasaran
uint32_t jumlahhari() { 
  RtcDateTime now = Rtc.GetDateTime();
  uint8_t d = now.Day();
  uint8_t m = now.Month();
  uint16_t y = now.Year();

  static const uint16_t hb[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
  uint32_t ht = (y - 1970) * 365 - 1;
  uint16_t hs = hb[m - 1] + d;

  if (y % 4 == 0 && m > 2) hs++; // Tambahkan 1 hari jika tahun kabisat dan lewat Februari

  uint32_t kab = (y - 1969) / 4;  // Hitung langsung jumlah tahun kabisat

  return (ht + hs + kab);
}
