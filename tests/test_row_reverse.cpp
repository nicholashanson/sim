#include "../grid.hpp"
#include <vector>
#include <cassert>
#include <iostream>

#include <mdspan-stable/include/mdspan/mdspan.hpp>

using md3x1x9 = Kokkos::mdspan<double, Kokkos::extents<size_t, 3, 1, 9>>;

std::vector<double> before_reverse = {
    0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 
    0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 
    1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6,
};

std::vector<double> after_reverse = {
    1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6,
    0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 
    0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 
};

void test_reverse_rows() {

    sim::grid<std::vector<double>, md3x1x9> gd( before_reverse );

    gd.reverse_rows();

    sim::grid<std::vector<double>, md3x1x9> gd_expected( after_reverse );

    assert( gd == gd_expected && "reverse_rows test failed" );
}

int main() {

    test_reverse_rows();
    std::cout << "reverse_rows test passed" << std::endl;

    return 0;
}