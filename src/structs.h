#ifndef STRUCTS_H
#define STRUCTS_H

struct configuration {
  bool paused;     // PID doesn't update control variable if TRUE
  unsigned short int driving;        // The hardware being controlled
  float noise_band;    // Autotune noise band
  float lookback_min;  // Autotune lookback
};

struct profile {
  char name[16];
  double kp, ki, kd;
  double target_temp;  // The temperature we want to hit (user-specified)
  int sample_time;
};

#endif
