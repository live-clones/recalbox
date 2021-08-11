#include <stdio.h>

// return cpu temp (thermal zone 0)
float get_cpu_temp() {
  float systemp, millideg;
  FILE *thermal;
  
  thermal = fopen("/sys/class/thermal/thermal_zone0/temp","r");
  if (!thermal) 
    return -1;

  fscanf(thermal, "%f", &millideg);
  fclose(thermal);
  systemp = millideg / 1000;
  return systemp;
}
