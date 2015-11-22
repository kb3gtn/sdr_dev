#ifndef __RECT_POLAR_LUT_HPP
#define __RECT_POLAR_LUT_HPP
#define using_cpp

// if using c++ enviroment enable exceptions...
#ifdef using_cpp
#include <exception>
class LUT_Exception;
#endif


// data struct for a rect vector in X,Y
struct rect_vector {
    int8_t x;
    int8_t y;
};

// data struct for a polar vector in Mag,NRA
struct polar_vector {
    uint8_t mag;
    uint8_t nra;
};

//If using c++, provide stream operator to print rv,pv as text
#ifdef using_cpp
#include <ostream>
// add method for stream operator to print what a rect_vector is
std::ostream& operator <<(std::ostream& os, const rect_vector &v);

// add method for stream operator to print what a polar_vector is
std::ostream& operator <<(std::ostream& os, const polar_vector &v);
#endif


// Function prototypes for lookup operations
struct polar_vector convert_rect_2_polar( rect_vector rv );
struct rect_vector convert_polar_2_rect( polar_vector pv );

#endif

//EOF

