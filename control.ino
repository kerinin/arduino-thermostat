#define controlPin 10

PID myPID(&temperature, &power, &config.targetTemp, 0.0, 0.0, 0.0, DIRECT);
PID_ATune aTune(&temperature, &power);
double lastPower;

void control_setup(){
}

void control_loop() {
  pinMode( controlPin, OUTPUT );
  
  if(config.tuning && aTune.Runtime()) {
    finishAutotune();
  } else if(!config.tuning) {
    myPID.SetMode(!config.paused);
    myPID.SetTunings(profiles[config.driving].kp, profiles[config.driving].ki, profiles[config.driving].kd);
    myPID.SetSampleTime(profiles[config.driving].sampleTime);  // Update the control value once per second
    myPID.Compute();
    
    if(lastPower != power) {
      lastPower = power;
      analogWrite(controlPin, power);
      Serial.print("Log\tpower\t");
      Serial.println(power);
    }
  }
}

void finishAutotune() {
  profiles[config.driving].kp = aTune.GetKp();
  profiles[config.driving].ki = aTune.GetKi();
  profiles[config.driving].kd = aTune.GetKd();
  
  save();
  config.tuning = false;
}

void startAutotune() {
  aTune.SetNoiseBand(config.noiseBand);
  aTune.SetOutputStep(50);
  aTune.SetLookbackSec((int)(config.lookbackMin * 60));
  
  config.tuning = true;
}
