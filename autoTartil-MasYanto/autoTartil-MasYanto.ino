/*
  Auto Tart.tick();m with Arduino Micro + ESP8266 + DFPlayer Mini
  --------------------------------------------------------------
  - ESP8266: menerima konfigurasi dari App Inventor via AP + HTTP GET
  - Arduino micro: membaca konfigurasi via Serial, menyimpan konfigurasi dalam array
  - Arduino mengatur jadwal berdasarkan waktu dan memutar rekaman tartil dan adzan
  - DFPlayer Mini: memainkan file tartil & adzan
  - Relay: mengaktifkan power amplifier saat audio diputar
*/

#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "OneButton.h"

#define LED_WIFI 2

//LIBRARY UNTUK ACCES POINT
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsClient.h>

//OBJEK dfPlayer
#define dfSerial Serial2
DFRobotDFPlayerMini dfplayer;

//OBJEK WEB SERVER
WebServer server(80);
WebSocketsClient webSocket;

IPAddress local_IP(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

#define RELAY_PIN         27
#define RUN_LED           9

#define EEPROM_SIZE 1000

#define HARI_TOTAL  8 // 7 hari + SemuaHari (index ke-7)
#define WAKTU_TOTAL 5
#define MAX_FILE    30
#define MAX_FOLDER  2 //3
#define JEDA_ANTAR_TARTIL 50 //500 jeda antar file tartil dalam milidetik

//#define DEBUG 1

struct WaktuConfig {
  byte aktif;
  byte aktifAdzan;
  byte fileAdzan;
  byte tartilDulu;
  byte folder;
  byte list[5];
};

WaktuConfig jadwal[HARI_TOTAL][WAKTU_TOTAL];
uint8_t durasiAdzan[MAX_FILE];
uint16_t durasiTartil[MAX_FOLDER][MAX_FILE];
byte volumeDFPlayer;
uint8_t jamSholat[WAKTU_TOTAL]; //= {4, 12, 15, 18, 19};
uint8_t menitSholat[WAKTU_TOTAL];// = {30, 0, 30, 0, 30};

bool tartilSedangDiputar = false;
uint32_t tartilMulaiMillis = 0;
byte tartilFolder = 0;
byte tartilIndex = 0;

uint16_t tartilCounter = 0;
uint16_t targetDurasi = 0;
unsigned long lastTick = 0;

bool jedaAktif = false;
uint32_t jedaMulaiMillis = 0;

WaktuConfig *currentCfg = nullptr;

uint32_t lastTriggerMillis = 0;
bool sudahEksekusi = false;
bool adzanSedangDiputar = false;
uint32_t adzanMulaiMillis = 0;
uint16_t adzanDurasi = 0;

uint32_t lastAdzanTick = 0;
uint16_t adzanCounter = 0;
uint16_t targetDurasiAdzan = 0;

byte currentDay = 0;

// Tambahan untuk relay delay dan manual
uint32_t relayOffDelayMillis = 0;
bool relayMenungguMati = false;
bool manualSedangDiputar = false;
bool adzanManualSedangDiputar = false;

//variabel untuk led status system
static uint8_t m_Counter = 0;
static uint16_t waveStepDelay = 20;  // Delay antar frame LED breathing (ms)
static uint32_t lastWaveMillis = 0;
//bool STATUS_MODE = false;
//bool lastStatusMode = !STATUS_MODE;     // agar langsung update saat pertama kali
bool lastNormalStatus = false;
uint32_t lastTimeReceived = 0;
const uint32_t TIMEOUT_INTERVAL = 70000; // 70 detik, lebih dari 1 menit
bool wsConnected = false;
bool wifiConnected = false;
unsigned long lastWiFiAttempt = 0;
const unsigned long wifiRetryInterval = 5000;
// ------------------- WebSocket Event -------------------
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      //Serial.println("[WS] Terputus dari server");
      wsConnected = false;
      break;
    case WStype_CONNECTED:
      //Serial.println("[WS] Terhubung ke server");
      wsConnected = true;
      webSocket.sendTXT("CLIENT_READY");
      break;
    case WStype_TEXT: {
      String msg = String((char*)payload);
      //Serial.println(msg);
     if(msg == "restart"){
        delay(500);
        ESP.restart();
      }else{
        Serial.println(msg);
      }
      break;
    }
  }
}

void setup() {
  EEPROM.begin(EEPROM_SIZE);//
  digitalWrite(RELAY_PIN, HIGH); // Awal mati
  
  pinMode(RUN_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  
  delay(1000);
  Serial.begin(9600);
  dfSerial.begin(9600, SERIAL_8N1, /*rx =*/16, /*tx =*/17);
 
  if (!dfplayer.begin(dfSerial,/*isACK = */true, /*doReset = */true)) {
    Serial.println("DFPlayer tidak terdeteksi!");
    while (1);
  }
  
  dfplayer.enableDAC(); // Pakai output DAC (line out)
  Serial.println("Sistem Auto Tartil Siap.");
  
  loadFromEEPROM();
  
  delay(2000);
  
  dfplayer.volume(volumeDFPlayer);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  if (sudahEksekusi && millis() - lastTriggerMillis > 60000) {
    sudahEksekusi = false;
  }
  bacaDataSerial();
  cekDanPutarSholatNonBlocking();
  cekSelesaiTartil();
  cekSelesaiAdzan();
  cekSelesaiAdzanManual();
  cekRelayOffDelay();
  cekSelesaiManual();
  //cekStatusSystem();
  getStatusRun();

 if (!wifiConnected && millis() - lastWiFiAttempt >= wifiRetryInterval) {
    lastWiFiAttempt = millis();

    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;

        webSocket.begin("192.168.2.1", 81, "/");
    } 
  }

  if (wifiConnected ) {
    webSocket.loop();
  }

  digitalWrite(LED_WIFI, (wifiConnected && wsConnected ) ? HIGH : LOW);

}

void bacaDataSerial() {
  static String buffer = "";
  while (Serial.available()) {
    char c = Serial.read();
    //Serial.print(c); // DEBUG: tampilkan semua karakter yang diterima
    if (c == '\n') {
      //Serial.println("\n>> Memanggil parseData()");
      parseData(buffer);
      buffer = "";
    } else {
      buffer += c;
    }
  }
}

//================= parsing data dari Akses Point =========================//
int getIntPart(String &s, int &pos) {
  int comma = s.indexOf(',', pos);
  if (comma == -1) comma = s.length();
  int val = s.substring(pos, comma).toInt();
  pos = comma + 1;
  return val;
}

void parseData(String data) {
  //Serial.println("DATA: " + data);
lastTimeReceived = millis();
 // --- Parsing TIME ---
if (data.startsWith("TIME:")) {
  //lastTimeReceived = millis();
  int idx = 5;
  uint8_t jam    = getIntPart(data, idx);
  uint8_t menit  = getIntPart(data, idx);
  uint8_t detik  = getIntPart(data, idx);
  uint8_t hari   = getIntPart(data, idx);

  if (jam < 24 && menit < 60 && detik < 60 && hari < 7) {
    setTime(jam, menit, detik, 1, 1, 2024);
    currentDay = hari;
    //============ DEBUG =============//
//    Serial.print(F("Waktu diatur ke: "));
//    Serial.print(jam); Serial.print(":");
//    Serial.print(menit); Serial.print(":");
//    Serial.print(detik); Serial.print(" | Hari ke-");
//    Serial.println(hari);
  } else {
    //Serial.println(F("Format TIME tidak valid."));
  }
  return;
}


  // --- Parsing VOL ---
  if (data.startsWith("VOL:")) {
    //lastTimeReceived = millis();
    volumeDFPlayer = data.substring(4).toInt();
    dfplayer.volume(volumeDFPlayer);
    saveToEEPROM();
    return;
  }

//---- Program baru------//
  // --- Parsing HR (jadwal harian) ---
if (data.startsWith("HR:")) {
  //lastTimeReceived = millis();
  int hariEnd = data.indexOf('|');
  if (hariEnd == -1) return;

  int hari = data.substring(3, hariEnd).toInt();
  if (hari < 0 || hari >= HARI_TOTAL) return;

  for (int w = 0; w < WAKTU_TOTAL; w++) {
    String tag = "|W" + String(w) + ":";
    int idxW = data.indexOf(tag);
    if (idxW == -1) continue;

    int pos = idxW + tag.length();
    WaktuConfig &cfg = jadwal[hari][w];

    cfg.aktif        = getIntPart(data, pos);
    cfg.aktifAdzan   = getIntPart(data, pos);
    cfg.fileAdzan    = getIntPart(data, pos);
    cfg.tartilDulu   = getIntPart(data, pos);
    cfg.folder       = getIntPart(data, pos);

    // Lebih aman dan memastikan semua list[i] terisi
for (int i = 0; i < 5; i++) {
  int dash = data.indexOf('-', pos);
  if (dash != -1) {
    cfg.list[i] = data.substring(pos, dash).toInt();
    pos = dash + 1;
  } else {
    cfg.list[i] = data.substring(pos).toInt(); // pastikan tetap terisi jika dash tidak ada
    break;
  }
}
  }
  saveToEEPROM();
  return;
}
//----------------------------//

if (data.startsWith("PLAY:")) {
  //lastTimeReceived = millis();
  int idx = 5;
  byte folder = getIntPart(data, idx);
  byte file   = getIntPart(data, idx);

  if (folder >= 1 && folder < 12 && file >= 1 && file < MAX_FILE) {
    uint16_t durasi = durasiTartil[folder-1][file];  // ambil dari array
    if (durasi > 0) {
      dfplayer.playFolder(folder, file);
      //============ DEBUG =============//
//      Serial.print("Memutar manual: folder "); Serial.print(folder);
//      Serial.print(", file "); Serial.print(file);
//      Serial.print(", durasi "); Serial.print(durasi); Serial.println(" detik");

      digitalWrite(RELAY_PIN, LOW);//relay NYALA
      tartilCounter         = 0;
      targetDurasi          = durasi;
      lastTick              = millis();
      manualSedangDiputar   = true;
      relayMenungguMati     = false;
    } else {
      //Serial.println("Durasi tidak ditemukan atau 0.");
    }
  }
  return;
}

//------------------------------------------------
if (data.startsWith("PLAD:")) {
  int idx = 5;
  byte file   = getIntPart(data, idx);

    uint16_t durasi = durasiAdzan[file];  // ambil dari array
    //============ DEBUG =============//
//    Serial.print("file "); Serial.print(file); Serial.print(" ");
//    Serial.print(durasi); Serial.println(" detik");
    if (durasi > 0) {
      dfplayer.playFolder(11, file);
      digitalWrite(RELAY_PIN, LOW);//relay NYALA
      adzanCounter         = 0;
      targetDurasiAdzan    = durasi;
      lastAdzanTick        = millis();
      adzanManualSedangDiputar = true;
    } 
  return;
}
//------------------------------------------------

  // --- Perintah STOP ---
  if (data.startsWith("STOP")) {
    //lastTimeReceived = millis();
    dfplayer.stop();
    digitalWrite(RELAY_PIN, HIGH);//relay mati
    relayMenungguMati = false;
    tartilSedangDiputar = false;
    adzanSedangDiputar = false;
    manualSedangDiputar = false;
    //============ DEBUG =============//
    //Serial.println("STOP: DFPlayer dan relay dimatikan");
    return;
  }

// ----------- PROGRAM BARU
if (data.startsWith("NAMAFILE:")) {
  //lastTimeReceived = millis();
  int idx = 9;
  byte folder = getIntPart(data, idx);
  byte list   = getIntPart(data, idx);
  int durasi  = getIntPart(data, idx);

  if (folder < MAX_FOLDER && list < MAX_FILE) {
    durasiTartil[folder][list] = durasi;
    //============ DEBUG =============//
//    Serial.print("Disimpan durasi tartil => Folder ");
//    Serial.print(folder); Serial.print(", List ");
//    Serial.print(list); Serial.print(", Durasi ");
//    Serial.print(durasi); Serial.println(" detik");
    saveToEEPROM();
  } else {
    //Serial.println("Folder atau List melebihi batas.");
  }
  return;
}


if (data.startsWith("ADZAN:")) {
  //lastTimeReceived = millis();
  int idx = 6;
  byte file = getIntPart(data, idx);
  int durasi = getIntPart(data, idx);
  if (file < MAX_FILE) {
    durasiAdzan[file] = durasi;
    //============ DEBUG =============//
//    Serial.print("Disimpan durasi adzan file ");
//    Serial.print(file); Serial.print(" = ");
//    Serial.print(durasi); Serial.println(" detik");
    saveToEEPROM();
  }
  return;
}

if (data.startsWith("JWS:")) {
  //lastTimeReceived = millis();
  String sisa = data.substring(4); // Hilangkan "JWS:"
  for (int i = 0; i < WAKTU_TOTAL; i++) {
    int komaIdx = sisa.indexOf(',');
    int pemisahIdx = sisa.indexOf('|');

    if (komaIdx == -1) break;
    jamSholat[i] = sisa.substring(0, komaIdx).toInt();

    if (pemisahIdx == -1) {
      // Tidak ada | berarti ini adalah elemen terakhir
      menitSholat[i] = sisa.substring(komaIdx + 1).toInt();
      break;
    } else {
      menitSholat[i] = sisa.substring(komaIdx + 1, pemisahIdx).toInt();
      sisa = sisa.substring(pemisahIdx + 1); // lanjut ke data berikutnya
    }
  }

  saveToEEPROM();
  //============ DEBUG =============//
  //Serial.println("Jadwal Sholat diperbarui:");
//  for (int i = 0; i < WAKTU_TOTAL; i++) {
//    Serial.print(" - Waktu "); Serial.print(i);
//    Serial.print(": "); Serial.print(jamSholat[i]);
//    Serial.print(":"); Serial.println(menitSholat[i]);
//  }
  return;
}

}
//============================== END =================================//

//============================== Cek Play Manual Tartil dan Adzan ==========================//
void cekSelesaiManual() {
if (manualSedangDiputar) {
  if (millis() - lastTick >= 1000) {
    lastTick = millis();
    tartilCounter++;
    //Serial.print("Counter: "); Serial.println(tartilCounter);
    
    if (tartilCounter >= targetDurasi) {
      dfplayer.stop();
      digitalWrite(RELAY_PIN, HIGH);//relay mati
      manualSedangDiputar = false;
      //Serial.println("Manual tartil selesai.");
    }
  }
}
}

void cekSelesaiAdzanManual() {
  if (adzanManualSedangDiputar) 
{
  if (millis() - lastAdzanTick >= 1000) {
    lastAdzanTick = millis();
    adzanCounter++;

    if (adzanCounter >= targetDurasiAdzan) {
      dfplayer.stop();
      digitalWrite(RELAY_PIN, HIGH);//relay mati
      adzanManualSedangDiputar = false;
      adzanCounter=0;
     // Serial.println("Adzan selesai. Relay dimatikan.");
    }
  }
}
}
//========================== END ====================================//

//========================== mengambil durasi Tartil dan Adzan ===============================//
uint16_t getDurasiTartil(byte folder, int file) {
  if (folder == 0 || folder > MAX_FOLDER || file >= MAX_FILE) return 0;
  return durasiTartil[folder - 1][file];
}

uint16_t getDurasiAdzan(int file) {
  if (file == 0 || file >= MAX_FILE) return 0;
  return durasiAdzan[file];
}
//============================= END ============================================//

//====================== cek putar waktu Tartil ==============================//
void cekDanPutarSholatNonBlocking() {
  if (tartilSedangDiputar || adzanSedangDiputar || sudahEksekusi) return;

  uint32_t detikSekarang = hour() * 3600UL + minute() * 60UL + second();  // cukup pakai uint16_t

  static bool stateJadwal = false;

  // Cetak hanya sekali pada menit tertentu
  if ((minute() == 0 || minute() == 15 || minute() == 30 || minute() == 45) && second() == 0 && !stateJadwal) {
    stateJadwal = true;
    Serial.println("jadwal");
  } else if (second() != 0) {
  stateJadwal = false;
  } 


  for (byte w = 0; w < WAKTU_TOTAL; w++) { 
    
    WaktuConfig &cfg = jadwal[currentDay][w];
    if (!cfg.aktif) continue;
    if (jamSholat[w] == 0 && menitSholat[w] == 0) continue;  // Lewati jadwal tidak valid
    
    uint16_t totalDurasi = 0;
    
    // Hitung total durasi dari file tartil
    for (byte i = 0; i < 5; i++) {
      byte f = cfg.list[i];
      if (f) {
        uint16_t d = getDurasiTartil(cfg.folder, f);
        if (d) totalDurasi += d;
      }
    }

    uint32_t jadwalDetik = jamSholat[w] * 3600UL + menitSholat[w] * 60UL;
    uint32_t triggerDetik = cfg.tartilDulu ? (jadwalDetik - totalDurasi) : jadwalDetik;
    
    
    if (triggerDetik > 86400) continue;  // Lewati jika melebihi 1 hari
   
    if (detikSekarang == triggerDetik) {
      /*/============ DEBUG =============//
      Serial.println("TRIGGER MATCH!");
      Serial.println("jam: " + String(hour()) + " " + "menit: " + String(minute()) + "detik: " + String(second()));
      Serial.println("jamSholat[w]: " + String(jamSholat[w]));
      Serial.println("menitSholat[w]: " + String(menitSholat[w]));
      Serial.println("jadwalDetik: " + String(jadwalDetik));
      Serial.println("totalDurasi: " + String(totalDurasi));
      Serial.println("triggerDetik: " + String(triggerDetik));
      Serial.println("detikSekarang: " + String(detikSekarang));
      //================================/*/
      
      digitalWrite(RELAY_PIN, LOW);//relay NYALA
      currentCfg = &cfg;
      lastTriggerMillis = millis();
      sudahEksekusi = true;

      if (cfg.tartilDulu && totalDurasi > 0) {
        tartilIndex = 0;
        tartilFolder = cfg.folder;
        tartilCounter = 0;
        tartilSedangDiputar = true;
        manualSedangDiputar = false;

        byte f = cfg.list[tartilIndex];
        targetDurasi = getDurasiTartil(tartilFolder, f);
        lastTick = millis();
        dfplayer.playFolder(tartilFolder, f);

#if DEBUG
        Serial.print("Tartil dimulai: ");
        Serial.println(f);
#endif

      } else if (cfg.aktifAdzan) {
        targetDurasiAdzan = getDurasiAdzan(cfg.fileAdzan);
        adzanCounter = 0;
        lastAdzanTick = millis();
        adzanSedangDiputar = true;
        dfplayer.playFolder(11, cfg.fileAdzan);

#if DEBUG
        Serial.print("Adzan langsung diputar: ");
        Serial.println(cfg.fileAdzan);
#endif
      }
    }
  }
}
//=================== END =================//

//==================== cek status putar Tartil =======================//
void cekSelesaiTartil() {
  if (!tartilSedangDiputar) return;

  // Jeda antar file tartil
  if (jedaAktif) {
    if (millis() - jedaMulaiMillis >= JEDA_ANTAR_TARTIL) {
      jedaAktif = false;

      if (tartilIndex < 5) {
        byte f = currentCfg->list[tartilIndex];
        if (f) {
          targetDurasi = getDurasiTartil(tartilFolder, f);
          tartilCounter = 0;
          lastTick = millis();
          dfplayer.playFolder(tartilFolder, f);
#if DEBUG
          Serial.print("Memutar tartil selanjutnya: ");
          Serial.println(f);
#endif
        } else {
          tartilIndex = 5; // skip ke akhir
        }
      } else {
        tartilSedangDiputar = false;
      }
    }
    return;
  }

  // Counter tartil per detik
  if (millis() - lastTick >= 1000) {
    lastTick = millis();
    if (++tartilCounter >= targetDurasi) {
      tartilIndex++;
      if (tartilIndex < 5) {
        if (currentCfg->list[tartilIndex]) {
          jedaAktif = true;
          jedaMulaiMillis = millis();
#if DEBUG
          Serial.println("Menunggu jeda antar file tartil...");
#endif
        } else {
          tartilIndex = 5;
        }
      } else {
        // Tartil selesai
        tartilSedangDiputar = false;
        if (currentCfg->aktifAdzan) {
          adzanCounter = 0;
          targetDurasiAdzan = getDurasiAdzan(currentCfg->fileAdzan);
          lastAdzanTick = millis();
          adzanSedangDiputar = true;
          dfplayer.playFolder(11, currentCfg->fileAdzan);
#if DEBUG
          Serial.println("Tartil selesai, memutar adzan.");
#endif
        } else {
          matikanSemuaAudio();
          //digitalWrite(RELAY_PIN, LOW);
#if DEBUG
          Serial.println("Tartil selesai, relay dimatikan.");
#endif
        }
      }
    }
  }
}
//========================== END ========================//


void matikanSemuaAudio() {
  dfplayer.stop();
  digitalWrite(RELAY_PIN, HIGH);//relay mati
  relayMenungguMati = false;
  tartilSedangDiputar = false;
  adzanSedangDiputar = false;
  manualSedangDiputar = false;
}


 void cekRelayOffDelay() {
   if (relayMenungguMati && millis() - relayOffDelayMillis >= 5000) {
     digitalWrite(RELAY_PIN, HIGH);//relay mati
     //Serial.println("cekRelayOffDelay");
     relayMenungguMati = false;
     manualSedangDiputar = false;
   }
 }

//=================== Cek status putar Adzan =======================//
void cekSelesaiAdzan() {
  if (!adzanSedangDiputar) return;

  if (millis() - lastAdzanTick >= 1000) {
    lastAdzanTick = millis();
    adzanCounter++;

    if (adzanCounter >= targetDurasiAdzan) {
      dfplayer.stop();
      digitalWrite(RELAY_PIN, HIGH);//relay mati
      adzanSedangDiputar = false;
     // Serial.println("Adzan selesai. Relay dimatikan.");
    }
  }
}
//=========================== END ==============================//

//================== indikator system ============================//
void getStatusRun() {
  uint32_t now = millis();
  if (now - lastWaveMillis >= waveStepDelay) {
    lastWaveMillis = now;
    updateWaveLED();
  }
}

void updateWaveLED() {
  // brightness naik turun dari 0 - 255 - 0
  uint8_t brightness = (m_Counter < 128) ? m_Counter * 2 : (255 - m_Counter) * 2;
  setLED(brightness);

  m_Counter = (m_Counter + 1) % 256;  // loop kembali ke 0 setelah 255
}

void setLED(uint8_t brightness) {
  analogWrite(RUN_LED, brightness);
}
//========================== END ==========================//

void RESTART(){
  Serial.println("restart");
}

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

 // EEPROM.write(addr++, EEPROM_MAGIC); // simpan MAGIC di akhir

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
      /*/============ DEBUG =============//
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
//  Serial.print("adzan["); Serial.print(i);
//  Serial.print("] = "); Serial.println(durasiAdzan[i]);
    //================================//
  }

  for (int f = 0; f < MAX_FOLDER; f++) {
    for (int i = 0; i < MAX_FILE; i++) {
      EEPROM.get(addr, durasiTartil[f][i]);
      addr += sizeof(uint16_t);  // perbaikan: harus cocok dengan penyimpanan
      //============ DEBUG =============//
//    Serial.print("Tartil["); Serial.print(f); Serial.print("]["); Serial.print(i);
//    Serial.print("] = "); Serial.println(durasiTartil[f][i]);
      //================================//
    }
  }

  EEPROM.get(addr, volumeDFPlayer);
  addr += sizeof(volumeDFPlayer);
  //============ DEBUG =============//
  //Serial.println("VOL:" + String(volumeDFPlayer));
  //================================//
  
  for (int i = 0; i < WAKTU_TOTAL; i++) {
    EEPROM.get(addr, jamSholat[i]); addr += sizeof(uint8_t);
    EEPROM.get(addr, menitSholat[i]); addr += sizeof(uint8_t);
    /*/============ DEBUG =============//
    Serial.print("jamSholat["); Serial.print(i);
    Serial.print("] = "); Serial.println(jamSholat[i]);
    Serial.print("menitSholat["); Serial.print(i);
    Serial.print("] = "); Serial.println(menitSholat[i]);
    //================================/*/
  }
}
