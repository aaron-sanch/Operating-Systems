/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  this->queue = Queue();
}

void Scheduler::yield() {
  if (queue.size() > 0) {
    Thread* popped_thread = queue.pop();
    // queue.push(popped_thread); // I think this might be needed to push to the back of the queue
    // Not sure if yield removes from ready queue or not
    Thread::dispatch_to(popped_thread);
    return;
  }
  Console::puts("Empty queue, cannot yield.\n");
}

void Scheduler::resume(Thread * _thread) {
  queue.push(_thread); // queue push adds to ready queue
  Console::puts("Size: ");
  Console::puti(queue.size());
  Console::puts("\n");
}

void Scheduler::add(Thread * _thread) {
  resume(_thread);
}

void Scheduler::terminate(Thread * _thread) {
  if (_thread == Thread::CurrentThread()) {
    // might need to yield here
    yield();
    delete _thread;
    return;
  }

  queue.destroy_thread(_thread);
}
