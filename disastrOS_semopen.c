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

  // se non esiste lo creo
  if(!sem){

    sem = Semaphore_alloc(semnum, 0);
    // se non può essere creato torna errore
    if(!sem){
      running -> syscall_retvalue = DSOS_ESEMAPHORECREATE;
      return;
    }

    // inserisco il semaforo appena creato nella semaphores_list
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);
  }

  // controllo se è già presente il descrittore del semaforo nel processo, quindi se il semaforo è già aperto
  SemDescriptor* open_descriptor = Search_id(&(running->sem_descriptors), semnum);
  
  // se è aperto torno il suo descrittore
  if(open_descriptor){
    running -> syscall_retvalue = open_descriptor -> fd;
    return;
  }
  else{
    // creo il descrittore per il semaforo in questo processo e lo aggiungo alla lista dei descrittori
    SemDescriptor* sem_des = SemDescriptor_alloc(running -> last_sem_fd, sem, running);
    if(!sem_des){
      running -> syscall_retvalue = DSOS_ESEMAPHORENOFD;
      return;
    }

    // incremento sem_fd per la prossima chiamata
    running -> last_sem_fd++;
  }


  // creo il puntatore al nuovo descrittore 
  SemDescriptorPtr* sem_des_ptr = SemDescriptorPtr_alloc(sem_des);
  if(!sem_des_ptr){
    running -> syscall_retvalue = DSOS_ESEMAPHORENOFDPTR;
    return;
  }

  List_insert(&running -> sem_descriptors, running -> sem_descriptors.last, (ListItem*) sem_des);
  
  // aggiungo al semaforo, nella lista dei descrittori, un puntatore al nuovo descrittore creato
  sem_des -> ptr = sem_des_ptr;
  List_insert(&(sem -> descriptors), sem -> descriptors.last, (ListItem*) sem_des_ptr);

  // ritorna il fd del nuovo descrittore
  running -> syscall_retvalue = sem_des -> fd;

}
