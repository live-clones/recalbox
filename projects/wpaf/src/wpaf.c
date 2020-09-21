#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"

#include "boards.h"
#include "board_driver.h"
#include "cpu_controller.h"
#include "fan_driver.h"
#include "fan_manager.h"
#include "gui.h"
#include "oled_driver.h"
#include "waveshare_poehatb.h"

uint32_t start_wpaf(uint32_t boardid) ;
int get_boardid_from_name(char * board_name) ;
void parse_args(int argc, char **argv) ;
void list_known_boards() ;
void show_help();

struct args {
  char * board;
} arguments;

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
  int boardid;

  parse_args(argc, argv);

  boardid = get_boardid_from_name(arguments.board);

  return start_wpaf(boardid);
}

void parse_args(int argc, char **argv) {
  int opt;

  // put ':' in the starting of the
  // string so that program can
  //distinguish between '?' and ':'
  while((opt = getopt(argc, argv, ":lhb:")) != -1) {
    switch(opt) {
      case 'h':
        show_help();
        exit(0);
        break;
      case 'b':
        arguments.board = optarg;
        break;
      case 'l':
        list_known_boards();
        exit(0);
      case ':':
        printf("option needs a value\n");
        break;
      case '?':
         printf("unknown option: %c\n", optopt);
         break;
    }
  }
}

int get_boardid_from_name(char * board_name) {
  return 0;
}

void list_known_boards() {
  printf("List of known boards and ids\n");
  printf("----------------------------\n");
  for(int i = 0; i<NUMBER_OF_BOARDS; i++) {
    printf("%s (%s)\n", board_list[i], board_id[i]);
  }
}

void show_help() {
  printf("wpaf [-b BOARD_ID | -h | -l]\n\n");
  printf("  -l              list known boards\n");
  printf("  -h              show help\n\n");
  printf("  -b board_id     start wpaf for board_id\n\n");
  printf("board_id can be obtained from list of known boards (-l)\n");
}

/* main routine
 */
uint32_t start_wpaf(uint32_t boardid) {
  board_interface * board = boards[boardid];
  board_handler * handler = board->init();

  char string_buffer[256];
  char recalbox_version[50];
  char recalbox_string[256];
  pid_t pid = fork();

  if (pid == -1) {
    perror("can't start fan manager\n");
    return EXIT_FAILURE;
  }else if (pid == 0) {
    start_fan_manager(handler->fan, handler->f_handler);
  }

  /*
   * get recalbox version from fs
   */
  FILE * fd = fopen("/recalbox/recalbox.version", "r");
  if (fd)
    fgets(recalbox_version, sizeof(recalbox_version), fd);
  fclose(fd);
  snprintf(recalbox_string, sizeof(recalbox_string), "RecalboxOS %s", recalbox_version);

  gui_init(handler->oled, handler->o_handler, handler->o_handler->columns, handler->o_handler->pages);

  while(true) {
    float cputemp = get_cpu_temp();
    snprintf(string_buffer, sizeof(string_buffer), "CPU temp: %.2f", cputemp);
    gui_clear(handler->oled, handler->o_handler);
    gui_string(handler->o_handler, 0, 0, string_buffer, &Font12, BLACK, WHITE);
    gui_string(handler->o_handler, 0, 16, recalbox_string, &Font12, BLACK, WHITE);
    gui_draw(handler->oled, handler->o_handler);
    sleep(1);
  }
  board->close(handler);

  wait_for_all();
  return EXIT_SUCCESS;
}

