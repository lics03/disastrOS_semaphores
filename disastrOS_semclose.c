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
      running -> syscall_retvalue = DSOS_ESEMAPHORECLOSE;
      return;
  }

  // rimuovo il descrittore dalla lista dei semafori
  sem_des = (SemDescriptor*) List_detach(&(running -> sem_descriptors), (ListItem*) sem_des);
  assert(sem_des);

  Semaphore* sem = sem_des -> semaphore;

  // rimuovo il sem descriptor pointer dalla lista dei semafori
  SemDescriptorPtr* sem_des_ptr = (SemDescriptorPtr*) List_detach(&(sem -> descriptors), (ListItem*)(sem_des -> ptr));
  assert(sem_des_ptr);


  // rimuovo il sem descriptor waiting pointer dalla lista waiting_descriptors
  SemDescriptorPtr* sem_des_ptr_waiting_list = (SemDescriptorPtr*) List_detach(&(sem -> waiting_descriptors), (ListItem*)(sem_des -> ptr_waiting_list));
  assert(sem_des_ptr_waiting_list);

  // libero la memoria allocata
  SemDescriptor_free(sem_des);
  SemDescriptorPtr_free(sem_des_ptr);
  SemDescriptorPtr_free(sem_des_ptr_waiting_list);
  

  // controllo se il semaforo è stato chiuso da tutti i processi che lo hanno aperto 
  if(sem -> descriptors.size == 0){

    sem = (Semaphore*) List_detach(&semaphores_list, (ListItem*) sem);
    // sem non deve essere nullo, in quanto il semaforo deve essere nella semaphores_list
    assert(sem);
    // libero la memoria
    Semaphore_free(sem);
  }

  // ritorna 0 in caso di successo
  running -> syscall_retvalue = 0;
}
