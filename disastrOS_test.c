#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  
  
  // apro i semafori
  printf("Il processo %d sta aprendo il semaforo sem\n", disastrOS_getpid());
  int sem = disastrOS_semOpen(0);
  printf("Il processo %d sta aprendo il semaforo sem2\n", disastrOS_getpid());
  int sem2 = disastrOS_semOpen(3);

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);


    disastrOS_semWait(sem);
    disastrOS_semWait(sem2);
    
    // sezione critica
    printf("Il processo %d sta entrando in sezione critica\n", disastrOS_getpid());
    disastrOS_sleep(disastrOS_getpid()*2);
    printf("Il processo %d sta uscendo dalla sezione critica\n", disastrOS_getpid());

    disastrOS_semPost(sem2);
    disastrOS_semPost(sem);

  }

  // chiudo i semafori
  printf("Il processo %d sta chiudendo il semaforo sem\n", disastrOS_getpid());
  disastrOS_semClose(sem);
  printf("Il processo %d sta chiudendo il semaforo sem2\n", disastrOS_getpid());
  disastrOS_semClose(sem2);


  printf("PID: %d, terminating\n", disastrOS_getpid());
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
