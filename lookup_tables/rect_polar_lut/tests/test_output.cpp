#include <iostream>
#include "rect_polar_lut.hpp"

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


int main() {
    if ( run_test() != 0 ) {
        std::cout << "Test failed..\n";
    } else {
        std::cout << "Test Passed..\n";
    }
    return 0;
}

