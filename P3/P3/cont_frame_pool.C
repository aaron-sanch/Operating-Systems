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

ContFramePool* ContFramePool::frames[69];
unsigned long ContFramePool::index = 0;

unsigned long ContFramePool::get_start_frame() {
    if (info_frame_no == 0) {return base_frame_no;}
    return info_frame_no;
}
unsigned long ContFramePool::get_length() {
    return n_frames;
}

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames) :
                            base_frame_no(_base_frame_no),
                            n_frames(_n_frames),
                            info_frame_no(_info_frame_no),
                            n_info_frames(_n_info_frames)
{
    if (_info_frame_no == 0) {
        // info frame 0 case
        area = (char *)(base_frame_no * FRAME_SIZE);
        for (unsigned long i = 0; i < _n_frames; i++) {
            area[i] = FREE;
        }
        for (unsigned long i = 0; i < needed_info_frames(_n_info_frames); i++) {
            // create info frame pool area
            area[i] = INACCESSIBLE;
        }
    }
    else {
        area = (char *)(_info_frame_no * FRAME_SIZE);
        // otherwise just create the regular frame pool
        for (unsigned long i = 0; i < _n_frames; i++) {
            area[i] = FREE;
        }
    }
    
    // add the frame pool to our static array that holds all of the arrays
    frames[index] = this;
    index++;
}

bool ContFramePool::frames_available(unsigned long curr_pos, unsigned long _n_frames) {
    if((curr_pos + _n_frames) > (n_frames)) {
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
    for(unsigned long i = 0; i < (n_frames); i++) {
        // Makes sure there is enough space as well
        if (area[i] == FREE && frames_available(i, _n_frames)) {
            first_free_frame = i;
            break;
        }
        // Deals with if no frames are available
        if (i == ( n_frames - 1)) {
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
   for(unsigned long i = _base_frame_no; i < (_base_frame_no + _n_frames); i++) {
        area[i] = INACCESSIBLE;
    }
}

bool ContFramePool::check_inaccessible(unsigned long curr_pos) {
    for(unsigned long i = curr_pos; i < (n_frames); i++) {
        if (area[i] == ALLOCATED) {
            return true;
        }
        else if (area[i] == FREE || area[i] == HEAD_OF_SEQUENCE) {
            return false;
        }
    }
    return false;
}

void ContFramePool::release_helper(unsigned long _first_frame_no)
{
    area[_first_frame_no] = FREE;
    _first_frame_no++;
    while (area[_first_frame_no] == ALLOCATED || area[ _first_frame_no] == INACCESSIBLE) {
        // This means there was a bad alloc
        if (area[_first_frame_no] == INACCESSIBLE && check_inaccessible(_first_frame_no + 1)) {
            return;
        }
        else {
            area[ _first_frame_no] = FREE;
            _first_frame_no++;
        }
    }
    return;
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    for (int i = 0; i < 69; i++) {
        // Check we are within the range of the frame pool that is at index i
        if (_first_frame_no >= frames[i]->get_start_frame() && _first_frame_no < (frames[i]->get_start_frame() + frames[i]->get_length())) {
            // call release helper for the frame pool we are on, but we want the index of the frame, not the overall 
            // frame value, that would cause an out of bounds error for release_helper as it accesses the area[_first_frame_no],
            // so we will subtract from the start frame, so we get the index
            frames[i]->release_helper(_first_frame_no - frames[i]->get_start_frame());

            // return, theres not gonna be more than one pool at this location
            return;
        }
    }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    return (_n_frames / FRAME_SIZE + (_n_frames % FRAME_SIZE > 0 ? 1 : 0));
}
