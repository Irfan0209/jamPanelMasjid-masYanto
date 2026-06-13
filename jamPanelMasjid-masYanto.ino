
/*************************************************************************************
JAM_DIGITAL_MASJID_MAS-YANTO 93 X 16
17/02/2026
**************************************************************************************/

#include <DMDESP.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/KecNumber.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/BigNumber.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/Font4x6.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/SystemFont5x7.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/Font3x5.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/EMSans8x16.h>
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/Calibri14.h>  
#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/Mono5x7.h>
//#include <C:\Users\irfan\Documents\Arduino\libraries\DMDESP-master\fonts/ElektronMartArabic5x6.h>

#include <Wire.h>
#include <RtcDS3231.h>
#include <Prayer.h>
#include <ESP_EEPROM.h>

#define BUZZ    D4 

#define Font0 Font4x6
#define Font1 SystemFont5x7
#define Font2 Font3x5
#define Font3 BigNumber
#define Font4 KecNumber
#define Font5 EMSans8x16
#define Font6 Calibri14
#define Font7 SystemFont5x7

//#define DEBUG 1
char password[20] = "00000000";

// Object Declarations
DMDESP Disp(3,1);

//create object
RtcDS3231<TwoWire> Rtc(Wire);
RtcDateTime now;

// Constractor
Prayer JWS;
Hijriyah Hijir;

uint8_t iqomah[]        = {1,1,1,1,1,1};
uint8_t displayBlink[]  = {1,1,1,1,1,1};
uint8_t dataIhty[]      = {0,0,0,0,0,0};

struct Config {
  uint8_t durasiadzan;
  uint8_t altitude ;
  double latitude ;
  double longitude ;
  uint8_t zonawaktu;
  int8_t Correction ; //Koreksi tanggal hijriyah, -1 untuk mengurangi, 0 tanpa koreksi, 1 untuk menambah
  uint8_t   brightness;
  uint8_t    speedDate;
  uint8_t    speedText1;  
  uint8_t    speedText2;
  uint8_t    speedText3;
  uint8_t    speedText4;
  uint8_t    speedText5;
  uint8_t    speedName;
  bool       stateMode;
  bool       stateBuzzerClock;
  bool       stateBuzzer;
  bool       stateAlarm ;
  uint8_t    jamOn ;
  uint8_t    jamOff ;
  uint8_t    menitOn ;
  uint8_t    menitOff ;
  char text1[250];
  char text2[250];
  char text3[250];
  char text4[250];
  char text5[250];
  char name[250];
  char ctrJadwal[30];
  
};
Config config;

uint8_t    DWidth        = Disp.width();
uint8_t    DHeight       = Disp.height();

// Variabel untuk waktu, tanggal, teks berjalan, tampilan ,dan kecerahan
bool       adzan         = 0;
int8_t     sholatNow     = -1;
bool       reset_x       = 0; 

/*======library tambahan=======*/
bool       flagAnim = false;
float      dataFloat[10];
//int8_t     dataInteger[10];
bool       stateSendSholat = false; 
bool       stateBuzzWar    = 0;
bool       butuhHitungJadwal = true;
bool       DoSwap          = false;
bool       panelState = false; // false = OFF, true = ON

//bool showVolumeTemp = false;
//uint32_t volumeDisplayMillis = 0;
//constexpr uint16_t volumeDisplayDuration = 2000; // 1 detik
//
//constexpr uint8_t MAX_VOLUME = 25;
//constexpr uint8_t MIN_VOLUME = 0;

//byte volume = 10;

/*============== end ================*/

enum Show{
 // ANIM_ZONK,
  ANIM_BIGFONT,
  ANIM_TEXT1,
  ANIM_TEXT2,
  ANIM_TEXT3,
  ANIM_TEXT4,
  ANIM_TEXT5,
  ANIM_NAME,
  ANIM_DATE,
  ANIM_ADZAN,
  ANIM_IQOMAH,
  ANIM_BLINK,
  ANIM_COUNTER
};
Show show = ANIM_BIGFONT;

enum Line{
  ANIM_ZONK,
  ANIM_SHOLAT,
  ANIM_MASEHI,
  ANIM_DAY_NASIONAL,
  ANIM_DAY_PASARAN
};
Line line = ANIM_ZONK;

#define EEPROM_SIZE       2000


// ================= TEXT (250 char) =================
#define ADDR_TEXT1        0      // 251
#define ADDR_TEXT2        251    // 251
#define ADDR_TEXT3        502    // 251
#define ADDR_TEXT4        753    // 251
#define ADDR_TEXT5        1004   // 251
#define ADDR_NAME         1255   // 251

// ================= DISPLAY =================
#define ADDR_BRIGHTNESS   1506   // 2

// ================= SPEED =================
#define ADDR_SPEEDTX1     1508   // 2
#define ADDR_SPEEDTX2     1510   // 2
#define ADDR_SPEEDTX3     1512   // 2
#define ADDR_SPEEDTX4     1514   // 2
#define ADDR_SPEEDTX5     1516   // 2
#define ADDR_SPEEDNAME    1518   // 2
#define ADDR_SPEEDDT      1520   // 2

// ================= LOKASI =================
#define ADDR_LATITUDE     1522   // 4
#define ADDR_LONGITUDE    1526   // 4
#define ADDR_TZ           1530   // 2
#define ADDR_ALTITUDE     1532   // 2

// ================= SHOLAT =================
#define ADDR_IQOMAH       1534   // 6
#define ADDR_BLINK        1540   // 6
#define ADDR_IHTY         1546   // 6

// ================= AUDIO =================
#define ADDR_BUZZER       1552   // 1
#define ADDR_BUZZER_CLOCK 1553   // 1

// ================= SECURITY =================
#define ADDR_PASSWORD     1554   // 9

// ================= WAKTU =================
#define ADDR_DURASIADZAN  1563   // 2
#define ADDR_CORRECTION   1565   // 2
#define ADDR_MODE         1567   // 1

#define ADDR_JAMON        1568   // 1
#define ADDR_MENITON      1569   // 1
#define ADDR_JAMOFF       1570   // 1
#define ADDR_MENITOFF     1571   // 1

#define ADDR_STATEALARM   1572   // 1

// Menggunakan const char* (Array of Character) menggantikan objek String
void saveStringToEEPROM(int startAddr, const char* data, int maxLength) {
  int len = strlen(data); // Hitung panjang teks asli
  
  for (int i = 0; i < maxLength; i++) {
    if (i < len) {
      EEPROM.write(startAddr + i, data[i]);
    } else {
      // Sisa ruang diisi dengan NULL (0) untuk membersihkan sisa memori lama
      EEPROM.write(startAddr + i, 0); 
    }
  }
}

// EEPROM.put otomatis menangani konversi byte untuk tipe Float (4 bytes)
void saveFloatToEEPROM(int addr, float value) {
  EEPROM.put(addr, value);
}

// EEPROM.put otomatis menangani pemecahan highByte dan lowByte untuk int16 (2 bytes)
void saveIntToEEPROM(int addr, int16_t value) {
  EEPROM.put(addr, value);
}


// Fungsi untuk mengatur jam, tanggal, running text, dan kecerahan dari Serial
void handleSetTimeSerial() {
  static char buffer[255];
  static uint8_t index = 0;

  while (Serial.available() > 0) {
    char c = Serial.read();

    // FILTER 1: Hanya simpan karakter ASCII yang normal / bisa dibaca!
    // Karakter bernilai 32 (spasi) sampai 126 (simbol '~') mencakup semua huruf, angka, dan tanda baca (=, -, :).
    // Karakter aneh seperti '⸮' (nilai di atas 127) akan langsung DITENDANG dan tidak dimasukkan ke memori.
    if (c >= 32 && c <= 126) {
      if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      }
    }
    
    // FILTER 2: Jika mendeteksi tombol Enter (Newline '\n') atau Carriage Return ('\r')
    else if (c == '\n' || c == '\r') {
      if (index > 0) { 
        buffer[index] = '\0'; // Tutup dan kunci teksnya
        
        // --- Boleh biarkan jebakan debug ini sementara ---
//        Serial.print(F("[DEBUG PINTU MASUK] Teks bersih: '"));
//        Serial.print(buffer);
//        Serial.println(F("'"));
        // -------------------------------------------------

        getData(buffer); // Lempar ke pemroses data
        index = 0;       // Reset index menjadi 0 untuk pesan berikutnya
      }
    }
  }
}


//----------------------------------------------------------------------
// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void ICACHE_RAM_ATTR refresh() {
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);
}


void Disp_init_esp() {
  Disp.setDoubleBuffer(true);
  Disp.start();
  Disp.clear();
  Disp.setBrightness(config.brightness);
  
  Disp.swapBuffers();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(refresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
}


//=======================================
//===SETUP=============================== 
//=======================================
void setup()
{ 
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  
  pinMode(BUZZ, OUTPUT); 
  digitalWrite(BUZZ,LOW);
  delay(200);
  digitalWrite(BUZZ,HIGH);
  
  uint8_t rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
    if (rtn != 0) {
      Serial.println(F("I2C bus error. Could not clear"));
      if (rtn == 1) {
        Serial.println(F("SCL clock line held low"));
      } else if (rtn == 2) {
        Serial.println(F("SCL clock line held low by slave clock stretch"));
      } else if (rtn == 3) {
        Serial.println(F("SDA data line held low"));
      }
    } 
    else { // bus clear, re-enable Wire, now can start Wire Arduino master
      Wire.begin();
    }
  
  Rtc.Begin();
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
  
  loadFromEEPROM();
  
  delay(1000);
  
  Disp_init_esp();
  Serial.println("PANEL_OK");
  
  delay(500);
  for(uint8_t i = 0; i < 4; i++)
   {
      Buzzer(1);
      delay(80);
      Buzzer(0);
      delay(80);
   }
   
}

//=======================================
//===MAIN LOOP Function =================   
//=======================================
void loop()
  {
    // =========================================
    // List of Display Component Block =========
    // =========================================
  handleSetTimeSerial();
  DoSwap  = false ;
  check();
  islam();
  Disp.clear();
  
//  if (showVolumeTemp) {
//    tampilkanVolume();
//    if (millis() - volumeDisplayMillis >= volumeDisplayDuration) {
//      showVolumeTemp = false;
//    }
//  }else{
  switch(show){
    case ANIM_BIGFONT : 
       runn(config.name,config.speedName,5);
    break;

    case ANIM_DATE :
     runn(showTanggal(),config.speedDate,1);
    break;

    case ANIM_NAME :
    runn(config.name,config.speedName,1);
    break;

    case ANIM_TEXT1 :
    runn(config.text1,config.speedText1,1);
    break;

    case ANIM_TEXT2 :
    runn(config.text2,config.speedText2,1);
    break;

    case ANIM_TEXT3 :
    runn(config.text3,config.speedText3,1);
    break;

    case ANIM_TEXT4 :
    runn(config.text4,config.speedText4,1);
    break;

    case ANIM_TEXT5 :
    runn(config.text5,config.speedText5,1);
    break;

    case ANIM_ADZAN :
      drawAzzan();
    break;

    case ANIM_IQOMAH :
      drawIqomah();
    break;

    case ANIM_BLINK :
      blinkBlock();
    break;

    case ANIM_COUNTER :
     updateNowTime();
     updateSholatNow();
     runn(tampilSelisih(),50,1);
    break;
  };

  switch(line){
    case ANIM_ZONK :

    break;
    
    case ANIM_SHOLAT :
      updateAnimSholat();
    break;

    case ANIM_MASEHI :
      updateAnimUpDown(TGLMASEHI());
    break;

    case ANIM_DAY_NASIONAL :
      updateAnimUpDown(NASIONAL());
    break;

    case ANIM_DAY_PASARAN :
      updateAnimUpDown(PASARAN());
    break;
  };
  
  jamCenter();
  
//}
    buzzerWarning(stateBuzzWar);
    yield();
    if(DoSwap){Disp.swapBuffers();} // Swap Buffer if Change

  
   
  }

void getData(const char* data) {
  //const char* data = input.c_str();
  const char* eq_ptr = strchr(data, '=');

//  Serial.print(F("[DEBUG 1] Masuk ke getData. Isi raw: '"));
//  Serial.print(data);
//  Serial.println(F("'"));
  
  if (eq_ptr != nullptr) {
    size_t key_len = eq_ptr - data; // Mengambil panjang kunci
    const char* ptr = eq_ptr + 1;   // Pointer untuk membaca nilai

    if (key_len == 2 && strncmp(data, "Tm", 2) == 0) {
      // Format: HH:MM:SS-Tanggal-Bulan-Tahun
      uint8_t jam = atoi(ptr);
      ptr = strchr(ptr, ':'); if (!ptr) return; ptr++;
      uint8_t menit = atoi(ptr);
      ptr = strchr(ptr, ':'); if (!ptr) return; ptr++;
      uint8_t detik = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint8_t tanggal = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint8_t bulan = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint16_t tahun = atoi(ptr);

      Rtc.SetDateTime(RtcDateTime(tahun, bulan, tanggal, jam, menit, detik));
      stateSendSholat = 1;
      butuhHitungJadwal = true;
    } 
    
    else if (key_len == 4 && strncmp(data, "text", 4) == 0) {
      int indexText = atoi(ptr);
      const char* dash_ptr = strchr(ptr, '-');
      
      if (dash_ptr != nullptr) {
        const char* pesan = dash_ptr + 1;
        
        // strncpy menyalin maksimal 250 karakter dengan sangat aman & efisien
        char tempBuffer[251];
        strncpy(tempBuffer, pesan, 250);
        tempBuffer[250] = '\0'; // Pastikan ada null-terminator

        if (indexText == 1) {
          strcpy(config.text1, tempBuffer);
          saveStringToEEPROM(ADDR_TEXT1, config.text1, 250);
        } else if (indexText == 2) {
          strcpy(config.text2, tempBuffer);
          saveStringToEEPROM(ADDR_TEXT2, config.text2, 250);
        } else if (indexText == 3) {
          strcpy(config.text3, tempBuffer);
          saveStringToEEPROM(ADDR_TEXT3, config.text3, 250);
        } else if (indexText == 4) {
          strcpy(config.text4, tempBuffer);
          saveStringToEEPROM(ADDR_TEXT4, config.text4, 250);
        } else if (indexText == 5) {
          strcpy(config.text5, tempBuffer);
          saveStringToEEPROM(ADDR_TEXT5, config.text5, 250);
        }
      }
      Buzzer(1); delay(500); ESP.restart();
    } 
    
    else if (key_len == 4 && strncmp(data, "name", 4) == 0) {
      strncpy(config.name, ptr, 250);
      config.name[250] = '\0'; // Pastikan aman (tidak lebih dari 250 char)
      Serial.println(config.name);
      saveStringToEEPROM(ADDR_NAME, config.name, 250);
      Buzzer(1); delay(500); ESP.restart();
    } 
    
    else if (key_len == 2 && strncmp(data, "Br", 2) == 0) {
      config.brightness = map(atoi(ptr), 0, 100, 10, 255);
      Disp.setBrightness(config.brightness);
      saveIntToEEPROM(ADDR_BRIGHTNESS, config.brightness);
    } 
    
    else if (key_len == 5 && strncmp(data, "Sptx1", 5) == 0) {
      config.speedText1 = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDTX1, config.speedText1);
    } else if (key_len == 5 && strncmp(data, "Sptx2", 5) == 0) {
      config.speedText2 = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDTX2, config.speedText2);
    } else if (key_len == 5 && strncmp(data, "Sptx3", 5) == 0) {
      config.speedText3 = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDTX3, config.speedText3);
    } else if (key_len == 5 && strncmp(data, "Sptx4", 5) == 0) {
      config.speedText4 = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDTX4, config.speedText4);
    } else if (key_len == 5 && strncmp(data, "Sptx5", 5) == 0) {
      config.speedText5 = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDTX5, config.speedText5);
    } 
    
    else if (key_len == 4 && strncmp(data, "Spdt", 4) == 0) {
      config.speedDate = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDDT, config.speedDate);
    } else if (key_len == 4 && strncmp(data, "Spnm", 4) == 0) {
      config.speedName = map(atoi(ptr), 0, 100, 10, 80);
      saveIntToEEPROM(ADDR_SPEEDNAME, config.speedName);
    } 
    
    // Gunakan atof() untuk konversi ke float (bilangan desimal)
    else if (key_len == 2 && strncmp(data, "Lt", 2) == 0) {
      config.latitude = roundf(atof(ptr) * 1000000.0) / 1000000.0;
      saveFloatToEEPROM(ADDR_LATITUDE, config.latitude);
      butuhHitungJadwal = true;
    } else if (key_len == 2 && strncmp(data, "Lo", 2) == 0) {
      config.longitude = roundf(atof(ptr) * 1000000.0) / 1000000.0;
      saveFloatToEEPROM(ADDR_LONGITUDE, config.longitude);
      butuhHitungJadwal = true;
    } 
    
    else if (key_len == 2 && strncmp(data, "Tz", 2) == 0) {
      config.zonawaktu = atoi(ptr);
      saveIntToEEPROM(ADDR_TZ, config.zonawaktu);
      butuhHitungJadwal = true;
    } else if (key_len == 2 && strncmp(data, "Al", 2) == 0) {
      config.altitude = atoi(ptr);
      saveIntToEEPROM(ADDR_ALTITUDE, config.altitude);
      butuhHitungJadwal = true;
    } 
    
    // Bagian Index dengan pemisah '-'
    else if (key_len == 2 && strncmp(data, "Iq", 2) == 0) {
      uint8_t indexSholat = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint8_t indexKoreksi = atoi(ptr);
      iqomah[indexSholat] = indexKoreksi;
      EEPROM.write(ADDR_IQOMAH + indexSholat, indexKoreksi);
    } else if (key_len == 2 && strncmp(data, "Dy", 2) == 0) {
      uint8_t indexSholat = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint8_t indexKoreksi = atoi(ptr);
      displayBlink[indexSholat] = indexKoreksi;
      EEPROM.write(ADDR_BLINK + indexSholat, indexKoreksi);
    } else if (key_len == 2 && strncmp(data, "Kr", 2) == 0) {
      uint8_t indexSholat = atoi(ptr);
      ptr = strchr(ptr, '-'); if (!ptr) return; ptr++;
      uint8_t indexKoreksi = atoi(ptr);
      dataIhty[indexSholat] = indexKoreksi;
      EEPROM.write(ADDR_IHTY + indexSholat, indexKoreksi);
      butuhHitungJadwal = true;
    } 
    
    else if (key_len == 2 && strncmp(data, "Da", 2) == 0) {
      config.durasiadzan = atoi(ptr);
      EEPROM.write(ADDR_DURASIADZAN, config.durasiadzan & 0xFF);
      EEPROM.write(ADDR_DURASIADZAN + 1, (config.durasiadzan >> 8) & 0xFF);
    } else if (key_len == 4 && strncmp(data, "CoHi", 4) == 0) {
      config.Correction = atoi(ptr);
      EEPROM.write(ADDR_CORRECTION, config.Correction & 0xFF);
      EEPROM.write(ADDR_CORRECTION + 1, (config.Correction >> 8) & 0xFF);
      butuhHitungJadwal = true;
    } 
    
    else if (key_len == 3 && strncmp(data, "Bzr", 3) == 0) {
      config.stateBuzzer = atoi(ptr);
      EEPROM.write(ADDR_BUZZER, config.stateBuzzer);
    } else if (key_len == 6 && strncmp(data, "bzrClk", 6) == 0) {
      config.stateBuzzerClock = atoi(ptr);
      EEPROM.write(ADDR_BUZZER_CLOCK, config.stateBuzzerClock);
    } 
    
    else if (key_len == 5 && strncmp(data, "alarm", 5) == 0) {
      config.stateAlarm = atoi(ptr);
      EEPROM.write(ADDR_STATEALARM, config.stateAlarm);
      if (!config.stateAlarm) {
        panelState = false;
        Disp.setBrightness(config.brightness);
      }
    } 
    
    else if (key_len == 7 && strncmp(data, "alarmOn", 7) == 0) {
      uint8_t h = atoi(ptr);
      ptr = strchr(ptr, ':'); 
      if (ptr) {
        uint8_t m = atoi(ptr + 1);
        if (h <= 23 && m <= 59) {
          config.jamOn = h;
          config.menitOn = m;
          saveIntToEEPROM(ADDR_JAMON, config.jamOn);
          saveIntToEEPROM(ADDR_MENITON, config.menitOn);
        }
      }
    } 
    
    else if (key_len == 8 && strncmp(data, "alarmOff", 8) == 0) {
      uint8_t h = atoi(ptr);
      ptr = strchr(ptr, ':'); 
      if (ptr) {
        uint8_t m = atoi(ptr + 1);
        if (h <= 23 && m <= 59) {
          config.jamOff = h;
          config.menitOff = m;
          saveIntToEEPROM(ADDR_JAMOFF, config.jamOff);
          saveIntToEEPROM(ADDR_MENITOFF, config.menitOff);
        }
      }
    } 
    
    else if (key_len == 4 && strncmp(data, "mode", 4) == 0) {
      config.stateMode = atoi(ptr);
      EEPROM.write(ADDR_MODE, config.stateMode);
      delay(1000);
      ESP.restart();
    } 
    
    else if (key_len == 6 && strncmp(data, "status", 6) == 0) {
      bool state = atoi(ptr);
      if (state) {
        for (byte i = 0; i < 3; i++) {
          Buzzer(1); delay(80);
          Buzzer(0); delay(80);
        }
        Serial.println(F("PANEL_OK"));
      }
    } 
    
    else if (key_len == 6 && strncmp(data, "jadwal", 6) == 0) {
      stateSendSholat = atoi(ptr);
    } 
    
    else if (key_len == 7 && strncmp(data, "restart", 7) == 0) {
      bool state = atoi(ptr);
      if (state) {
        Buzzer(1); 
        Serial.println(F("RESTART_OK")); 
        config.stateMode = 0;
        EEPROM.write(ADDR_MODE, config.stateMode); 
        delay(1000);
        ESP.restart();
      }
    } 
    
    else if (key_len == 11 && strncmp(data, "newPassword", 11) == 0) {
      if (strlen(ptr) == 8) {
        strncpy(password, ptr, 9); // Salin 8 karakter + 1 null terminator
        saveStringToEEPROM(ADDR_PASSWORD, password, 8);
        Buzzer(1); delay(500); ESP.restart();
      }
    }

    EEPROM.commit(); // Eksekusi penyimpanan ke flash memori
  }
}

void loadFromEEPROM() {
  //Serial.println("=== Membaca Data dari EEPROM ===");
 
  for (uint8_t i = 0; i < 250; i++) {
    config.text1[i] = EEPROM.read(ADDR_TEXT1 + i);
    if (config.text1[i] == 0) break;
  }

  for (uint8_t i = 0; i < 250; i++) {
    config.text2[i] = EEPROM.read(ADDR_TEXT2 + i);
    if (config.text2[i] == 0) break;
  }

  for (uint8_t i = 0; i < 250; i++) {
    config.text3[i] = EEPROM.read(ADDR_TEXT3 + i);
    if (config.text3[i] == 0) break;
  }

  for (uint8_t i = 0; i < 250; i++) {
    config.text4[i] = EEPROM.read(ADDR_TEXT4 + i);
    if (config.text4[i] == 0) break;
  }

  for (uint8_t i = 0; i < 250; i++) {
    config.text5[i] = EEPROM.read(ADDR_TEXT5 + i);
    if (config.text5[i] == 0) break;
  }

  for (uint8_t i = 0; i < 250; i++) {
    config.name[i] = EEPROM.read(ADDR_NAME + i);
    if (config.name[i] == 0) break;
  }

  config.brightness = EEPROM.read(ADDR_BRIGHTNESS);

  config.speedText1 = EEPROM.read(ADDR_SPEEDTX1);

  config.speedText2 = EEPROM.read(ADDR_SPEEDTX2);

  config.speedText3 = EEPROM.read(ADDR_SPEEDTX3);

  config.speedText4 = EEPROM.read(ADDR_SPEEDTX4);
 
  config.speedText5 = EEPROM.read(ADDR_SPEEDTX5);

  config.speedDate = EEPROM.read(ADDR_SPEEDDT);

  config.speedName = EEPROM.read(ADDR_SPEEDNAME);

  // Latitude
  float latVal;
  byte *ptrLat = (byte*)(void*)&latVal;
  for (int i = 0; i < sizeof(float); i++) {
    ptrLat[i] = EEPROM.read(ADDR_LATITUDE + i);
  }
  config.latitude = latVal;

  // Longitude
  float lonVal;
  byte *ptrLon = (byte*)(void*)&lonVal;
  for (int i = 0; i < sizeof(float); i++) {
    ptrLon[i] = EEPROM.read(ADDR_LONGITUDE + i);
  }
  config.longitude = lonVal;

  config.zonawaktu = EEPROM.read(ADDR_TZ) | (EEPROM.read(ADDR_TZ + 1) << 8);

  config.altitude = EEPROM.read(ADDR_ALTITUDE) | (EEPROM.read(ADDR_ALTITUDE + 1) << 8);

  for (uint8_t i = 0; i < 6; i++) {
    iqomah[i] = EEPROM.read(ADDR_IQOMAH + i);
    #if DEBUG
      Serial.print("Iqomah[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(iqomah[i]);
    #endif
  }

  for (uint8_t i = 0; i < 6; i++) {
    displayBlink[i] = EEPROM.read(ADDR_BLINK + i);
    #if DEBUG
      Serial.print("Blink[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(displayBlink[i]);
    #endif
  }

  for (uint8_t i = 0; i < 6; i++) {
    dataIhty[i] = EEPROM.read(ADDR_IHTY + i);
    #if DEBUG
      Serial.print("Ihtiyath[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(dataIhty[i]);
    #endif
  }

  config.stateBuzzer = EEPROM.read(ADDR_BUZZER);

  config.stateBuzzerClock = EEPROM.read(ADDR_BUZZER_CLOCK);

  config.stateAlarm = EEPROM.read(ADDR_STATEALARM);

  config.jamOn = EEPROM.read(ADDR_JAMON);

  config.menitOn = EEPROM.read(ADDR_MENITON);

  config.jamOff = EEPROM.read(ADDR_JAMOFF);

  config.menitOff = EEPROM.read(ADDR_MENITOFF);

  config.stateMode = EEPROM.read(ADDR_MODE);

  for (uint8_t i = 0; i < 8; i++) {
    password[i] = EEPROM.read(ADDR_PASSWORD + i);
  }
  password[8] = '\0';

  config.durasiadzan = EEPROM.read(ADDR_DURASIADZAN) | (EEPROM.read(ADDR_DURASIADZAN + 1) << 8);
 
  config.Correction = EEPROM.read(ADDR_CORRECTION) | (EEPROM.read(ADDR_CORRECTION + 1) << 8);
  
#if DEBUG
  Serial.print("Text1: ");
  Serial.println(config.text1);
  Serial.print("Text2: ");
  Serial.println(config.text2);
  Serial.print("Text3: ");
  Serial.println(config.text3);
  Serial.print("Text4: ");
  Serial.println(config.text4);
  Serial.print("Text5: ");
  Serial.println(config.text5);
  Serial.print("nama: ");
  Serial.println(config.name);
  Serial.print("Brightness: ");
  Serial.println(config.brightness);
  Serial.print("Speed Text1: ");
  Serial.println(config.speedText1);
  Serial.print("Speed Text2: ");
  Serial.println(config.speedText2);
  Serial.print("Speed Text3: ");
  Serial.println(config.speedText3);
  Serial.print("Speed Text4: ");
  Serial.println(config.speedText4);
  Serial.print("Speed Text5: ");
  Serial.println(config.speedText5);
  Serial.print("Speed Date: ");
  Serial.println(config.speedDate);
  Serial.print("Speed Name: ");
  Serial.println(config.speedName);
  Serial.print("Latitude: ");
  Serial.println(config.latitude, 6);
  Serial.print("Longitude: ");
  Serial.println(config.longitude, 6);
  Serial.print("Zona Waktu: ");
  Serial.println(config.zonawaktu);
  Serial.print("Altitude: ");
  Serial.println(config.altitude);
  Serial.print("Buzzer: ");
  Serial.println(config.stateBuzzer);
  Serial.print("stateBuzzerClock: ");
  Serial.println(config.stateBuzzerClock);
  Serial.print("stateAlarm: ");
  Serial.println(config.stateAlarm);
  Serial.print("jamOn: ");
  Serial.println(config.jamOn);
  Serial.print("menitOn: ");
  Serial.println(config.menitOn);
  Serial.print("jamOff: ");
  Serial.println(config.jamOff);
  Serial.print("menitOff: ");
  Serial.println(config.menitOff);
  Serial.print("mode: ");
  Serial.println(config.stateMode);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("Durasi Adzan: ");
  Serial.println(config.durasiadzan);
  Serial.print("Correction: ");
  Serial.println(config.Correction);
#endif

Serial.print("PWD=");
Serial.println(password);
//  Serial.println("=== Selesai Membaca EEPROM ===\n");
//  Serial.println("OK");
}

 //----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

uint8_t I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  uint8_t clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    uint8_t counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}

void buzzerWarning(bool cek){

   static bool state = false;
   static uint32_t save = 0;
   uint32_t tmr = millis();
   static uint8_t con = 0;
    
    if(tmr - save > 2500 && cek == 1){
      save = tmr;
      state = !state;
      Buzzer(state);
      if(con <= 6) { con++; }
      if(con == 7) { Buzzer(0); cek = 0; con = 0; state = false; stateBuzzWar = 0; }
    } 
    
}

void Buzzer(bool state)
  {
    if(!config.stateBuzzer) return;
    
     switch(state){
      case 0 :
        noTone(BUZZ);
      break;
      case 1 :
        tone(BUZZ,2000);
      break;
    };
  }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
