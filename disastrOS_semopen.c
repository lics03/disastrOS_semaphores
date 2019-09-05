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

  // controllo se il semaforo è già presente nel sistema
  Semaphore* sem = SemaphoreList_byId(&semaphores_list, semnum);

  //se non esiste lo creo
  if(sem){

    sem = Semaphore_alloc(semnum, 0);
    if(!sem){
      running -> syscall_retvalue = -1;
      return;
    }

    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);
  }

  if (!sem) {
     running->syscall_retvalue = -1;
     return;
  }


  //creo il descrittore per il semaforo in questo processo e lo aggiungo alla lista dei descrittori
  SemDescriptor* sem_des = SemDescriptor_alloc(running -> last_sem_fd, sem, running);
  if(!sem_des){
    running -> syscall_retvalue = -1;
    return;
  }

  //incremento sem_fd per la prossima chiamata
  running -> last_sem_fd++;

  SemDescriptorPtr * sem_des_ptr = SemDescriptorPtr_alloc(sem_des);

  
  List_insert(&running -> sem_descriptors, running -> sem_descriptors.last, (ListItem*) sem_des);
  
  //6 add to the resource, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  sem_des -> ptr = sem_des_ptr;
  List_insert(&sem -> descriptors, sem -> descriptors.last, (ListItem*) sem_des_ptr);

  // return the FD of the new descriptor to the process
  running -> syscall_retvalue = sem_des -> fd;

}
