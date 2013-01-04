#define control_pin 10


PID pid(&temperature, &power, &config.target_temp, 0.0, 0.0, 0.0, DIRECT);
PID_ATune auto_tune(&temperature, &power);
double last_power;

void control_setup(){
}

void control_loop() {
  pinMode( control_pin, OUTPUT );
  
  if(config.tuning && auto_tune.Runtime()) {
    finish_autotune();
  } else if(!config.tuning) {
    pid.SetMode(!config.paused);
    pid.SetTunings(profiles[config.driving].kp, profiles[config.driving].ki, profiles[config.driving].kd);
    pid.SetSampleTime(profiles[config.driving].sample_time);  // Update the control value once per second
    pid.Compute();
    
    if(last_power != power) {
      last_power = power;
      analogWrite(control_pin, power);
      Serial.print("Log\tpower\t");
      Serial.println((100.0 * ((float)power / 255.0)));
    }
  }
}

void finish_autotune() {
  profiles[config.driving].kp = auto_tune.GetKp();
  profiles[config.driving].ki = auto_tune.GetKi();
  profiles[config.driving].kd = auto_tune.GetKd();
  
  save();
  config.tuning = false;
}

void start_autotune() {
  auto_tune.SetNoiseBand(config.noise_band);
  auto_tune.SetOutputStep(50);
  auto_tune.SetLookbackSec((int)(config.lookback_min * 60));
  
  config.tuning = true;
}
