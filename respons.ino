//=================== menghitung nilai volume =======================//
/*void setVolume(byte newVolume) {

  if (newVolume > MAX_VOLUME) {
    volume = MAX_VOLUME;
    Buzzer(1);
    delay(50);
    Buzzer(0);
  }
  else if (newVolume < MIN_VOLUME) {
    volume = MIN_VOLUME;
    Buzzer(1);
    delay(50);
    Buzzer(0);
  }
  else {
    volume = newVolume;
  }

  // Aktifkan tampilan sementara
  showVolumeTemp = true;
  volumeDisplayMillis = millis();
}
*/
//=====================  perhitungan waktu mundur waktu sholat ============================//

// index: 1=Subuh, 2=Dzuhur, 3=Ashar, 4=Maghrib, 5=Isya

uint32_t nowSec;         // waktu sekarang (detik)
uint8_t  SholatNow;      // sholat aktif sekarang

void updateNowTime()
{
  RtcDateTime now = Rtc.GetDateTime();
  nowSec = toSecond(now.Hour(), now.Minute(), now.Second());
}


void updateSholatNow()
{
  if (nowSec < sholatSec[1])        SholatNow = 5; // sebelum subuh
  else if (nowSec < sholatSec[2])   SholatNow = 1;
  else if (nowSec < sholatSec[3])   SholatNow = 2;
  else if (nowSec < sholatSec[4])   SholatNow = 3;
  else if (nowSec < sholatSec[5])   SholatNow = 4;
  else                              SholatNow = 5;
}

static const uint8_t NEXT_SHOLAT[7] = {
  0, 2, 3, 4, 5, 1
};

uint32_t hitungSelisih()
{
  uint8_t next = NEXT_SHOLAT[SholatNow];

  if (sholatSec[next] >= nowSec)
    return sholatSec[next] - nowSec;
  else
    return (86400UL - nowSec) + sholatSec[next];
}

char * tampilSelisih()
{
  uint32_t sisa = hitungSelisih();

  uint8_t jam   = sisa / 3600;
  uint8_t menit = (sisa % 3600) / 60;
  uint8_t detik = sisa % 60;


  snprintf(config.ctrJadwal,sizeof(config.ctrJadwal),"%s %s -%02u:%02u:%02u","menuju waktu",jadwalAzzan[NEXT_SHOLAT[SholatNow]-1], jam, menit, detik);
  return config.ctrJadwal;
 
}

void cekJadwalPanel(uint8_t jamNow, uint8_t menitNow) {
  if(!config.stateAlarm) return;
  
  // Hitung waktu sekarang & jadwal dalam menit
  uint16_t nowTime  = jamNow * 60 + menitNow;
  uint16_t onTime   = config.jamOn * 60 + config.menitOn;
  uint16_t offTime  = config.jamOff * 60 + config.menitOff;

  bool shouldOn;

  // ====== HANDLE JADWAL NORMAL (ON < OFF) ======
  if (onTime < offTime) {
    shouldOn = (nowTime >= onTime && nowTime < offTime);
  }
  // ====== HANDLE JADWAL LEWAT TENGAH MALAM ======
  else {
    shouldOn = (nowTime >= onTime || nowTime < offTime);
  }

  // ====== EKSEKUSI ======
  if (shouldOn && !panelState) {
    panelON();
  } 
  else if (!shouldOn && panelState) {
    panelOFF();
  }
}

void panelON() {
  Disp.setBrightness(config.brightness);
  panelState = true;
  Serial.println("Panel ON");
}

void panelOFF() {
  Disp.setBrightness(0);
  panelState = false;
  Serial.println("Panel OFF");
}
