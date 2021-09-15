/*
** File: main.C
*/

#include "manager.H"
#include <iostream>


int main() {
    // Testing with a pool for frames 1024 to 1535
    const unsigned long POOL_NB_FRAMES = 512;
    const unsigned long BASE_FRAME = 1024;

    unsigned int bytes_needed = 
               POOL_NB_FRAMES / (8 / Manager::NumberBitsRepresentingFrame());
    char* area = new char[bytes_needed];

    // unsigned long long so that it can run cleanly in environments
    // where unsigned long is 4 byte-long and char* is 8 byte-long
    Manager frame_pool = Manager((unsigned long long) area,
                                 POOL_NB_FRAMES, BASE_FRAME);

    // Make sure we can allocate and deallocate the entire frame space
    std::cout << frame_pool.get_frames(512) << std::endl;
    std::cout << frame_pool.release_frames(0) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(511) << std::endl;

    // create a hole starting at frames 1048 and ending at frame 1051
    frame_pool.mark_inaccessible(1048, 4);
    
    // Test your implementation by invoking get_frames and release_frames
    // If you have the Google Test framework on your environment, you can
    // use the tests in gtest.C
    // (You can easily find guides to install the Google Test Framework on
    // Mac, Windows, and Linux environments)

    // Test the allocation of all of our array
    std::cout << (unsigned long) frame_pool.get_frame_state(0) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(100) << std::endl;

    // Test getting frames
    // Can turn the mark inaccessible ln.24 on/off to try different configurations
    // Simple test first frame with just a single frame
    std::cout << frame_pool.get_frames(1) << std::endl;
    // Test if we can put in frames with inaccessible turned off/on
    // When turned off we get it to allocate, when on no allocation
    std::cout << frame_pool.get_frames(508) << std::endl;
    // Try and allocate, when inaccessible is turned off we should
    // not be able to allocate as we are too large, when on we should
    // be able to allocate
    std::cout << frame_pool.get_frames(4) << std::endl; 
    //Head
    std::cout << (unsigned long) frame_pool.get_frame_state(0) << std::endl;
    //Head
    std::cout << (unsigned long) frame_pool.get_frame_state(1) << std::endl;
    //Allocated
    std::cout << (unsigned long) frame_pool.get_frame_state(2) << std::endl;
    //Inaccessible
    std::cout << (unsigned long) frame_pool.get_frame_state(24) << std::endl;
    //Free
    std::cout << (unsigned long) frame_pool.get_frame_state(90) << std::endl;


    std::cout << frame_pool.release_frames(0) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(0) << std::endl;

    std::cout << frame_pool.release_frames(1) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(1) << std::endl;


    // Test a longer release next to the inaccessible
    std::cout << frame_pool.get_frames(24) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(0) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(23) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(24) << std::endl;
    std::cout << frame_pool.release_frames(0) << std::endl;
    std::cout << (unsigned long) frame_pool.get_frame_state(0) << std::endl;
}
