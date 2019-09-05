#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  // do stuff :)

  int sem_fd = running -> syscall_args[0];

  // controllo se il descrittore del semaforo è nei semafori attivi del processo corrente
  SemDescriptor* sem_des = SemDescriptorList_byFd(&(running -> sem_descriptors), sem_fd);

  // se il sem_fd non è nel processo ritorna un errore
  if (!sem_des) {
      running -> syscall_retvalue = -1;
      return;
  }

  // rimuovo il descrittore dalla lista dei semafori
  sem_des = (SemDescriptor*) List_detach(&(running -> sem_descriptors), (ListItem*) sem_des);
  assert(sem_des);

  Semaphore* sem = sem_des -> semaphore;

  // rimuovo il sem descriptor pointer dalla lista dei semafori
  SemDescriptorPtr* sem_des_ptr = (SemDescriptorPtr*) List_detach(&(sem -> descriptors), (ListItem*)(sem_des -> ptr));
  assert(sem_des_ptr);
  
  SemDescriptor_free(sem_des);
  SemDescriptorPtr_free(sem_des_ptr);

  // ritorna 0 in caso di successo
  running -> syscall_retvalue = 0;
}
