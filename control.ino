/*

PID myPID(&temperature, &power, &targetTemp, 0.0, 0.0, 0.0, DIRECT);
PID_ATune aTune(&temperature, &power);

void control_setup(){
  //EEPROMex.readBlock(0, profiles, 4);
}

void control_loop() {
  if(tuning && aTune.Runtime()) {
    finishAutotune();
  } else if(!tuning) {
    myPID.SetMode(!paused);
    myPID.SetTunings(currentProfile()->kp, currentProfile()->ki, currentProfile()->kd);
    myPID.SetSampleTime(currentProfile()->sampleTime);  // Update the control value once per second
    myPID.Compute();
  }
}
void finishAutotune() {
  currentProfile()->kp = aTune.GetKp();
  currentProfile()->ki = aTune.GetKi();
  currentProfile()->kd = aTune.GetKd();
  //EEPROMex.updateBlock(0, profiles, 4);
  tuning = false;
}

void startAutotune() {
  tuning = true;
  aTune.SetNoiseBand(noiseBand);
  aTune.SetOutputStep(50);
  aTune.SetLookbackSec((int)(lookbackMin * 60));
}

struct profile* currentProfile() {
  switch (driving) {
  case drivingFermenter:
    return profiles[0];
  case drivingSousVide:
    return profiles[1];
  case drivingSmoker:
    return profiles[2];
  default:
    return profiles[3];
  }
}
*/
