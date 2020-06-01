#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "stphnsmpsn/clogger/Clogger.h"
#include "handler.h"

#define DEBUG

void foo();
void bar();
void bazz();
void cause_segfault(); 

int main(const int argc, const char const **argv)
{
  // We haven't yet set a logger - that's ok; null implementation protects us from crashes
  Clogger()->log(CLOGLVL_INFO, "Program Started");
  
  // Now we will set our logger
  SetClogger(GetPrintfClogger);
  Clogger()->log(CLOGLVL_INFO, "Program Started");
  
  // Log level defaults to 'INFO'. Any logs more fine than this will be ignored
  Clogger()->log(CLOGLVL_FINE, "This should be ignored");

  // Let's set the logging level. In debug, we want all logs. In production, only warning/critical should be logged
  #ifdef DEBUG
    Clogger()->setLevel(CLOGLVL_FINEST);
  #else
    Clogger()->setLevel(CLOGLVL_WARNING);
  #endif
  Clogger()->log(CLOGLVL_FINE, "If DEBUG is defined, this should print");
  Clogger()->log(CLOGLVL_WARNING, "This should print no matter what");

  // Now, let's set an error handler that uses the logger
  signal(SIGSEGV, generic_signal_handler);   
  
  // And now, we purposely cause a segmentation fault and ensure the proper handler gets invoked and a backtrace is logged
  foo();
}


void cause_segfault() {
  int *foo = (int*)-1;    // make a bad pointer
  printf("%d\n", *foo);   // causes segfault
}

void foo() {
  bar();
}

void bar() {
  bazz();
}

void bazz() {
  cause_segfault();
}

