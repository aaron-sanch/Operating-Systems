/*
** File: manager.C
 
Your implementation needs to maintain free *sequences* of frames.
This can be done in many ways, ranging from bitmaps to free-lists of frames etc.
 
IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the idea of bitmaps: instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD_OF_SEQUENCE.
 If a frame is marked as HEAD_OF_SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 If we were keeping track only of FREE and ALLOCATED, we would need only
 one bit to represent the state of a frame in the pool.
 But with three possible states (FREE, ALLOCATED, HEAD_OF_SEQUENCE), we need
 at least two bits to present the state of a frame.
 
 Using two bits to present the state of a frame means that each element of the
 array (of chars) holding the bitmap will be representing four frames.
 If you want to simplify your programming and have one frame per array element
 (a bytemap instead of bitmap) you are wasting memory. 

 You can choose between adoption a bitmap or bytemap (there is a bonus for using
 a bitmap) 
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD_OF_SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE
 
 get_frames(n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD_OF_SEQUENCE and the remaining n_frames-1 as
 ALLOCATED.

 release_frames(first_frame_no): Check whether the first frame is marked as
 HEAD_OF_SEQUENCE. (If not, something went wrong.) If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD_OF_SEQUENCE. Until then, mark the frames that you traverse as FREE.

 Notice that the API also requires to keep track of innacessible frames,
 so you have a frame can be in one of four states: 
 FREE, ALLOCATED, HEAD_OF_SEQUENCE, and INACCESSIBLE
 
--------------------------------------------------------------------------*/

#include "manager.H"
#include <assert.h>

// Changed first argument from unsigned long to unsigned long long
// to compile in some VSCode environments
Manager::Manager(unsigned long long _map_ptr,
                unsigned long _n_frames,
                unsigned long _base_frame)

{
    // Basic assigning of variables
    // map_ptr might need to be changed back to a char * so that I can actually manipulate the values again
    // also might need to make map_ptr into a shallow copy instead of a deep copy, so that we dont have 2 versions of 
    // the array that could be different
    area = (char*) &_map_ptr;
    n_frames = _n_frames;
    base_frame = _base_frame;

    // Assign all of the values in map_ptr to free
    for (unsigned long i = base_frame; i < (base_frame + n_frames); i++) {
        area[i] = FREE;
    }
}

bool Manager::frames_available(unsigned long curr_pos, unsigned long _n_frames) {
    if((curr_pos + _n_frames) > (n_frames + base_frame)) {
        return false;
    }
    for (unsigned long i = curr_pos + 1; i < (curr_pos + _n_frames); i++){
        if (area[i] != FREE) {
            return false;
        }
    }
    return true;
}

unsigned long Manager::get_frames(unsigned long _n_frames)
{
    unsigned long first_free_frame;
    for(unsigned long i = base_frame; i < (base_frame + n_frames); i++) {
        // Makes sure there is enough space as well
        if (area[i] == FREE && frames_available(i, _n_frames)) {
            first_free_frame = i;
            break;
        }
        // Deals with if no frames are available
        if (i == (base_frame + n_frames - 1)) {
            return 0;
        }
    }
    area[first_free_frame] = HEAD_OF_SEQUENCE;
    for(unsigned long i = first_free_frame + 1; i < (first_free_frame + _n_frames); i++) {
        area[i] = ALLOCATED;
    }
    
    return first_free_frame;
}

bool Manager::release_frames(unsigned long _first_frame_no)
{
    area[_first_frame_no] = FREE;
    _first_frame_no++;
    while (area[_first_frame_no] == ALLOCATED || area[_first_frame_no] == INACCESSIBLE) {
        if (area[_first_frame_no] == INACCESSIBLE) {
            _first_frame_no++;
            return false;
        }
        else {
            area[_first_frame_no] = FREE;
            _first_frame_no++;
        }
    }
    return true;
}

void Manager::mark_inaccessible(unsigned long _starting_frame,
                                      unsigned long _n_frames)
{
    for(unsigned long i = _starting_frame; i < (_starting_frame + _n_frames); i++) {
        area[i] = INACCESSIBLE;
    }
}
int Manager::NumberBitsRepresentingFrame() {
    // Using bytes for the moment
    return 8; 
}

char Manager::get_frame_state(unsigned long _frame_nb) { 
    // Return array at frame
    return area[base_frame + _frame_nb]; 
}
