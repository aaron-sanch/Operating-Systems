/* 
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University
			
	    A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define NULL 0
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
#include "console.H"

struct Node { // Creating a node class to hold the threads on the queue

   Node* next;
   Thread* curr_thread;
   
};

class Queue { // Creating the queue
   Node* head;
   Node* tail;
   long q_size;

public:
   Queue() {
      this->head = NULL;
      this->tail = NULL;
      this->head->next = this->tail;
      this->tail->next = NULL;

      this->q_size = 0;
   }

   long size() {
      return q_size;
   }

   void push(Thread *_thread) {
      if (this->q_size == 0) {
         this->head = new Node;
         this->head->curr_thread = _thread;
         this->head->next = NULL;
         this->tail = this->head;
         q_size += 1;
         return;
      }
      Node* curr_node = new Node;
      curr_node->curr_thread = _thread;
      curr_node->next = NULL;
      this->tail->next = curr_node;
      this->tail = curr_node;
      q_size += 1;
   }

   Thread* pop() {
      if (q_size <= 0) {
         Console::puts("Trying to pop from empty queue.\n");
         return NULL;
      }
      Thread* temp = this->head->curr_thread;
      this->head = this->head->next;
      q_size -= 1;

      return temp;
   }

   void destroy_thread(Thread *_thread) {
      Node *finder = this->head;
      while (finder != NULL) {
         if (finder->curr_thread == _thread) {
            Thread* to_destroy = finder->curr_thread;
            finder->curr_thread = finder->next->curr_thread;
            finder->next = finder->next->next;
	    Console::puts("Gets to destroy in destroy.\n");
            delete to_destroy;
	    Console::puts("Destroys in destroy thread.\n");
            q_size -= 1;
            return;
         }
         finder = finder->next;
      }
   }
};

/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'. 

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is 
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ). 
    (For details on how to set up a timer and how to handle timer interrupts 
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the 
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.
 
    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement 
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes, 
     such as 'FIFOScheduler'.)
    
 */

/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

class Scheduler {

  /* The scheduler may need private members... */
  // Creating a queue to use here
  Queue queue;

public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */
  
   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may 
      just add the thread to the ready queue, using 'resume'. */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. 
      Graciously handle the case where the thread wants to terminate itself.*/
  
};
	
	

#endif