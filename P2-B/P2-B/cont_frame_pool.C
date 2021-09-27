/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*

 Notice that the differences between the class ContFramePool and your P2-A
 Manager are few:

 (1) The constructor is different. For the Manager class, the constructor was
     given a memory to store the array representing the state of the frames
     being managed.
     In ContFramePool, the constructor receives only information about which
     frames are being managed, and if additional frames are being given to store
     the array representing state.

 (2) The release_frames is now static
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
*/

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"


ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames) :
                            base_frame_no(_base_frame_no),
                            n_frames(_n_frames),
                            info_frame_no(_info_frame_no),
                            n_info_frames(_n_info_frames)
{
    
}

bool ContFramePool::frames_available(unsigned long curr_pos, unsigned long _n_frames) {
    if((curr_pos + _n_frames) > (n_frames + base_frame_no)) {
        return false;
    }
    for (unsigned long i = curr_pos + 1; i < (curr_pos + _n_frames); i++){
        if (area[i] != FREE) {
            return false;
        }
    }
    return true;
}

unsigned long ContFramePool::get_frames(unsigned long _n_frames)
{
    unsigned long first_free_frame;
    for(unsigned long i = base_frame_no; i < (base_frame_no + n_frames); i++) {
        // Makes sure there is enough space as well
        if (area[i] == FREE && frames_available(i, _n_frames)) {
            first_free_frame = i;
            break;
        }
        // Deals with if no frames are available
        if (i == (base_frame_no + n_frames - 1)) {
            return 0;
        }
    }
    area[first_free_frame] = HEAD_OF_SEQUENCE;
    for(unsigned long i = first_free_frame + 1; i < (first_free_frame + _n_frames); i++) {
        area[i] = ALLOCATED;
    }
    
    return first_free_frame;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    assert(false);
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    assert(false);
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    return (_n_frames / FRAME_SIZE + (_n_frames % FRAME_SIZE > 0 ? 1 : 0));
}
