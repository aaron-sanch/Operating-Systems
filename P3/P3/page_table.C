#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;

// Used the tutorial at http://www.osdever.net/tutorials/view/implementing-basic-paging
// in order to learn how to do basic constructor and other functions

void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   shared_size = _shared_size;
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;

   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   // maybe something before these things? idk it says create the first page table before
   // paging has been enabled

   // get frames gives us the next free frame, so we are not getting the same address
   // personal note, as I keep forgetting and rewriting this
   page_directory = (unsigned long *) (PAGE_SIZE * kernel_mem_pool->get_frames(1));
   page_table = (unsigned long *) (PAGE_SIZE * kernel_mem_pool->get_frames(1)); 

   // we want to create the array here, which i think is page directory
   unsigned long address=0; // holds the physical address of where a page is
   unsigned int i;

   // map the first 4MB of memory
   // we want to then initialize the first 4MB as valid
   for (i = 0; i < ENTRIES_PER_PAGE; i++) 
   {
      page_table[i] = address | 3; // attribute set to: supervisor level, read/write, present(011 in binary)
      address = address + PAGE_SIZE; // 4096 = 4kb
   }
   
   // fill the first entry of the page directory
   page_directory[0] = (unsigned long) page_table; // attribute set to: supervisor level, read/write, present(011 in binary)
   page_directory[0] = page_directory[0] | 3;

   // we then want to manage the remaining pages explicitly
   for(i=1; i < ENTRIES_PER_PAGE; i++)
   {
      page_directory[i] = 0 | 2; // attribute set to: supervisor level, read/write, not present(010 in binary)
   }
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   // set the page directory into cr3 to write
   // I think this is what write_cr3 is supposed to do, seems to  work
   write_cr3((unsigned long) page_directory);

   current_page_table = this;
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   //set static enabled
   paging_enabled = true;
   write_cr0(read_cr0() | 0x80000000); 
   // set the paging bit in CR0 to 1, I dont know why I am using 0x8000000, but the tutorial uses it
   // and it seems to work
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  unsigned long addr = read_cr2();
  // want to use but dont have
  unsigned long * page_directory = current_page_table->page_directory;

  // want first 10 bit
  unsigned long idx = addr >> 22;
  // want second 10 bits
  unsigned long ipt_idx = (addr >> 12) & 0x3FF;

   // run if err_code last bit is not true
   if ((_r->err_code & 1) == 0) {
      // if last bit = 0, no inner page table
      if ( (page_directory[idx] & 1) == 0) {
         // no inner page table
         // create inner page table without last 12 bits
         unsigned long* inner_page_table = (unsigned long *) ((PAGE_SIZE * kernel_mem_pool->get_frames(1)) & 0xfffff000); 
         // set page_directory at index = to the inner page table
         page_directory[idx] = (unsigned long) inner_page_table;
         for (unsigned int i = 0; i < ENTRIES_PER_PAGE; i++) {
            // set inner page table entries = u/s = 1 and read write bit = 1, 
            // with valid/invalid bit = 0, as we have not yet moved into our physical mem
            inner_page_table[i] = 2;   // can i not use 7 here
         }
         // set inner page table at the inner page table index = address of the process memory pool frame or'd with 3
         inner_page_table[ipt_idx] = (PAGE_SIZE * ((unsigned long) process_mem_pool->get_frames(1))) | 3;
      }
      else {
         // create inner page table without last 12 bits
         unsigned long* inner_page_table = (unsigned long*) (page_directory[idx] & 0xfffff000);
         // set page_directory at index = to the inner page table
         page_directory[idx] = (unsigned long) inner_page_table;
         // set inner page table at the inner page table index = address of the process memory pool frame or'd with 3
         inner_page_table[ipt_idx] = (PAGE_SIZE * ((unsigned long) process_mem_pool->get_frames(1))) | 3;
      }
   }
  Console::puts("handled page fault\n");
}

