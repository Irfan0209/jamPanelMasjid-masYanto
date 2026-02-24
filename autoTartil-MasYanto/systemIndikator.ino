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

void cekStatusSystem(){
    bool status = (millis() - lastTimeReceived <= TIMEOUT_INTERVAL);
    if (status != lastNormalStatus) {
      digitalWrite(NORMAL_STATUS_LED, status);
      lastNormalStatus = status;
    }
}
//========================== END ==========================//


