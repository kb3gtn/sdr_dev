#ifndef __RECT_POLAR_LUT_HPP
#define __RECT_POLAR_LUT_HPP

// if using c++ enviroment enable exceptions...
//class LUT_Exception : public std::exception
//{
//    std::string s;
//public:
//    LUT_Exception( std::string ss) : s(ss) {}
//    ~LUT_Exception() throw() {}
//    const char * what() const throw() { return s.c_str(); }
//};


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
//// add method for stream operator to print what a rect_vector is
//std::ostream& operator <<(std::ostream& os, const rect_vector &v) {
//    os << "{" << std::setw(4) << (int)v.x << "," << std::setw(4) << (int)v.y << "}";
//    return os;
//}
//
//// add method for stream operator to print what a polar_vector is
//std::ostream& operator <<(std::ostream& os, const polar_vector &v) {
//    os << "{" << std::setw(3) <<  (int)v.mag << "<" << std::setw(3) << (int)v.nra << "}";
//    return os;
}


// Function prototypes for lookup operations
struct polar_vector convert_rect_2_polar( rect_vector rv );
struct rect_vector convert_polar_2_rect( polar_vector pv );

#endif

//EOF

