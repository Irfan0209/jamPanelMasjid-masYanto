
void showDays() // Box Sholah Time   tampilan jadwal sholat
  {
    RtcDateTime now = Rtc.GetDateTime();
    uint8_t daynow   = now.DayOfWeek();    // load day Number
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();
    static bool state = false;
    char  Buff_hariN[10];
    char  Buff_hariJ[10];
    char Buff_Jam[3];
    char Buff_Men[3];

    sprintf(Buff_hariN,"%s   ",Hari[daynow]); 
    sprintf(Buff_hariJ,"%s   ",pasar[jumlahhari()%5]); 
    sprintf(Buff_Jam,"%02d",now.Hour()); 
    sprintf(Buff_Men,"%02d",now.Minute());

    if((Tmr-lsRn)>1000){state = !state; lsRn=Tmr; }
    
    fType(7);
    dwCtr(0,0,Buff_hariN); //tulisan hari biasa
    dwCtr(0,9,Buff_hariJ);   //tulisan hari jawa 
    
    fType(1); 
    dwCtr(42,0,Buff_Jam);
    dwCtr(42,9,Buff_Men);
    state ? dwCtr(59,4,":") : dwCtr(59,4," ");
    
    DoSwap = true;          
  }
  
void showHijriah(){

    RtcDateTime now = Rtc.GetDateTime();
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();
    static bool state = false;
    char Jam[8];
    char tgl[15];
    
    if((Tmr-lsRn)>1000){state = !state; lsRn=Tmr; }
    state?sprintf(Jam,"%02d %02d",now.Hour(),now.Minute()):sprintf(Jam,"%02d:%02d",now.Hour(),now.Minute()); 
    
    sprintf(tgl,"%02d-%02d-%04d",now.Day(),now.Month(),now.Year());
    
    Disp.drawRect(1,11,63,12);
    
    fType(1); 
    dwCtr(0,9,Jam); //tulisan nama
    dwCtr(0,0,tgl);   //tulisan tangal
    
    DoSwap = true;
}

char * TGLJAWA()
  {
    static char  out[45];
    sprintf(out,"%02d-%s-%dH\0",Hijir.getHijriyahDate,namaBulanHijriah[Hijir.getHijriyahMonth - 1],Hijir.getHijriyahYear);
    return out;     
  }


void tampilkanVolume() {
  char buff[15];
  snprintf(buff, sizeof(buff), "%s=%02d", "VOLUME", volume);
  fType(1); 
  dwCtr(0,4,buff); //tulisan nama
  DoSwap  = true ;
}
