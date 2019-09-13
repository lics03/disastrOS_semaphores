#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  // do stuff :)

  int sem_fd = running -> syscall_args[0];

  // controllo se il descrittore del semaforo è nei semafori attivi del processo corrente
  SemDescriptor* sem_des = SemDescriptorList_byFd(&(running -> sem_descriptors), sem_fd);

  // se il sem_fd non è nel processo ritorna un errore
  if (!sem_des) {
      running -> syscall_retvalue = DSOS_ESEMAPHORENOTPRESENT;
      return;
  }

  // se c'è prendo il semaforo
  Semaphore* sem = sem_des -> semaphore;

  if (!sem) {
    running -> syscall_retvalue = DSOS_ESEMAPHOREOPEN;
    return;
  }

  if(sem->count < 0){







  }

  // incremento il valore
  (sem->count)++;

  // ritorna 0 in caso di successo
  running -> syscall_retvalue = 0;
  
}
