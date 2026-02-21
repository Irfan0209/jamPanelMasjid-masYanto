

char * TGLMASEHI()
  {
    static char  out[12];
    sprintf(out,"%02d-%02d-%04d",now.Day(),now.Month(),now.Year());
    return out;     
  }

char * DAY()
  {
    static char  out[15];
    sprintf(out,"%s %s",Hari[now.DayOfWeek()],pasar[jumlahhari() % 5]);
    return out;     
  }

char * showTanggal(){
  static char buff_date[60];

   RtcDateTime now = Rtc.GetDateTime();

   const char *pasar[] = {"WAGE","KLIWON","LEGI","PAHING","PON"};
    const char *Hari[]  = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
    const char *namaBulanHijriah[] = {
      "MUHARRAM","SHAFAR","RABIUL AWAL","RABIUL AKHIR",
      "JUMADIL AWAL","JUMADIL AKHIR","RAJAB",
      "SYA'BAN","RAMADHAN","SYAWAL",
      "DZULQA'DAH","DZULHIJAH"
    };
    
  sprintf(
      buff_date,
      "%s %s %02d-%02d-%04d %02d %s %02dH",
      Hari[now.DayOfWeek()],
      pasar[jumlahhari() % 5],
      now.Day(), now.Month(), now.Year(),
      Hijir.getHijriyahDate,
      namaBulanHijriah[Hijir.getHijriyahMonth - 1],
      Hijir.getHijriyahYear
    );

    return buff_date;
}

void tampilkanVolume() {
  char buff[15];
  snprintf(buff, sizeof(buff), "%s=%02d", "VOLUME", volume);
  fType(1); 
  dwCtr(0,4,buff); //tulisan nama
  DoSwap  = true ;
}
