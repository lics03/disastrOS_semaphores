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
    // rimuovo il primo processo nella waiting queue del semaforo
    SemDescriptorPtr* proc = (SemDescriptorPtr*) List_detach(&(sem -> waiting_descriptors), (ListItem*) sem -> waiting_descriptors.first);

    // prendo il PCB di quel processo
    PCB* proc_pcb = proc -> descriptor -> pcb;

    // rimuovo il processo dalla waiting list del sistema
    List_detach(&waiting_list, (ListItem*) proc_pcb);

    // cambio lo stato del processo a Ready
    proc_pcb -> status = Ready;

    // inserisco il processo nella ready list del sistema
    List_insert(&ready_list, ready_list.last, (ListItem*) proc_pcb);

  }

  // incremento il valore del semaforo
  (sem->count)++;

  // ritorna 0 in caso di successo
  running -> syscall_retvalue = 0;
  
}
