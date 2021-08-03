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
#include "display_manager.h"
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
  log_debug("fork [%i] ended with code %i\n", pid, status);
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
      {"debug",   no_argument,       0, 'd'},
      {"trace",   no_argument,       0, 't'},
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
  log_set_level(LOG_INFO);
  while((opt = getopt_long(argc, argv, ":lhvdtqb:", long_options, &option_index)) != -1) {
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
        log_set_level(LOG_ERROR);
        break;
      case 'd':
        log_set_level(LOG_DEBUG);
        break;
      case 't':
        log_set_level(LOG_TRACE);
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
  printf("  --debug                 debug mode\n");
  printf("  --trace                 trace mode\n");
}

/* main routine
 */
uint32_t start_wpaf(char ** argv, uint32_t boardid) {
  const char * fan_manager_process_name = "wpaf: fan manager";
  const char * display_manager_process_name = "wpaf: display manager";
  board_interface * board = boards[boardid];
  board_handler * handler = board->init();

  pid_t fan_pid;
  pid_t display_pid;

  fan_pid = fork();
  if (fan_pid == -1) {
    log_error("can't start fan manager");
    return EXIT_FAILURE;
  }else if (fan_pid == 0) {
    if (prctl(PR_SET_NAME, (unsigned long) fan_manager_process_name) < 0)
      log_error("%s", strerror(errno));
    start_fan_manager(handler->fan, handler->f_handler);
  }

  display_pid = fork();
  if (display_pid == -1) {
    log_error("can't start display manager");
    return EXIT_FAILURE;
  }else if (display_pid == 0) {
    if (prctl(PR_SET_NAME, (unsigned long) display_manager_process_name) < 0)
      log_error("%s", strerror(errno));
    start_display_manager(handler->display, handler->o_handler);
  }

  board->close(handler);

  wait_for_all();
  return EXIT_SUCCESS;
}

