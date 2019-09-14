#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
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

  // se il valore del semaforo è <= 0 il processo deve aspettare nella lista waiting_descriptors
  if(sem->count <= 0){

    (sem->count)--;
    List_insert(&sem -> waiting_descriptors, sem -> waiting_descriptors.last, (ListItem*) sem_des -> ptr);

    // il processo passa nello stato di waiting
    running -> status = Waiting;

    // lo inserisco nella waiting list del sistema
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

    // mando in running il primo processo nella coda dei ready (se c'è)
    if (ready_list.first)
      running = (PCB*) List_detach(&ready_list, ready_list.first);
    else {
      running=0;
      printf ("they are all sleeping\n");
      running -> syscall_retvalue = 0;
      //disastrOS_printStatus();
    }
  }

  else{
    (sem->count)--;
    running -> syscall_retvalue = 0;
  }
}
