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
#include "fan_manager.h"
#include "gui.h"
#include "log.h"
#include "waveshare_poehatb.h"
#include "wpaf.h"

sem_t *semaphore[10];
char * process_name;

uint32_t start_wpaf(char ** argv) ;
int get_boardid_from_name(char * board_name) ;
void parse_args(int argc, char **argv) ;
void list_known_boards() ;
void show_help();

__ARGS arguments;

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

  for(int i=0; i<10; i++) {
    semaphore[i] = mmap(NULL, sizeof(sem_t), PROT_READ |PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (sem_init(semaphore[i], 1, 1)) {
      printf("can't init semaphore: %s", strerror(errno));
    }
  }

  parse_args(argc, argv);

  return start_wpaf(argv);
}

void parse_args(int argc, char **argv) {
  int opt;
  int option_index;

  // sanitize arguments
  arguments.board = NULL;
  arguments.temp_setpoint = 0;

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
      {"temp-setpoint", required_argument, 0, 'T'},
      {0, 0, 0, 0}
    };
  // put ':' in the starting of the
  // string so that program can
  //distinguish between '?' and ':'
  log_set_level(LOG_INFO);
  while((opt = getopt_long(argc, argv, ":lhvdtqb:T:", long_options, &option_index)) != -1) {
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
      case 'T':
        arguments.temp_setpoint = atoi(optarg);
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
  printf("                          board_id can be obtained from list of known boards (-l)\n");
  printf("  -T,--temp-setpoint=int  override default board temperature setpoint\n");
  printf("                          temp setpoint is the target CPU temp to reach for fan to stop (or lower speed)\n");
  printf("\n  --verbose               verbose mode\n");
  printf("  --debug                 debug mode\n");
  printf("  --trace                 trace mode\n");
}

/* main routine
 */
uint32_t start_wpaf(char ** argv) {
  int boardid = get_boardid_from_name(arguments.board);
  board_interface * board = boards[boardid];
  board_handler * handler = board->init();

  pid_t fan_pid;
  pid_t display_pid;

  process_name = argv[0];

  fan_pid = fork();
  if (fan_pid == -1) {
    log_error("can't start fan manager");
    return EXIT_FAILURE;
  }else if (fan_pid == 0) {
//    strncpy(process_name, "wpaf: fan manager", 0xff);
    log_debug("fan manager started with pid %d", getpid());
    start_fan_manager(board, handler, &arguments);
  }

  display_pid = fork();
  if (display_pid == -1) {
    log_error("can't start display manager");
    return EXIT_FAILURE;
  }else if (display_pid == 0) {
 //   strncpy(process_name, "wpaf: display manager", 0xff);
    log_debug("display manager started with pid %d", getpid());
    start_display_manager(board, handler, &arguments);
  }

  wait_for_all();
  board->close(handler);

  return EXIT_SUCCESS;
}

