/* program to generate a lookup table header 
   the provides rectanglar to polar and polar to rectanglar conversions

   Rectanglar space:
   Rectanglar space is (x,y) where  -127 <= x <= 127 and -127 <= y <= 127
   
   Polar space:
   Polar space is (mag,nra) where  0 <= mag <= 180  and 0 <= nra <= 255
   nra is Normalized Radian Angle
   it maps radian angles of 0->2*PI to 0->256

   All values are 8 bit values.   x,y are signed 8 bit values 
   mag and nra are unsigned 8 bit values

*/

#include <iostream>
#include <fstream>
#include <utility>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <cstdlib>
#include <exception>
#include <string>

class LUT_Exception : public std::exception
{
    std::string s;
public:
    LUT_Exception( std::string ss) : s(ss) {}
    ~LUT_Exception() throw() {}
    const char * what() const throw() { return s.c_str(); }
};



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

// add method for stream operator to print what a rect_vector is
std::ostream& operator <<(std::ostream& os, const rect_vector &v) {
    os << "{" << std::setw(4) << (int)v.x << "," << std::setw(4) << (int)v.y << "}";
    return os;
}

// add method for stream operator to print what a polar_vector is
std::ostream& operator <<(std::ostream& os, const polar_vector &v) {
    os << "{" << std::setw(3) <<  (int)v.mag << "<" << std::setw(3) << (int)v.nra << "}";
    return os;
}


// Lookup table storage for rect 2 polar
// This LUT contains lookups for the first quadurant
// convertion to other quadurants can be perform via simple 90 degree
// translations to get values in other quadurants.
struct polar_vector rect_2_polar_lut[128][128];

// Lookup table for polar 2 rect
// This LUT contains lookups for the first quadurant.
// convertion to other quadurants can be perform via simple 90 degree
// translations to get values in other quadurants.
struct rect_vector polar_2_rect_lut[181][64];


// utility function to convert radians to NRA
uint8_t radians_2_nra( double radian_angle ) {
    if ( radian_angle < 0 ) {
        radian_angle += 2*M_PI;
    }
    uint8_t nra = (uint8_t)round( (radian_angle/(2.0*M_PI))*256.0 );
    return nra;
}

// utility function to convert NRA to radians
double nra_2_radians( uint8_t nra ) {
    return ((double)nra/256.0)*(2.0*M_PI);
}
 
// build lookup table of polar_vectors
void populate_rect2polar_lut() {
    int x,y;
    polar_vector pv;
    for ( x=0; x < 128; x++ ) {
        for ( y=0; y < 128; y++ ) {
            pv.mag = round( sqrt( pow(x,2) + pow(y,2) ) );
            pv.nra = radians_2_nra( atan2( y, x ) );
            rect_2_polar_lut[x][y] = pv;
        }
    }
}
                    
// build lookup table of ractangular vectors
void populate_polar2rect_lut() {
    int mag,nra;
    int x,y;
    rect_vector rv;
    for ( mag=0; mag < 181; mag++ ) {
        for (nra=0; nra < 64; nra++ ) {
            x = mag * cos( nra_2_radians( nra ) );
            y = mag * sin( nra_2_radians( nra ) );
            // saturate for mag,nra which produce x,y over |127|
            if ( x > 127 ) x=127;
            if ( y > 127 ) y=127;
            rv.x = x;
            rv.y = y;
            polar_2_rect_lut[mag][nra] = rv;
        }
    }
}

// perform lookup operation
struct rect_vector convert_polar_2_rect( polar_vector pv ) {
    rect_vector rv;
    if ( pv.nra < 64 ) {
        // first quadrant lookup
        rv = polar_2_rect_lut[pv.mag][pv.nra];
    } 
    if ( (pv.nra >= 64) && (pv.nra < 128) ) {
        // second quadrant
        rv = polar_2_rect_lut[pv.mag][pv.nra-64];
        uint8_t t = rv.x;
        rv.x = rv.y*-1;
        rv.y = t;
    }
    if ( (pv.nra >= 128) && ( pv.nra < 192) ) {
        // third quadrant
        rv = polar_2_rect_lut[pv.mag][pv.nra-128];
        rv.x = rv.x*(-1);
        rv.y = rv.y*(-1);
    }
    if ( (pv.nra >= 192) && ( pv.nra <= 255) ) {
        rv = polar_2_rect_lut[pv.mag][pv.nra-192];
        uint8_t t = rv.x;
        rv.x = rv.y;
        rv.y = t*-1;
    }
/*  not need if using uint8_t nra values..
    // 256 = 2*PI => 0  roll over..
    if (pv.nra == 256 ) { rv = polar_2_rect_lut[pv.mag][0]; };
    // if over 256, can not directly convert this.
    if (pv.nra > 256 ) { 
        rv = polar_2_rect_lut[0][0];
        throw LUT_Exception( std::string("cannot lookup angle over 2*PI\n"));
    };
*/
    return rv;
}

struct polar_vector convert_rect_2_polar( rect_vector rv ) {
    polar_vector pv;
    if (( rv.x > 127 ) || (rv.x < -127 )) {
        pv.mag = 0;
        pv.nra = 0;
        throw LUT_Exception( std::string("cannot lookup x (x>127 or x<-127)"));
    }
    if (( rv.y > 127 ) || (rv.y < -127 )) {
        pv.mag = 0;
        pv.nra = 0;
        throw LUT_Exception( std::string("cannot lookup y (y>127 or y<-127)"));
    }

    if ( (rv.x >= 0) && (rv.y >= 0 ) ) {
        // first quad
        pv = rect_2_polar_lut[rv.x][rv.y];
    }
    if ( (rv.x < 0) && (rv.y > 0 ) ) {
        // second quad
        pv = rect_2_polar_lut[(rv.x*-1)][rv.y];
        pv.nra += 64;
    }
    if ( ( rv.x <= 0 ) && ( rv.y <= 0 ) ) {
        // third quad
        pv = rect_2_polar_lut[(rv.x*-1)][(rv.y*-1)];
        pv.nra += 128;
    }
    if ( ( rv.x > 0 ) && ( rv.y < 0) ) {
        // forth quad
        pv = rect_2_polar_lut[rv.x][(rv.y*-1)];
        pv.nra += 192;
    }

    return pv;
}
        
// run a test on a rect lookup operation
int run_p2r_test( uint8_t mag, uint8_t nra, int8_t expected_x, int8_t expected_y) {
    polar_vector pv;
    rect_vector rv;
    pv.mag = mag, pv.nra = nra;
    rv = convert_polar_2_rect( pv );
    std::cout << pv << " => " << rv << "  .........  ";
    if (( rv.x == expected_x ) && ( rv.y == expected_y )) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL ";
        std::cout << "Expected { " << (int)expected_x << "," << (int)expected_y << " } " << std::endl;
        return -1;
    }
    return 0;
}

// run a test on a polar lookup operation
int run_r2p_test( int8_t x, int8_t y, uint8_t expected_mag, uint8_t expected_nra) {
    polar_vector pv;
    rect_vector rv;
    rv.x = x, rv.y = y;
    pv = convert_rect_2_polar( rv );
    std::cout << rv << " => " << pv << "  .........  ";
    if (( pv.mag == expected_mag ) && ( pv.nra == expected_nra )) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL ";
        std::cout << "Expected { " << (int)expected_mag << "," << (int)expected_nra << " } " << std::endl;
        return -1;
    }
    return 0;
}


int run_test() {
    // test polar to rect lookups
    std::cout << "Testing Polar to Rect lookups....\n";
    // quad 1 0->90
    //                 mag,nra => x,y
    if ( run_p2r_test( 71, 0, 71, 0 ) != 0 ) { return -1; }
    if ( run_p2r_test( 71, 32, 50, 50 ) != 0 ) { return -1; }
    if ( run_p2r_test( 71, 64, 0, 71 ) != 0 ) { return -1; }
    // quad 2 90->180
    if ( run_p2r_test( 71, 96, -50, 50 ) != 0 ) { return -1; }
    if ( run_p2r_test( 71, 128, -71, 0 ) != 0 ) { return -1; }
    // quad 3 180 -> 270
    if ( run_p2r_test( 71, 160, -50, -50 ) != 0 ) { return -1; }
    if ( run_p2r_test( 71, 192, 0, -71 ) != 0 ) { return -1; }
    // QUAD 4 270 -> 360
    if ( run_p2r_test( 71, 224, 50, -50 ) != 0 ) { return -1; }
    std::cout << "Testing Rect to Polar lookups....\n";
    // QUAD 1
    //                 x,y => mag,nra
    if ( run_r2p_test( 71, 0, 71, 0 ) != 0 ) { return -1; }
    if ( run_r2p_test( 50, 50, 71, 32 ) != 0 ) { return -1; }
    if ( run_r2p_test( 0, 71, 71, 64 ) != 0 ) { return -1; }
    // Quad 2
    if ( run_r2p_test( -50, 50, 71, 96 ) != 0 ) { return -1; }
    if ( run_r2p_test( -71, 0, 71, 128 ) != 0 ) { return -1; }
    // Quad 3
    if ( run_r2p_test( -50, -50, 71, 160 ) != 0 ) { return -1; }
    if ( run_r2p_test( 0, -71, 71, 192 ) != 0 ) { return -1; }
    // Quad 4
    if ( run_r2p_test( 50, -50, 71, 224 ) != 0 ) { return -1; }
    return 0;
}

// write a c program with our lookup tables defined as constants.
int write_c_file() {
    std::fstream fh;
    fh.open( "rect_polar_lut.cpp", std::ios::out );
    if ( fh.is_open() == 0 ) {
        return -1;
    }
    fh << "/* Rect/Polar conversion tables & functions " << std::endl;
    fh << "*/\n";
    fh << "#include <cstdint>\n";
    fh << "#include \"rect_polar_lut.hpp\"\n";
    fh << "\n";
    fh << "/* Lookup table for Rect to Polar conversions */\n";
    fh << "struct polar_vector rect_2_polar_lut[128][128] = \n";
    fh << "{ \n";
    int x,y;
    polar_vector pv;
    rect_vector rv;
    for (x=0; x < 128; x ++ ) {
        fh << "  {  // mag,nra for x=" << (int)x << " and y=0:127\n    ";
        for (y=0; y < 128; y++ ) {
            pv = rect_2_polar_lut[x][y];
            fh << "{" << std::setw(3) << (int)pv.mag << "," << std::setw(3) << (int)pv.nra << "}";
            if (  (y != 127 ) && (y+1)%8 == 0) {
                // continue on next line
                fh << ",\n    ";
            } else {
                if ( y == 127 ) { 
                    // last line, no comma on end
                    fh << "\n";
                } else {
                    fh << ", ";
                }
            }
        }
        if ( x == 127 ) {
            fh << "  }\n";
        } else {
            fh << "  },\n";
        }
    }
    fh << "};\n";
    fh << "\n";
    fh << "/* Lookup table for Polar to Rect Conversions */\n";
    fh << "struct rect_vector polar_2_rect_lut[181][64] = \n";
    fh << "{ \n";
    int mag,nra;
    for (mag=0; mag < 181; mag ++ ) {
        fh << "  {  // x,y for mag=" << (int)mag << " and nra=0:63\n    ";
        for (nra=0; nra < 64; nra++ ) {
            rv = polar_2_rect_lut[mag][nra];
            fh << "{" << std::setw(4) << (int)rv.x << "," << std::setw(4) << (int)rv.y << "}";
            if (  (nra != 63 ) && (nra+1)%8 == 0) {
                // continue on next line
                fh << ",\n    ";
            } else {
                if ( nra == 63 ) { 
                    // last line, no comma on end
                    fh << "\n";
                } else {
                    fh << ", ";
                }
            }
        }
        if ( mag == 180 ) {
            fh << "  }\n";
        } else {
            fh << "  },\n";
        }
    }
    fh << "};\n\n";
    fh << "// perform lookup operation\n";
    fh << "struct rect_vector convert_polar_2_rect( polar_vector pv ) {\n";
    fh << "    rect_vector rv;\n";
    fh << "    if ( pv.nra < 64 ) {\n";
    fh << "        // first quadrant lookup\n";
    fh << "        rv = polar_2_rect_lut[pv.mag][pv.nra];\n";
    fh << "    }\n";
    fh << "    if ( (pv.nra >= 64) && (pv.nra < 128) ) {\n";
    fh << "        // second quadrant\n";
    fh << "        rv = polar_2_rect_lut[pv.mag][pv.nra-64];\n";
    fh << "        uint8_t t = rv.x;\n";
    fh << "        rv.x = rv.y*-1;\n";
    fh << "        rv.y = t;\n";
    fh << "    }\n";
    fh << "    if ( (pv.nra >= 128) && ( pv.nra < 192) ) {\n";
    fh << "        // third quadrant\n";
    fh << "        rv = polar_2_rect_lut[pv.mag][pv.nra-128];\n";
    fh << "        rv.x = rv.x*(-1);\n";
    fh << "        rv.y = rv.y*(-1);\n";
    fh << "    }\n";
    fh << "    if ( (pv.nra >= 192) && ( pv.nra <= 255) ) {\n";
    fh << "        rv = polar_2_rect_lut[pv.mag][pv.nra-192];\n";
    fh << "        uint8_t t = rv.x;\n";
    fh << "        rv.x = rv.y;\n";
    fh << "        rv.y = t*-1;\n";
    fh << "    }\n";
    fh << "    return rv;\n";
    fh << "}\n";
    fh << "\n";
    fh << "struct polar_vector convert_rect_2_polar( rect_vector rv ) {\n";
    fh << "    polar_vector pv;\n";
    fh << "    if (( rv.x > 127 ) || (rv.x < -127 )) {\n";
    fh << "        pv.mag = 0;\n";
    fh << "        pv.nra = 0;\n";
    fh << "        //throw LUT_Exception( std::string(\"cannot lookup x (x>127 or x<-127)\"));\n";
    fh << "    }\n";
    fh << "    if (( rv.y > 127 ) || (rv.y < -127 )) {\n";
    fh << "        pv.mag = 0;\n";
    fh << "        pv.nra = 0;\n";
    fh << "        //throw LUT_Exception( std::string(\"cannot lookup y (y>127 or y<-127)\"));\n";
    fh << "    }\n";
    fh << "    if ( (rv.x >= 0) && (rv.y >= 0 ) ) {\n";
    fh << "        // first quad\n";
    fh << "        pv = rect_2_polar_lut[rv.x][rv.y];\n";
    fh << "    }\n";
    fh << "    if ( (rv.x < 0) && (rv.y > 0 ) ) {\n";
    fh << "        // second quad\n";
    fh << "        pv = rect_2_polar_lut[(rv.x*-1)][rv.y];\n";
    fh << "        pv.nra += 64;\n";
    fh << "    }\n";
    fh << "    if ( ( rv.x <= 0 ) && ( rv.y <= 0 ) ) {\n";
    fh << "        // third quad\n";
    fh << "        pv = rect_2_polar_lut[(rv.x*-1)][(rv.y*-1)];\n";
    fh << "        pv.nra += 128;\n";
    fh << "    }\n";
    fh << "    if ( ( rv.x > 0 ) && ( rv.y < 0) ) {\n";
    fh << "        // forth quad\n";
    fh << "        pv = rect_2_polar_lut[rv.x][(rv.y*-1)];\n";
    fh << "        pv.nra += 192;\n";
    fh << "    }\n";
    fh << "    return pv;\n";
    fh << "}\n";
    fh << "\n";
    fh << "#ifdef using_cpp\n";
    fh << "#include <ostream>\n";
    fh << "#include <iomanip>\n";
    fh << "\n";
    fh << "// add method for stream operator to print what a rect_vector is\n";
    fh << "std::ostream& operator <<(std::ostream& os, const rect_vector &v) {\n";
    fh << "    os << \"{\" << std::setw(4) << (int)v.x << \",\" << std::setw(4) << (int)v.y << \"}\";\n";
    fh << "    return os;\n";
    fh << "}\n";
    fh << "\n";
    fh << "// add method for stream operator to print what a polar_vector is\n";
    fh << "std::ostream& operator <<(std::ostream& os, const polar_vector &v) {\n";
    fh << "    os << \"{\" << std::setw(3) <<  (int)v.mag << \"<\" << std::setw(3) << (int)v.nra << \"}\";\n";
    fh << "    return os;\n";
    fh << "}\n";
    fh << "\n";
    fh << "class LUT_Exception : public std::exception\n";
    fh << "{\n";
    fh << "    std::string s;\n";
    fh << "public:\n";
    fh << "    LUT_Exception( std::string ss) : s(ss) {}\n";
    fh << "    ~LUT_Exception() throw() {}\n";
    fh << "    const char * what() const throw() { return s.c_str(); }\n";
    fh << "};\n";
    fh << "\n";
    fh << "#endif\n";   
    fh << "\n";
    fh << "\n";
    fh.close();
    return 0;
}

int write_h_file() {
    std::fstream fh;
    fh.open( "rect_polar_lut.hpp", std::ios::out );
    if ( fh.is_open() == 0 ) {
        return -1;
    }
    fh << "#ifndef __RECT_POLAR_LUT_HPP\n";
    fh << "#define __RECT_POLAR_LUT_HPP\n";
    fh << "#define using_cpp\n";
    fh << "\n";
    fh << "// if using c++ enviroment enable exceptions...\n";
    fh << "#ifdef using_cpp\n";
    fh << "#include <exception>\n";
    fh << "class LUT_Exception;\n";
    fh << "#endif\n";
    fh << "\n";
    fh << "\n";
    fh << "// data struct for a rect vector in X,Y\n";
    fh << "struct rect_vector {\n";
    fh << "    int8_t x;\n";
    fh << "    int8_t y;\n";
    fh << "};\n";
    fh << "\n";
    fh << "// data struct for a polar vector in Mag,NRA\n";
    fh << "struct polar_vector {\n";
    fh << "    uint8_t mag;\n";
    fh << "    uint8_t nra;\n";
    fh << "};\n";
    fh << "\n";
    fh << "//If using c++, provide stream operator to print rv,pv as text\n";
    fh << "#ifdef using_cpp\n";
    fh << "#include <ostream>\n";
    fh << "// add method for stream operator to print what a rect_vector is\n";
    fh << "std::ostream& operator <<(std::ostream& os, const rect_vector &v);\n";
    fh << "\n";
    fh << "// add method for stream operator to print what a polar_vector is\n";
    fh << "std::ostream& operator <<(std::ostream& os, const polar_vector &v);\n";
    fh << "#endif\n";
    fh << "\n";
    fh << "\n";
    fh << "// Function prototypes for lookup operations\n";
    fh << "struct polar_vector convert_rect_2_polar( rect_vector rv );\n";
    fh << "struct rect_vector convert_polar_2_rect( polar_vector pv );\n";
    fh << "\n";
    fh << "#endif\n\n";
    fh << "//EOF\n\n";
    fh.close();
    return 0;
}

int main() {
    std::cout << "Building Polar/Rect 8-bit lookup tables\n";
    populate_polar2rect_lut();
    std::cout << "Building Rect/Polar 8-bit lookup tables\n";
    populate_rect2polar_lut();
    std::cout << "Running validation tests..\n";
    if ( run_test() != 0 ) {
        std::cout << "Test Failed, run aborted..\n";
        std::exit(-1);
    } else {
        std::cout << "All Tests Passed..\n";
    }
    // write results to output files
    std::cout << "Writing output to rect_polar_lut.cpp\n";
    if ( write_c_file() != 0 ) {
        std::cout << "Failed to write output file, exit...\n";
        return -1;
    } else {
        std::cout << "Finished writing output source file..\n";
    }
    if ( write_h_file() != 0 ) {
        std::cout << "Failed to write output file, exit...\n";
        return -1;
    } else {
        std::cout << "Finished writing output header file..\n";
    }
    return 0;
}
    

