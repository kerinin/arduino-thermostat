#include "control.h"

double last_power;
double current_target_temp;
// RM NOTE: This presents a challenge for profile-specific temps...
PID pid(&temperature, &power, &current_target_temp, 0.0, 0.0, 0.0, DIRECT);
PID_ATune auto_tune(&temperature, &power);

void control_setup() {
}

void control_loop() {
  pinMode( control_pin, OUTPUT );
  
  if(tuning && auto_tune.Runtime()) {
    finish_autotune();
  } else if(!tuning) {
    pid.SetMode(!config.paused);
    pid.SetTunings(profiles[config.driving].kp, profiles[config.driving].ki, profiles[config.driving].kd);
    pid.SetSampleTime(1000 * profiles[config.driving].sample_time);  // Update the control value once per second
    current_target_temp = profiles[config.driving].target_temp;
    pid.Compute();
  }
  
  if(last_power != power) {
    last_power = power;
    analogWrite(control_pin, power);
    Serial.print("Log\tpower\t");
    Serial.println((100.0 * ((float)power / 255.0)));
  }
}

void finish_autotune() {
  Serial.println("Finished autotune");
  Serial.print("Kp: ");
  Serial.println(auto_tune.GetKp());
  Serial.print("Ki: ");
  Serial.println(auto_tune.GetKi());
  Serial.print("Kd: ");
  Serial.println(auto_tune.GetKd());
  
  profiles[config.driving].kp = auto_tune.GetKp();
  profiles[config.driving].ki = auto_tune.GetKi();
  profiles[config.driving].kd = auto_tune.GetKd();
  
  save();
  tuning = false;
}

void start_autotune() {
  power = 127;
  auto_tune.SetControlType(1);
  auto_tune.SetNoiseBand(config.noise_band);
  auto_tune.SetOutputStep(127);
  auto_tune.SetLookbackSec((int)(config.lookback_min * 60));
  
  Serial.println("Starting Autotune");
  tuning = true;
}
