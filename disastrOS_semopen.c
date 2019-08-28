#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  // do stuff :)

  int semnum = running -> syscall_args[0];

  Semaphore* s = Semaphore_alloc(semnum, 0);
  if(!s){
    running -> syscall_retvalue = -1;
    return;
  }

  


}
