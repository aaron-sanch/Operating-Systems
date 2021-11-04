/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"

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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    // replace the assertion with your constructor code

    // You need to register this VMPool with the page table.
    // You can do that by invoking method PageTable::register_pool
    // The page_table object file that we provide (named page_table_provided.o)
    // contains this method.

    base_address = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;

    // create array at base address
    regions = (Region*) base_address;

    //initialize array
    for (int i = 0; i < (size / PageTable::PAGE_SIZE); i++) {
        Region region;
        region.start_address = 0;
        region.size = 0;
    }
}

unsigned long VMPool::allocate(unsigned long _size) {
    if (size == 0) {
        return 0;
    }
    // want to loop through array and see if there is a memory hole large enough

}

void VMPool::release(unsigned long _start_address) {
    // Notice that the regions being released may be in
    // the page tables. We want to unmap all pages in the
    // region being released, remove them from the page table.
    // To unmap virtual page addr you can use
    // method PageTable::free_page:
    //      page_table->free_page(addr)
    // The page_table object file that we provide (named page_table_provided.o)
    // contains this method.

        // replace the assertion with your code
    assert(false);
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    // replace the assertion with your code
    assert(false);
    return true;
}

