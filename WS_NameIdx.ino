

char * TGLMASEHI()
  {
    RtcDateTime now = Rtc.GetDateTime();
    static char  out[12];
    snprintf(out,sizeof(out),"%02d-%02d-%04d",now.Day(),now.Month(),now.Year());
    return out;     
  }

char * NASIONAL()
  {
    static char  out[8];
    RtcDateTime now = Rtc.GetDateTime();
    snprintf(out,sizeof(out),"%s",Hari[now.DayOfWeek()]);
    return out;     
  }

char * PASARAN()
  {
    static char  out[8];
    snprintf(out,sizeof(out),"%s",pasar[jumlahhari() % 5]);
    return out;     
  }

char * showTanggal(){
  static char buff_date[60];

   RtcDateTime now = Rtc.GetDateTime();
    
  snprintf(
      buff_date,sizeof(buff_date),
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

//void tampilkanVolume() {
//  char buff[15];
//  snprintf(buff, sizeof(buff), "%s=%02d", "VOLUME", volume);
//  fType(1); 
//  dwCtr(0,4,buff); //tulisan nama
//  DoSwap  = true ;
//}
