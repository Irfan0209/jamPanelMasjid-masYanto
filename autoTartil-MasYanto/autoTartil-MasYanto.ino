
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include "OneButton.h"

#define PASSWORD_LEN 20   // maksimal 15 karakter + '\0'

const char* ssid = "JAM_PANEL";
char password[PASSWORD_LEN] = "00000000";

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

#define RELAY_PIN         26
#define RUN_LED           13
#define NORMAL_STATUS_LED 14
#define LED_WIFI          27

#define EEPROM_SIZE 1000

#define HARI_TOTAL  8 // 7 hari + SemuaHari (index ke-7)
#define WAKTU_TOTAL 5
#define MAX_FILE    50
#define MAX_FOLDER  2 //3
#define JEDA_ANTAR_TARTIL 20 //500 jeda antar file tartil dalam milidetik

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
uint32_t lastTick = 0;

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
// uint32_t relayOffDelayMillis = 0;
// bool relayMenungguMati = false;
bool manualSedangDiputar = false;
bool adzanManualSedangDiputar = false;

//variabel untuk led status system
static uint8_t m_Counter = 0;
constexpr uint16_t waveStepDelay = 20;  // Delay antar frame LED breathing (ms)
static uint32_t lastWaveMillis = 0;

bool lastNormalStatus = false;
uint32_t lastTimeReceived = 0;
constexpr uint32_t TIMEOUT_INTERVAL = 50000; // 70 detik, lebih dari 1 menit

bool wsConnected = false;
bool wifiConnected = false;
unsigned long lastWiFiAttempt = 0;
constexpr uint32_t wifiRetryInterval = 5000;

bool autoTartilEnable = true;
bool voiceClock = true;

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
        parseData(msg);
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
  pinMode(NORMAL_STATUS_LED, OUTPUT);
  
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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lastWiFiAttempt = millis();
  
  dfplayer.volume(volumeDFPlayer);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  digitalWrite(RELAY_PIN, HIGH); // Awal mati
}

void loop() {
  if (sudahEksekusi && millis() - lastTriggerMillis > 60000) {
    sudahEksekusi = false;
  }
  cekDanPutarSholatNonBlocking();
  cekSelesaiTartil();
  cekSelesaiAdzan();
  cekSelesaiAdzanManual();
  cekSelesaiManual();
  getStatusRun();
  cekStatusSystem();
  //bacaDataSerial();

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

// void bacaDataSerial() {
//   static String buffer = "";
//   while (Serial.available()) {
//     char c = Serial.read();
//     //Serial.print(c); // DEBUG: tampilkan semua karakter yang diterima
//     if (c == '\n') {
//       //Serial.println("\n>> Memanggil parseData()");
//       parseData(buffer);
//       buffer = "";
//     } else {
//       buffer += c;
//     }
//   }
// }

//================= parsing data dari Akses Point =========================//
int getIntPart(String &s, int &pos) {
  int comma = s.indexOf(',', pos);
  if (comma == -1) comma = s.length();
  int val = s.substring(pos, comma).toInt();
  pos = comma + 1;
  return val;
}

void parseData(String data) {
Serial.print(F("data=" )); Serial.println(data);

lastTimeReceived = millis();
 // --- Parsing TIME ---
if (data.startsWith("TIME:")) {
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
  else if (data.startsWith("VOL:")) {
    //lastTimeReceived = millis();
    volumeDFPlayer = data.substring(4).toInt();
    //Serial.println("volume:" + String(volumeDFPlayer));
    dfplayer.volume(volumeDFPlayer);
    saveToEEPROM();
    return;
  }

//---- Program baru------//
  // --- Parsing HR (jadwal harian) ---
else if (data.startsWith("HR:")) {
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

else if (data.startsWith("PLAY:")) {
  //lastTimeReceived = millis();
  int idx = 5;
  byte folder = getIntPart(data, idx);
  byte file   = getIntPart(data, idx);

  if (folder >= 1 && folder < 12 && file >= 1 && file < MAX_FILE) {
    uint16_t durasi = durasiTartil[folder-1][file];  // ambil dari array
    if (durasi > 0) {
      //dfplayer.playFolder(folder, file);
      dfplayer.volume(volumeDFPlayer);
      dfplayer.playFolder(1,file);
      //============ DEBUG =============//
//      Serial.print("Memutar manual: folder "); Serial.print(folder);
//      Serial.print(", file "); Serial.print(file);
//      Serial.print(", durasi "); Serial.print(durasi); Serial.println(" detik");

      digitalWrite(RELAY_PIN, LOW);//relay NYALA
      tartilCounter         = 0;
      targetDurasi          = durasi;
      lastTick              = millis();
      manualSedangDiputar   = true;
      //relayMenungguMati     = false;
    } else {
      //Serial.println("Durasi tidak ditemukan atau 0.");
    }
  }
  return;
}

//------------------------------------------------
else if (data.startsWith("PLAD:")) {
  int idx = 5;
  byte file   = getIntPart(data, idx);

    uint16_t durasi = durasiAdzan[file];  // ambil dari array
    //============ DEBUG =============//
//    Serial.print("file "); Serial.print(file); Serial.print(" ");
//    Serial.print(durasi); Serial.println(" detik");
    if (durasi > 0) {
      //dfplayer.playFolder(11, file);
      dfplayer.volume(volumeDFPlayer);
      dfplayer.playFolder(1,file);
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
else if (data.startsWith("STOP")) {
    //lastTimeReceived = millis();
    dfplayer.stop();
    digitalWrite(RELAY_PIN, HIGH);//relay mati
    //relayMenungguMati = false;
    tartilSedangDiputar = false;
    adzanSedangDiputar = false;
    manualSedangDiputar = false;
    //============ DEBUG =============//
    //Serial.println("STOP: DFPlayer dan relay dimatikan");
    return;
  }

// ----------- PROGRAM BARU
else if (data.startsWith("NAMAFILE:")) {
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


else if (data.startsWith("ADZAN:")) {
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

else if (data.startsWith("JWS:")) {
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
}

  // --- Parsing At (Auto Tartil) ---
else if (data.startsWith("At:")) {
  autoTartilEnable = data.substring(3).toInt();
  // Serial.print("AutoTartil: ");
  // Serial.println(autoTartilEnable);
  return;
}

// --- Parsing newPassword ---
else if (data.startsWith("newPassword=")) {
  String pwd = data.substring(12);

  if (pwd.length() == 8) {
    pwd.toCharArray(password, 9); // copy aman

    // Serial.print("Password baru diterima: ");
    // Serial.println(password);

    saveToEEPROM();   // simpan di EEPROM ESP8266 (bukan ESP-01)
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Password invalid (harus 8 karakter)");
  }
  return;
}

  saveToEEPROM();
  //============ DEBUG =============//
  

data="";
}
//============================== END =================================//
