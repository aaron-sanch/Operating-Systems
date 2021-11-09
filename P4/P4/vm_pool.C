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
               PageTable     *_page_table): 
               regions_array_size(28),
               curr_size(0),
               regions() {
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
    page_table->register_pool(this);

    //initialize array
    for (int i = 0; i < (regions_array_size); i++) {
        Region region;
        region.start_address = base_address + 1;
        region.size = 0;
        //Console::puts("Starts allocate.\n");
        regions[i] = region;
        //Console::puts("Gets past the allocation of regions.\n");
    }
}

unsigned long VMPool::allocate(unsigned long _size) {
    if (size == 0 || curr_size == regions_array_size) {
        // size of vmpool is 0 we cant allocate
        return 0;
    }
    // add a counter

    // Calculating frames needed and rounding up
    unsigned long allocated_frames = (_size / PageTable::PAGE_SIZE);
    if (_size % PageTable::PAGE_SIZE > 0) {
        allocated_frames++;
    }

    if (curr_size == 0) {
        regions[0].size = allocated_frames * PageTable::PAGE_SIZE;
        regions[0].start_address = base_address + PageTable::PAGE_SIZE;
        curr_size++;
        // Console::puts((char*)(base_address + PageTable::PAGE_SIZE));
        return regions[0].start_address;
    }

    // want to loop through array and see if there is a memory hole large enough

    //Check if there is holes at the start of the array 
    unsigned long hole_size = 0;
    if (!check_base_addr()) {
        int curr_hole = find_next_hole(-1);
        hole_size = (regions[curr_hole].start_address + regions[curr_hole].size) - (base_address + PageTable::PAGE_SIZE);
        if (hole_size >= _size) {
            regions[curr_size].size = allocated_frames * PageTable::PAGE_SIZE;
            regions[curr_size].start_address = base_address + PageTable::PAGE_SIZE;
            curr_size++;
            return regions[curr_size - 1].start_address;
        }
    }

    for (int i = 0; i < curr_size; i++) {
        unsigned long hole_size = 0;
        int next_hole = find_next_hole(i);
        if (next_hole == -1) {
            hole_size = (base_address + size) - (regions[i].start_address + regions[i].size);
        }
        else {
            hole_size = (regions[i].start_address + regions[i].size) - (regions[next_hole].start_address + regions[next_hole].size);
        }

        if (hole_size >= _size) {
            regions[curr_size].size = allocated_frames * PageTable::PAGE_SIZE;
            regions[curr_size].start_address = regions[i].start_address + regions[i].size;
            curr_size++;
            return regions[curr_size - 1].start_address;
        }
    }
    return 0;
}

bool VMPool::check_base_addr() {
    for (int i = 0; i < curr_size; i++) {
        if (regions[i].start_address == (base_address + PageTable::PAGE_SIZE)) {
            return true;
        }
    }
    return false;
 }
int VMPool::find_next_hole(int curr) {
    int curr_closest = -1;
    for (int i = 0; i < curr_size; i++) {
        if (i == curr) {
            continue;
        }
        if (curr == -1) {
            if (regions[i].start_address > (base_address + PageTable::PAGE_SIZE) && (curr_closest == -1 || i < curr_closest)) {
                curr_closest = i;
            }
        }
        else if (regions[i].start_address > regions[curr].start_address && (curr_closest == -1 || i < curr_closest)) {
            curr_closest = i;
        }
    }
    return curr_closest;
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
    int val_for_release = -1;
    for (int i = 0; i < (curr_size); i++) {
        if (_start_address == regions[i].start_address) {
            val_for_release = i;
            break;
        }
    }
    if (val_for_release != -1) {
        for (unsigned long i = 0; i <= (regions[val_for_release].size / PageTable::PAGE_SIZE); i++) {
            page_table->free_page(_start_address + PageTable::PAGE_SIZE * (i + 1));
        }
    }
    //regions[val_for_release].start_address = base_address + 1;
    //regions[val_for_release].size = 0;
    for (unsigned long i = val_for_release; i < curr_size; i++) {
        regions[i].start_address = regions[i + 1].start_address;
        regions[i].size = regions[i + 1].size;
    }
    curr_size--;
    page_table->load();
}

bool VMPool::is_legitimate(unsigned long _address) {
    // check valid range    
    if (_address >= base_address && _address < (base_address + PageTable::PAGE_SIZE)){
        return true;
    }
    for (int i = 0; i < curr_size; i++) {
        if (regions[i].size == 0) {
            continue;
        }
        if (regions[i].start_address <= _address && (regions[i].start_address + size) > _address) {
            return true;
        }
    }
    return false;
}
