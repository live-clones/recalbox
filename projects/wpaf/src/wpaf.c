#include <errno.h>
#include <getopt.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"

#include "boards.h"
#include "board_driver.h"
#include "cpu_controller.h"
#include "fan_driver.h"
#include "fan_manager.h"
#include "gui.h"
#include "log.h"
#include "waveshare_poehatb.h"

sem_t *semaphore[10];

uint32_t start_wpaf(char ** argv, uint32_t boardid) ;
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

  for(int i=0; i<10; i++) {
    semaphore[i] = mmap(NULL, sizeof(sem_t), PROT_READ |PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (sem_init(semaphore[i], 1, 1)) {
      printf("can't init semaphore: %s", strerror(errno));
    }
  }

  parse_args(argc, argv);

  boardid = get_boardid_from_name(arguments.board);

  return start_wpaf(argv, boardid);
}

void parse_args(int argc, char **argv) {
  int opt;
  int option_index;
  static struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose", no_argument,       0, 'v'},
      {"quiet",   no_argument,       0, 'q'},
      /* These options don’t set a flag.
         We distinguish them by their indices. */
      {"help",    no_argument,       0, 'h'},
      {"list",    no_argument,       0, 'l'},
      {"board",   required_argument, 0, 'b'},
      {0, 0, 0, 0}
    };
  // put ':' in the starting of the
  // string so that program can
  //distinguish between '?' and ':'
  while((opt = getopt_long(argc, argv, ":lhvqb:", long_options, &option_index)) != -1) {
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
      case 'v':
        log_set_level(LOG_DEBUG);
        break;
      case 'q':
        log_set_quiet(true);
        break;
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
  int32_t i=0;
  while(boards[i]) {
    if (strcmp(board_id[i], board_name) == 0)
      return i;
    i++;
  }
  return -1;
}

void list_known_boards() {
  uint32_t i = 0;
  printf("List of known boards and ids\n");
  printf("----------------------------\n");
  while(boards[i]) {
    printf("%s (%s)\n", board_list[i], board_id[i]);
    i++;
  }
}

void show_help() {
  printf("wpaf [-b BOARD_ID | -h | -l]\n\n");
  printf("  -l,--list               list known boards\n");
  printf("  -h,--help               show help\n\n");
  printf("  -b,--board=board_id     start wpaf for board_id\n");
  printf("                          board_id can be obtained from list of known boards (-l)\n\n");
  printf("  --verbose               verbose mode\n");
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
  snprintf(recalbox_string, sizeof(recalbox_string), "Recalbox %s", recalbox_version);

  gui_init(handler->display, handler->o_handler, handler->o_handler->columns, handler->o_handler->pages);

  while(true) {
    float cputemp = get_cpu_temp();
    snprintf(string_buffer, sizeof(string_buffer), "CPU temp: %.2f", cputemp);
    gui_clear(handler->display, handler->o_handler);
    gui_string(handler->o_handler, 0, 0, string_buffer, &Font12, BLACK, WHITE);
    gui_string(handler->o_handler, 0, 16, recalbox_string, &Font12, BLACK, WHITE);
    gui_draw(handler->display, handler->o_handler);
    sleep(1);
  }
  board->close(handler);

  wait_for_all();
  return EXIT_SUCCESS;
}

