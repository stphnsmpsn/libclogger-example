#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include "handler.h"
#include "stphnsmpsn/clogger/Clogger.h"

static void print_trace();
static int decode_symbol(char const * const program_name, char* addr);

void generic_signal_handler(int sig) {
  print_trace();
  exit(1);
}

static void print_trace() {
  void *array[100];
  size_t size;
  char** strings;

  size = backtrace(array, 100);
  strings = backtrace_symbols(array, size);

  for(int i = 3; i < size - 2; i++){
    decode_symbol("./build/clogger_demo", strings[i]);
  }
}

static int decode_symbol(char const * const program_name, char* symbol)
{
  char command[512] = {0};
  char path[512] = {0};
  FILE *fp = NULL;
  char* offset; 

  strtok(symbol, "(");
  offset = strtok(NULL, ")");

  sprintf(command,"addr2line -f -p -e %.256s %s", program_name, offset); 

  fp = popen(command, "r");
  if (fp == NULL) {
    Clogger()->log(CLOGLVL_CRITICAL, "Failed to run command");
    exit(1);
  }

  while (fgets(path, sizeof(path), fp) != NULL) {
    Clogger()->log(CLOGLVL_CRITICAL, strtok(path, "\r\n"));
  }

  pclose(fp);

  return 0;
}