#include <performance_profile.hpp>

#include <mdspan-stable/include/mdspan/mdspan.hpp>

#include <vector>

#include "../grid.hpp"

const size_t xdim = 100;
const size_t ydim = 100;
const size_t zdim = 10;

using view = Kokkos::mdspan<double, Kokkos::extents<size_t, ydim, xdim, zdim>>;

struct f : profiler::base_fixture<10,1> {

    f() : gd( cell_states ) {
    }

    static std::vector<double> cell_states;
    sim::grid<std::vector<double>, view> gd;
};

std::vector<double> f::cell_states( xdim * ydim * zdim, 0.0 );

const bool array_writing = profiler::profile_avg<f>(
    "array_writing",
    "writing",
    []( auto& fx, auto& start_time, auto& end_time ) {
        START_TIMER
        for ( size_t y = 0; y < ydim; ++y ) {
            for ( size_t x = 0; x < xdim; ++x ) {

                std::array<double, zdim> cell_state = {};

                fx.gd.set_cell_state_array( cell_state, y, x );
            }
        }
        END_TIMER
    }
);

const bool element_wise_writing = profiler::profile_avg<f>(
    "element_wise_writing",
    "writing",
    []( auto& fx, auto& start_time, auto& end_time ) {
        START_TIMER
        for ( size_t y = 0; y < ydim; ++y ) {
            for ( size_t x = 0; x < xdim; ++x ) {
                for ( size_t z = 0; z < zdim; ++z )

                    fx.gd.set_cell_state( 0, y, x, z );
            }
        }
        END_TIMER
    }
);

const bool initializer_list_writing = profiler::profile_avg<f>(
    "initializer_list_writing",
    "writing",
    []( auto& fx, auto& start_time, auto& end_time ) {
        START_TIMER
        for ( size_t y = 0; y < ydim; ++y ) {
            for ( size_t x = 0; x < xdim; ++x ) {

                fx.gd.set_cell_state_array({
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }, y, x );
            }
        }
        END_TIMER
    }
);
