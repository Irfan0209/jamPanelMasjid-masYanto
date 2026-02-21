
//=================== new variabel ========================//
char * const pasar[]  = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
char * const Hari[]  = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
//const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char* jadwal[] = {"IMSAK","SUBUH", "TERBT", "DUHUR", "ASHAR", "MAGRB", "ISYA'"};
char* jadwalAzzan[] = {"SUBUH","DZUHUR", "ASHAR", "MAGRIB", "ISYA'"};
char * namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};

struct SholatAnim {
  uint8_t  phase;      // IN / HOLD / OUT
  uint8_t  sNum;       // index sholat
  uint8_t  x;          // posisi animasi
  uint32_t timer;
};

struct MasehiAnim {
  uint8_t  phase;      // IN / HOLD / OUT
  uint8_t  sNum;       // index sholat
  uint8_t  x;          // posisi animasi
  uint32_t timer;
};

SholatAnim shAnim;
MasehiAnim msAnim;

#define SHOLAT_COUNT 7
#define PHASE_IN     0
#define PHASE_HOLD   1
#define PHASE_OUT    2

void initAnimSholat() {
  shAnim.phase = PHASE_IN;
  shAnim.sNum  = 0;
  shAnim.x     = 0;
  shAnim.timer = millis();
  Disp.clear();
}

void updateAnimSholat() {

  uint32_t now = millis();
  const uint8_t center = 8;   // 32x16 panel

  switch (shAnim.phase) {

    // ====== MASUK ======
    case PHASE_IN:
      if (now - shAnim.timer > 50) {
        shAnim.timer = now;
        if (shAnim.x < center) shAnim.x++;
        else shAnim.phase = PHASE_HOLD;
      }
      break;

    // ====== TAHAN ======
    case PHASE_HOLD:
      if (now - shAnim.timer > 2000) {
        shAnim.phase = PHASE_OUT;
      }
      break;

    // ====== KELUAR ======
    case PHASE_OUT:
      if (now - shAnim.timer > 50) {
        shAnim.timer = now;
        if (shAnim.x > 0) shAnim.x--;
        else {
          shAnim.sNum++;
          if (shAnim.sNum >= SHOLAT_COUNT) {
            line = ANIM_MASEHI;
            shAnim.sNum=0;
            return;
          }
          shAnim.phase = PHASE_IN;
        }
      }
      break;
  }

  drawSholatFrame(shAnim.sNum, shAnim.x-center);
}

void drawSholatFrame(uint8_t sNum, int8_t x) {

  if (sNum >= SHOLAT_COUNT) return;

  float sholatT[]={JWS.floatImsak,JWS.floatSubuh,JWS.floatTerbit,JWS.floatDzuhur,JWS.floatAshar,JWS.floatMaghrib,JWS.floatIsya};

 // Disp.clear();

  // ===== JAM =====
//  RtcDateTime now = Rtc.GetDateTime();
//  char h[3], m[3];
//  snprintf(h, sizeof(h), "%02d", now.Hour());
//  snprintf(m, sizeof(m), "%02d", now.Minute());

  // ===== SHOLAT =====
  float st = sholatT[sNum];
  uint8_t hh = (uint8_t)st;
  uint8_t mm = (uint8_t)((st - hh) * 60);

  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", hh, mm);

  fType(1);
  Disp.drawText(33, x, jadwal[sNum]);
  Disp.drawText(65, x, timeBuf);

  //DoSwap = true;
}

void initAnimMasehi() {
  msAnim.phase = PHASE_IN;
  msAnim.x     = 0;
  msAnim.timer = millis();
  Disp.clear();
}

void updateAnimUpDown(const char* msg) {

  uint32_t now = millis();
  const uint8_t center = 8;   // 32x16 panel

  switch (msAnim.phase) {

    // ====== MASUK ======
    case PHASE_IN:
      if (now - msAnim.timer > 50) {
        msAnim.timer = now;
        if (msAnim.x < center) msAnim.x++;
        else msAnim.phase = PHASE_HOLD;
      }
      break;

    // ====== TAHAN ======
    case PHASE_HOLD:
      if (now - msAnim.timer > 2000) {
        msAnim.phase = PHASE_OUT;
      }
      break;

    // ====== KELUAR ======
    case PHASE_OUT:
      if (now - msAnim.timer > 50) {
        msAnim.timer = now;
        if (msAnim.x > 0) msAnim.x--;
        else {
          if(line == ANIM_MASEHI){ line = ANIM_DAY; }
          else if(line == ANIM_DAY){ line = ANIM_SHOLAT; }
          msAnim.phase = PHASE_IN;
          return;
        }
      }
      break;
  }

  //drawFrame(msg,);
  fType(1);
  Disp.drawText(33,msAnim.x-center,msg);
 
  //DoSwap = true;
}


//==================== animasi jam dan running text =================//

void jamCenter(){
//    if(adzan) return;
//    //if(!dwDo(DrawAdd)) return; 
    RtcDateTime now = Rtc.GetDateTime();
  Disp.drawFilledRect(0, 0, 31, 16, 0);
  if(now.Second() % 2 ){
      Disp.drawCircle(15,4,1,1);
      Disp.drawCircle(15,11,1,1);
    }else{
      Disp.drawCircle(15,4,1,0);
      Disp.drawCircle(15,11,1,0);
    }

  fType(3);
  
  Disp.drawChar(0, 0, '0' + now.Hour() / 10);
  Disp.drawChar(7, 0, '0' + now.Hour() % 10); 
  
  Disp.drawChar(18, 0, '0' + now.Minute() / 10);
  Disp.drawChar(25, 0, '0' + now.Minute() % 10);
 
}

void runn(const char* msg, uint8_t speed, uint8_t fontt)
{
  static uint32_t x = 0;
  static uint32_t fullScroll = 0;
  static uint32_t lastMs = 0;
  static const char* lastMsg = nullptr;

  // ====== Pesan baru → reset animasi ======

 fType(fontt);
  // ====== Pesan kosong → langsung lompat ======
  uint16_t w = Disp.textWidth(msg);
  if (w == 0) {
    nextShowState();
    return;
  }

  // ====== Hitung panjang scroll hanya sekali ======
    fullScroll = w + DWidth;

  uint32_t now = millis();
  if (now - lastMs < speed) return;
  lastMs = now;

  // ====== Animasi scroll ======
  if (x < fullScroll) {
    x++;
  } else {
    x = 0;
    fullScroll = 0;
    nextShowState();
    return;
  }

  Disp.drawText(
    DWidth - x,
    (fontt == 5) ? 0 : 8,
    msg
  );
  DoSwap = true;
}

void nextShowState()
{
  switch(show){
    case ANIM_BIGFONT: show = ANIM_DATE;  line = ANIM_SHOLAT; break;
    case ANIM_DATE:    show = ANIM_TEXT1; break;
    case ANIM_TEXT1:   show = ANIM_TEXT2; break;
    case ANIM_TEXT2:   show = ANIM_TEXT3; break;
    case ANIM_TEXT3:   show = ANIM_TEXT4; break;
    case ANIM_TEXT4:   show = ANIM_TEXT5; break;
    case ANIM_TEXT5:   show = ANIM_COUNTER; break;
    case ANIM_COUNTER:   show = ANIM_BIGFONT; line = ANIM_ZONK; break;
  }
}



//==================== animasi jam dan running text =================//
void dwMrq(const char* msg, byte Speed, byte dDT,byte fontt) //running teks ada jam nya
  { 
    static uint32_t   x; 
    static uint32_t fullScroll = 0;
    if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0; fullScroll = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    
    
     
    if (fullScroll == 0) { // Hitung hanya sekali
       fType(fontt);
       (fontt == 5)? fullScroll = Disp.textWidth(msg) + DWidth + 20 : fullScroll = Disp.textWidth(msg) + DWidth ; 
    }   
    
    
    
    if((Tmr-lss)> Speed)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          RtcDateTime now = Rtc.GetDateTime();
          if(show==ANIM_BIGFONT){show=ANIM_BIGFONT; Serial.println("TIME:" + String(now.Hour()) + "," + String(now.Minute()) + "," + String(now.Second()) + "," + String(now.DayOfWeek()));}
          //else if(show==ANIM_BIGFONT){show=ANIM_BIGFONT; Serial.println("TIME:" + String(now.Hour()) + "," + String(now.Minute()) + "," + String(now.Second()) + "," + String(now.DayOfWeek()));}
         // else if(show==ANIM_BIG){show=ANIM_DATE;}
          x = 0; 
          fullScroll = 0;
          return;}
     if(dDT==1)
        {
        //fType(1);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
        //fType(1);
        if (x<=6)                     { drawGreg_TS(16-x);}
        else if (x>=(fullScroll-6))   { drawGreg_TS(16-(fullScroll-x));}
        else                          { drawGreg_TS(9);}//posisi jamnya yang bawah
   
        }
     else if(dDT==2) //jam yang diatas
        {    
        //fType(1);
        if (x<=6)                     { drawGreg_TS(x-6);}
        else if (x>=(fullScroll-6))   { drawGreg_TS((fullScroll-x)-6);}
        else                          { drawGreg_TS(0);}  //posisi jam nya yang diatas
        //fType(1); //Marquee  running teks dibawah
        Disp.drawText(DWidth - x, 9 , msg);//runinng teks dibawah
        
        }
      else if(dDT==3) //jam yang diatas
      {
        //fType(1);  //Marquee    jam yang tampil di bawah
        //drawGreg_TS(4);
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
        Serial.println("x:" + String(x));
      }
        DoSwap = true;
      }          
     
  }

void drawGreg_TS(int y)   // Draw Time
  {
    RtcDateTime now = Rtc.GetDateTime();
    char  Buff[8];
    sprintf(Buff,"%02d:%02d",now.Hour(),now.Minute());
    Disp.drawText(0,y,Buff);
    //DoSwap = true;
  }

//===================== end ========================//

void dwCtr(int x, int y, String Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   uint16_t   c = int((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
  
void fType(int x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
    else if(x==5) Disp.setFont(Font5);
    else if(x==6) Disp.setFont(Font6); 
    else if(x==7) Disp.setFont(Font7); 
  }
