
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

SholatAnim shAnim;

#define SHOLAT_COUNT 7
#define PHASE_IN     0
#define PHASE_HOLD   1
#define PHASE_OUT    2

//===================    end       ========================//

// =========================================
// Drawing Content Block====================    
// =========================================

//================ animasi adzan ====================//
void drawAzzan()
  {
   // if(adzan) return;
    RtcDateTime now = Rtc.GetDateTime();
    static const char *jadwal[] = {"SUBUH", "DZUHUR", "ASHAR", "MAGHRIB","ISYA'"};
    const char *sholat = jadwal[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = config.durasiadzan;
    char buff_jam[10];
    char buff_sec[2];
    char buff_text1[10];
    char buff_text2[10];
  
    sprintf(buff_jam,"%02d:%02d",now.Hour(),now.Minute());
    sprintf(buff_sec,"%02d  ",now.Second());
    sprintf(buff_text1,"%s","ADZAN");
    sprintf(buff_text2,"%s",jadwalAzzan[sholatNow]);  
   
   if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
          fType(1); Disp.drawText(2,0,buff_text1);
          fType(1); Disp.drawText(1,8,buff_text2);
          Buzzer(1);
        }
        else 
          {
            Buzzer(0);
          }
         DoSwap = true;
        ct++;
    }
      fType(1); dwCtr(42,9,buff_sec);
      fType(1); dwCtr(34,0,buff_jam);
       //DoSwap = true;
   if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        adzan = 0;
        show = ANIM_CLOCK_BIG;
        ct = 0;
        Buzzer(0);
    }
  }
//===================== end ========================//

//=================== animasi hari ====================//
void drawDays()
  {
    if(adzan) return;
    static uint8_t    x;
    static uint8_t    s; // 0=in, 1=out
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();
    uint8_t           DrawWd=DWidth;   //DWidth - c;    
    uint8_t mid = DWidth / 2;

    if((Tmr-lsRn)>10) 
      {
        if(s==0 and x<(DrawWd/2)){x++;lsRn=Tmr;}
        if(s==1 and x>0){x--;lsRn=Tmr;}
      }
      
    if((Tmr-lsRn)>4000 and x ==(DrawWd/2)) {s=1;}
    if (x == 0 and s==1) 
      { 
        show = ANIM_SHOLAT;
        s=0;
        Disp.clear();
      }


    showDays();

//    Disp.drawFilledRect(0,0,DrawWd/2-x,15,0);
//    Disp.drawFilledRect(DrawWd/2+x,0,66,15,0);
// mask
  Disp.drawFilledRect(0, 0, mid - x, 15, 0);
  Disp.drawFilledRect(mid + x, 0, DWidth - 1, 15, 0);
  }
//===================== end ========================//

//==================== animasi tanggal hijriah =================//
  void drawDateHijriah()
  {
    if(adzan) return;
    static uint8_t    x;
    static uint8_t    s; // 0=in, 1=out 
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();
    byte              DrawWd=DWidth;    
    uint8_t mid = DWidth / 2;

    if((Tmr-lsRn)>10) 
      {
        if(s==0 and x<(DrawWd/2)){x++;lsRn=Tmr;}
        if(s==1 and x>0){x--;lsRn=Tmr;}
      }
      
    if((Tmr-lsRn)>3000 and x ==(DrawWd/2)) {s=1;}
    if (x == 0 and s==1) 
      { 
        show = ANIM_MASEHI;
        Disp.clear();
        s=0;
      }

    showHijriah();

// mask
  Disp.drawFilledRect(0, 0, mid - x, 15, 0);
  Disp.drawFilledRect(mid + x, 0, DWidth - 1, 15, 0);

  }
  
//===================== end ========================//
//================= animasi jadwal sholat new =================//
void updateAnimSholat() {
  if(adzan) return;
  
  RtcDateTime now = Rtc.GetDateTime();
  static int y = 0, y1 = 0;
  static uint8_t s = 0, s1 = 0;
  static bool run = false;

  static uint32_t lsRn_y1 = 0;
  static uint32_t lsRn_y = 0;
  static uint32_t tHold = 0;
  static uint8_t list = 0;

  uint32_t Tmr = millis();

  // Pilih waktu sholat sesuai list
  float stime;
  switch (list) {
    case 0: stime = JWS.floatImsak; break;
    case 1: stime = JWS.floatSubuh; break;
    case 2: stime = JWS.floatTerbit; break;
    case 3: stime = JWS.floatDzuhur; break;
    case 4: stime = JWS.floatAshar; break;
    case 5: stime = JWS.floatMaghrib; break;
    case 6: stime = JWS.floatIsya; break;
    default: stime = 0; break;
  }

  // Transisi vertikal y1 (jam muncul/hilang)
  if ((Tmr - lsRn_y1) > 55) {
    lsRn_y1 = Tmr;

    if (s1 == 0 && y1 < 17) {
      y1++;
    } else if (s1 == 1 && y1 > 0) {
      y1--;
    }
  }
  //Serial.println("y1:" + String(y1));
  // Saat y1 selesai muncul, mulai animasi jadwal
  if (y1 == 17 && s1 == 0) {
    run = true; 
    if(now.Second() % 2 ){
      Disp.drawRect(14, 3, 15, 5, 1); //posisi y = 6
      Disp.drawRect(14, 10, 15, 12, 1); //posisi y = 9
    }else{
      Disp.drawRect(14, 3, 15, 5, 0); //posisi y = 5
      Disp.drawRect(14, 10, 15, 12, 0); //posisi y = 8
    }
  }

  // Animasi gerakan teks (y)
  if (run && (Tmr - lsRn_y) > 55) {
    lsRn_y = Tmr;

    if (s == 0 && y < 9) {
      y++;
    } else if (s == 1 && y > 0) {
      y--;
    }
  }

  // Delay sebelum animasi keluar (reverse)
  if (y == 9 && s == 0 && tHold == 0) {
    tHold = millis();
  }
  if (tHold > 0 && (millis() - tHold > 1500)) {//4000
    s = 1;     // mulai keluar
    tHold = 0; // reset timer
  }

  // Setelah animasi selesai
  if (y == 0 && s == 1) {
    s = 0;
    list = (list + 1) % 7;
    if (list == 0) {
      run = false;
      s1 = 1; // trigger keluar vertikal
      Disp.drawRect(14, 4, 15, 5, 0); //posisi y = 5
      Disp.drawRect(14, 11, 15, 12, 0); //posisi y = 8
    }
  }

// Tampilkan jam digital
  fType(3);
  Disp.drawChar(0, y1 - 17, '0' + now.Hour() / 10); //12
  Disp.drawChar(7, y1 - 17, '0' + now.Hour() % 10);

  Disp.drawChar(17, y1 - 17, '0' + now.Minute() / 10);
  Disp.drawChar(24, y1 - 17, '0' + now.Minute() % 10);

  // Tampilkan teks jadwal sholat
  uint8_t shour = (uint8_t)stime;
  uint8_t sminute = (uint8_t)((stime - shour) * 60);

  char buf[6];
  buf[0] = '0' + shour / 10;
  buf[1] = '0' + shour % 10;
  buf[2] = ':';
  buf[3] = '0' + sminute / 10;
  buf[4] = '0' + sminute % 10;
  buf[5] = '\0';

  fType(1);
  dwCtr(31, y - 9, jadwal[list]);
  dwCtr(32, 18 - y, buf);
  DoSwap = true;
  if (y1 == 0 && s1 == 1) {
    s1 = 0;
    show = ANIM_TEXT1; // ganti mode jika perlu
  }
}

//==================== end =========================//

/*
void initAnimSholat() {
  shAnim.phase = PHASE_IN;
  shAnim.sNum  = 0;
  shAnim.x     = 0;
  shAnim.timer = millis();
  Disp.clear();
}

void updateAnimSholat() {

  uint32_t now = millis();
  const uint8_t center = 32;   // 32x16 panel

  switch (shAnim.phase) {

    // ====== MASUK ======
    case PHASE_IN:
      if (now - shAnim.timer > 15) {
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
      if (now - shAnim.timer > 15) {
        shAnim.timer = now;
        if (shAnim.x > 0) shAnim.x--;
        else {
          shAnim.sNum++;
          if (shAnim.sNum >= SHOLAT_COUNT) {
            show = ANIM_TEXT1;
            shAnim.sNum=0;
            return;
          }
          shAnim.phase = PHASE_IN;
        }
      }
      break;
  }

  drawSholatFrame(shAnim.sNum, shAnim.x);
}

void drawSholatFrame(uint8_t sNum, uint8_t x) {

  if (sNum >= SHOLAT_COUNT) return;

  float sholatT[]={JWS.floatImsak,JWS.floatSubuh,JWS.floatTerbit,JWS.floatDzuhur,JWS.floatAshar,JWS.floatMaghrib,JWS.floatIsya};

  Disp.clear();

  // ===== JAM =====
  RtcDateTime now = Rtc.GetDateTime();
  char h[3], m[3];
  snprintf(h, sizeof(h), "%02d", now.Hour());
  snprintf(m, sizeof(m), "%02d", now.Minute());

  fType(3);
  Disp.drawText(0, 0, h);
  Disp.drawText(19, 0, m);

  Disp.drawRect(15, 3, 16, 5, 1);
  Disp.drawRect(15, 10, 16, 12, 1);

  // ===== SHOLAT =====
  float st = sholatT[sNum];
  uint8_t hh = (uint8_t)st;
  uint8_t mm = (uint8_t)((st - hh) * 60);

  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", hh, mm);

  fType(1);
  dwCtr(33, 0, jadwal[sNum]);
  dwCtr(33, 9, timeBuf);

  // ===== MASK =====
//  Disp.drawFilledRect(0, 0, x, 15, 0);
//  Disp.drawFilledRect(63 - x, 0, 63, 15, 0);

  //Disp.drawFilledRect(0,0,32-x,15,0);
  Disp.drawFilledRect(32+x,0,63,15,0);

  DoSwap = true;
}*/

//================== animasi jam besar ==================//
void anim_JG()
  {
    if(adzan) return;
    
    RtcDateTime now = Rtc.GetDateTime();
    char  BuffJ[6];
    char  BuffM[6];
    char  BuffD[6];
    uint8_t daynow = now.DayOfWeek();
    
    static byte    y;
    static bool    s; // 0=in, 1=out              
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();

    if((Tmr-lsRn)>75) 
      { 
        if(s==0 and y<17){Disp.clear();lsRn=Tmr; y++;}
        if(s==1 and y>0){Disp.clear();lsRn=Tmr; y--; }
      }
    

    sprintf(BuffJ,"%02d",now.Hour());
    sprintf(BuffM,"%02d",now.Minute());
    sprintf(BuffD,"%02d",now.Second());

    fType(3);
    Disp.drawText(2,17-y,BuffJ);  //tampilkan jam 1
    Disp.drawText(25,y-17,BuffM);  //tampilkan menit
    Disp.drawText(67-y,0,BuffD);  //tampilkan detik //x=50  67
    
    if (y==17)
      {
        Disp.drawRect(20,0+3,18,0+5,1);
        Disp.drawRect(20,0+10,18,0+12,1);

         Disp.drawRect(45,0+3,43,0+5,1);
         Disp.drawRect(45,0+10,43,0+12,1);
      }

    if((Tmr-lsRn)>5000 and y ==17) {s=1;}
    if (y == 0 and s==1) {Disp.clear();Serial.println("TIME:" + String(BuffJ) + "," + String(BuffM) + "," + String(BuffD) + "," + String(daynow)); s=0; show=ANIM_HIJRIAH;}//show=ANIM_HIJRIAH;}//dwDone(DrawAdd);
    DoSwap = true; 
    }


//===================== end ========================//

//==================== animasi jam dan running text =================//
void dwMrq(const char* msg, int Speed, int dDT) //running teks ada jam nya
  { 
    static uint16_t   x; 
    if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    static uint16_t fullScroll = 0;
    
    
    if (fullScroll == 0) { // Hitung hanya sekali
       fullScroll = Disp.textWidth(msg) + DWidth ; 
    }   
    
    
    
    if((Tmr-lss)> Speed)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          if(show == ANIM_MASEHI){ show = ANIM_DAY; }
          else if(show == ANIM_TEXT1){ show = ANIM_TEXT2; }
          else if(show == ANIM_TEXT2){ show = ANIM_CLOCK_BIG; }            
          x = 0; 
          fullScroll = 0;
          return;}
     if(dDT==1)
        {
        fType(7);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
        fType(1);
        if (x<=6)                     { drawGreg_TS(16-x);}
        else if (x>=(fullScroll-6))   { drawGreg_TS(16-(fullScroll-x));}
        else                          { drawGreg_TS(9);}//posisi jamnya yang bawah
   
        }
     else if(dDT==2) //jam yang diatas
        {    
        fType(1);
        if (x<=6)                     { drawGreg_TS(x-6);}
        else if (x>=(fullScroll-6))   { drawGreg_TS((fullScroll-x)-6);}
        else                          { drawGreg_TS(0);}  //posisi jam nya yang diatas
        fType(7); //Marquee  running teks dibawah
        Disp.drawText(DWidth - x, 9 , msg);//runinng teks dibawah
        
        }
        DoSwap = true;
      }          
     
  }

void drawGreg_TS(int y)   // Draw Time
  {
    RtcDateTime now = Rtc.GetDateTime();
    char  Buff[8];
    sprintf(Buff,"%02d:%02d:%02d",now.Hour(),now.Minute(),now.Second());
    dwCtr(0,y,Buff);
    DoSwap = true;
  }

//===================== end ========================//

void dwCtr(int x, int y, String Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   uint16_t   c = int((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
  
void fType(int x)
  {
    //if(x==0) Disp.setFont(Font0);
    if(x==1) Disp.setFont(Font1); 
    //else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    //else if(x==4) Disp.setFont(Font4);
    else if(x==5) Disp.setFont(Font5);
    //else if(x==6) Disp.setFont(Font6); 
    else if(x==7) Disp.setFont(Font7); 
  }
