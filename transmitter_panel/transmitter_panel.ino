// ESP8266 Server with OTA Upload Mode
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

char ssid[20]     = "JAM_PANEL";
char password[20] = "00000000";

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

IPAddress local_IP(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

bool clientReady[5] = { false, false, false, false, false };
bool modeOTA = false;

bool stateRestart  = false;
String pass;
bool passwordReady = false;

void getData(const char* input) {
  Serial.println(input);
  // Di sini bisa tambahkan pengolahan data lebih lanjut
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      clientReady[num] = false;
      break;

    case WStype_DISCONNECTED:
      clientReady[num] = false;
      break;

    case WStype_TEXT: {
      // Kita tetap gunakan String sementara untuk kemudahan parsing
      String msg = String((char*)payload);

      if (msg == "CLIENT_READY") {
        clientReady[num] = true;
      } 
      else if (msg == "restart") {
        // PERBAIKAN: Gunakan String sementara untuk penggabungan, lalu kirim .c_str()
        String temp = msg + "=1";
        getData(temp.c_str()); 
        
        delay(500);
        ESP.restart();
      } 
      else if (msg == "jadwal") {
        String temp = msg + "=1";
        getData(temp.c_str());
      } 
      else {
        // PERBAIKAN: Kirim isi String msg sebagai const char*
        getData(msg.c_str());
      }
      break;
    }
  }
}

void handleSetTime() {
  //String data = "";
  char dataBuffer[250];
  
   if (server.hasArg("Tm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Tm=%s", server.arg("Tm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan jam berhasil diupdate");
    return;
  }
  if (server.hasArg("text")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "text=%s", server.arg("text").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan text berhasil diupdate");
    return;
  }
  if (server.hasArg("name")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "name=%s", server.arg("name").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan nama berhasil diupdate");
    return;
  }
  if (server.hasArg("Br")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Br=%s", server.arg("Br").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecerahan berhasil diupdate");
    return;
  }
  if (server.hasArg("Spdt")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Spdt=%s", server.arg("Spdt").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan kalender berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx1")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx1=%s", server.arg("Sptx1").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 1 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx2")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx2=%s", server.arg("Sptx2").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx3")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx3=%s", server.arg("Sptx3").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx4")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx4=%s", server.arg("Sptx4").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx5")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx5=%s", server.arg("Sptx5").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Spnm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Spnm=%s", server.arg("Spnm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan nama berhasil diupdate");
    return;
  }
  if (server.hasArg("Iq")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Iq=%s", server.arg("Iq").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"iqomah diupdate");
    return;
  }
  if (server.hasArg("Dy")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Dy=%s", server.arg("Dy").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"displayBlink diupdate");
    return;
  }
  if (server.hasArg("Kr")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Kr=%s", server.arg("Kr").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Selisih jadwal sholat diupdate");
    return;
  }
  if (server.hasArg("Lt")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Lt=%s", server.arg("Lt").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"latitude diupdate");
    return;
  }
  if (server.hasArg("Lo")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Lo=%s", server.arg("Lo").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"longitude diupdate");
    return;
  }
  if (server.hasArg("Tz")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Tz=%s", server.arg("Tz").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"timezone diupdate");
    return;
  }
  if (server.hasArg("Al")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Al=%s", server.arg("Al").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"altitude diupdate");
    return;
  }
  if (server.hasArg("Da")) { 
    snprintf(dataBuffer, sizeof(dataBuffer), "Da=%s", server.arg("Da").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");// "durasi adzan diupdate");
    return;
  }
  if (server.hasArg("CoHi")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "CoHi=%s", server.arg("CoHi").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"coreksi hijriah diupdate");
    return;
  }

  if (server.hasArg("Bzr")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Bzr=%s", server.arg("Bzr").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("bzrClk")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "bzrClk=%s", server.arg("bzrClk").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarm=%s", server.arg("alarm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarmOn")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarmOn=%s", server.arg("alarmOn").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarmOff")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarmOff=%s", server.arg("alarmOff").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("mode")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "mode=%s", server.arg("mode").c_str());
    getData(dataBuffer);
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("PLAY")) {
    // 1. Ambil data mentah sebagai String hanya untuk parsing
    String mentah = server.arg("PLAY"); 
    
    int idx = 0;
    byte folder = getIntPart(mentah, idx);
    byte file   = getIntPart(mentah, idx);

    // 2. Format data langsung ke dalam dataBuffer
    // %d adalah placeholder untuk integer/byte
    snprintf(dataBuffer, sizeof(dataBuffer), "PLAY:%d,%d", folder, file);

    // 3. Kirim data yang sudah rapi di buffer ke client
    kirimDataKeClient(dataBuffer); 

    server.send(200, "text/plain", "OK");
    return; // Keluar dari fungsi agar lebih efisien
}

// --- PLAD ---
  if (server.hasArg("PLAD")) {
    String mentah = server.arg("PLAD");
    int idx = 0;
    byte file = getIntPart(mentah, idx);
    snprintf(dataBuffer, sizeof(dataBuffer), "PLAD:%d", file);
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- STOP ---
  if (server.hasArg("STOP")) {
    kirimDataKeClient("STOP"); // Langsung kirim teks statis
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- VOL ---
  if (server.hasArg("VOL")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "VOL:%s", server.arg("VOL").c_str());
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- HR ---
  if (server.hasArg("HR")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "HR:%s", server.arg("HR").c_str());
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- NAMAFILE ---
  if (server.hasArg("NAMAFILE")) {
    String mentah = server.arg("NAMAFILE");
    int idx = 0;
    byte folder = getIntPart(mentah, idx);
    byte file   = getIntPart(mentah, idx);
    int durasi  = getIntPart(mentah, idx);
    snprintf(dataBuffer, sizeof(dataBuffer), "NAMAFILE:%d,%d,%d", folder, file, durasi);
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- ADZAN ---
  if (server.hasArg("ADZAN")) {
    String mentah = server.arg("ADZAN");
    int idx = 0;
    byte file  = getIntPart(mentah, idx);
    int durasi = getIntPart(mentah, idx);
    snprintf(dataBuffer, sizeof(dataBuffer), "ADZAN:%d,%d", file, durasi);
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- At ---
  if (server.hasArg("At")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "At:%s", server.arg("At").c_str());
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- Vc ---
  if (server.hasArg("Vc")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Vc:%s", server.arg("Vc").c_str());
    kirimDataKeClient(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }
  
  if (server.hasArg("status")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "%s","status=1");
    getData(dataBuffer);
    server.send(200, "text/plain", "CONNECTED");
     return;
  }
 
 if (server.hasArg("newPassword")) {
      // 1. Ambil password baru dari argumen server
      String passwordBaru = server.arg("newPassword");

      // 2. Format untuk kirim ke serial/monitor (dataBuffer)
      snprintf(dataBuffer, sizeof(dataBuffer), "newPassword=%s", passwordBaru.c_str());
      
      // 3. Simpan ke variabel global 'pass' untuk digunakan fungsi Restart nanti
      // Kita langsung isi, tidak perlu ditambah-tambah (+) agar tidak menumpuk
      pass = dataBuffer; 

      getData(dataBuffer);
      kirimDataKeClient(dataBuffer);

      // 4. Picu proses restart
      stateRestart = true;
      
      server.send(200, "text/plain", "OK");
      return;
  }
  
  //EEPROM.commit();
}

int getIntPart(String &s, int &pos) {
  int comma = s.indexOf(',', pos);
  if (comma == -1) comma = s.length();
  int val = s.substring(pos, comma).toInt();
  pos = comma + 1;
  return val;
}


void AP_init() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  server.on("/setPanel", handleSetTime);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void kirimDataKeClient(String data) {

  for (uint8_t i = 0; i < 5; i++) {
    if (clientReady[i] && webSocket.clientIsConnected(i)) {
      webSocket.sendTXT(i, data);
    } else {
      clientReady[i] = false;
    }
  }
}

void cekSerialMonitor() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    //Serial.print("[Serial] Kirim ke semua client: ");
    //Serial.println(input);
    for (uint8_t i = 0; i < 5; i++) {
      if (clientReady[i] && webSocket.clientIsConnected(i)) {
        webSocket.sendTXT(i, input);
      }
    }
  }
}

void waitPasswordFromESP8266() {
  Serial.println("WAIT_PASSWORD");

  unsigned long tStart = millis();

  while (!passwordReady) {

    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();

      // Format wajib: PWD=12345678
      if (line.startsWith("PWD=")) {
        String pwd = line.substring(4);

        if (pwd.length() == 8) {
          pwd.toCharArray(password, 9);   // simpan ke buffer password
          passwordReady = true;

          Serial.println(F("PWD_OK"));       // ACK ke ESP8266
        }
      }
    }

    // watchdog safety
    yield();
  }

//  Serial.print("Password diterima: ");
//  Serial.println(password);
}

void setup() {
  Serial.begin(9600);

  waitPasswordFromESP8266();  // ⬅️ GATE WAJIB

  AP_init();                  // baru jalan SETELAH password siap
//  Serial.println("SYSTEM RUN");
}

void loop() {
    server.handleClient();
    webSocket.loop();
    cekSerialMonitor();
    Restart(pass.c_str());
}

void Restart(const char* msg){
  if(!stateRestart) return;

  static uint32_t sv = 0;

  if(millis() - sv > 5000){
    sv = millis();
    stateRestart = false;
    getData(msg);
    delay(100);
    ESP.restart();
  }
}
