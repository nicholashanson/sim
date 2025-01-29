#include <performance_profile.hpp>

#include <mdspan-stable/include/mdspan/mdspan.hpp>

#include <vector>

#include "../grid.hpp"

const size_t xdim = 100;
const size_t ydim = 100;
const size_t zdim = 10;

using view = Kokkos::mdspan<float, Kokkos::extents<size_t, ydim, xdim, 5 * 6>>;

struct f : profiler::base_fixture<10,1> {
    f() = default;
};

const bool direct_vector_population = profiler::profile_avg<f>(
    "direct_vector_population",
    "vector_population",
    []( auto& fx, auto& start_time, auto& end_time ) {
        START_TIMER
        std::vector<float> data_v;

        for ( size_t y = 0; y < ydim; ++y ) {

            for ( size_t x = 0; x < xdim; ++x ) {

                data_v.insert( data_v.end(), {
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                });
            }
        }
        END_TIMER
    }
);

const bool grid_vector_population = profiler::profile_avg<f>(
    "grid_vector_population",
    "vector_population",
    []( auto& fx, auto& start_time, auto& end_time ) {
        START_TIMER
        std::vector<float> data_v( ydim * xdim * zdim * 5 * 6 );

        sim::grid<std::vector<float>, view> gd( data_v.data() );

        for ( size_t y = 0; y < ydim; ++y ) {

            for ( size_t x = 0; x < xdim; ++x ) {

                gd.set_cell_state_array({
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                    0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 
                }, y, x );
            }
        }
        END_TIMER
    }
);

