#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "config.h"

#include "cpu_controller.h"
#include "fonts.h"
#include "fan_manager.h"
#include "oled_driver.h"
#include "ssd1306_oled_controller.h"
#include "gui.h"

// wait for all children to exit
void wait_for_all() {
	int status;
	pid_t pid;
	pid = wait(&status);
  printf("Fork [%i] ended with code %i\n", pid, status);
}

/*
 * Main
 **/
int main(int argc, char ** argv) {
  oled_interface * oled;
  oled_handler * oled_handler;
  char string_buffer[256];
  pid_t pid = fork();

  if (pid == -1) {
    perror("can't start fan manager\n");
    return EXIT_FAILURE;
  }else if (pid == 0) {
    start_fan_manager();
  }

  oled = &ssd1306_interface;
  oled_handler = oled->init(0x3c);

  gui_init(oled, oled_handler, 128, 32);

  while(true) {
    float cputemp = get_cpu_temp();
    snprintf(string_buffer, sizeof(string_buffer), "CPU temp: %.2f", cputemp);
    gui_clear(oled, oled_handler);
    gui_string(oled_handler, 0, 0, string_buffer, &Font12, BLACK, WHITE);
    gui_draw(oled, oled_handler);
    sleep(1);
  }
  oled->close(oled_handler);

  //OLED_Init();
  //GUI_Init(OLED_WIDTH, OLED_HEIGHT);
  //GUI_Clear();
  //GUI_DisString_EN(0 , 0, "eth :", &Font12, FONT_BACKGROUND, WHITE);
  //GUI_Display();

  wait_for_all();
  return EXIT_SUCCESS;
}

