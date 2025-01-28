#ifndef GRID_HPP
#define GRID_HPP

#include <variant>
#include <iostream>
#include <cmath>
#include <array>
#include <iomanip>

namespace sim {

    template<typename DataStorage, typename View>
    class grid {
        public:

            using value_type_ = typename View::element_type;

            grid();

            // constructor for owning grid
            grid( DataStorage );
            // constructor for non-owning grid
            grid( typename DataStorage::pointer );

            grid( const grid& other );

            grid( grid&& other ) noexcept;

            grid& operator=( grid&& other ) noexcept;

#ifndef NDEBUG
            // equality operator only used for testing
            bool operator==( const grid& ) const;
#endif

            size_t get_dim( size_t dim ) const {
                return grid_.static_extent( dim );
            }

            template<typename... Indices>
            decltype( auto ) get_cell_state( Indices... ) const;

            template<typename... Indices>
            std::array<typename DataStorage::value_type, View::extents_type::static_extent( View::rank() - 1 )>
            get_cell_state_array( Indices... ) const;

            template<typename... Indices>
            void set_cell_state( const value_type_, Indices... );

            template<typename... Indices>
            void set_cell_state_array( const std::array<typename DataStorage::value_type, View::extents_type::static_extent( View::rank() - 1 )>&, Indices... );

            template<typename... Indices>
            void set_cell_state_array( const std::initializer_list<typename DataStorage::value_type> state, Indices... indices );

            typename DataStorage::pointer get_data_handle();

            const typename DataStorage::pointer get_data_handle() const;

#ifndef NDEBUG
            void print() const {

                const size_t ydim = get_dim( 0 );
                const size_t xdim = get_dim( 1 );

                for ( size_t y = 0; y < ydim; ++y ) {
                    for ( size_t x = 0; x < xdim; ++x ) {
                        if constexpr ( View::rank() == 3 ) {
                            for ( size_t q = 0; q < 9; ++q )
                                std::cout << std::fixed << std::setw(10) << std::setprecision(6) << grid_[ y, x, q ] << ", ";
                            std::cout << std::endl;
                        } else {
                            std::cout << std::fixed << std::setw(10) << std::setprecision(6) << grid_[ y, x ] << ", ";
                        }
                    }
                    std::cout << std::endl;
                }
            }
#endif

        private:
            // grid is either owning or non-owning
            std::variant<DataStorage, typename DataStorage::pointer> cell_states_;
            View grid_;
    };

    template<typename DataStorage, typename View>
    grid<DataStorage, View>::grid()
        : cell_states_( nullptr ), grid_( nullptr ) {
    }

    // constructor for an owning grid
    template<typename DataStorage, typename View>
    grid<DataStorage, View>::grid( DataStorage cell_states ) :
        cell_states_( std::move( cell_states ) ), grid_( std::get<DataStorage>( cell_states_ ).data() ) {
    }

    // constructor for a non-owning grid
    template<typename DataStorage, typename View>
    grid<DataStorage, View>::grid( typename DataStorage::pointer cell_states ) :
        cell_states_( cell_states ), grid_( std::get<typename DataStorage::pointer>( cell_states_ ) ) {
    }

    template<typename DataStorage, typename View>
    grid<DataStorage, View>::grid( const grid& other )
        : cell_states_( other.cell_states_ ), grid_( std::get<DataStorage>( cell_states_ ).data() ) {
    }

    template<typename DataStorage, typename View>
    grid<DataStorage, View>::grid( grid&& other ) noexcept
        : cell_states_( std::move( other.cell_states_ ) ), grid_( std::move( other.grid_ ) ) {
    }

    template<typename DataStorage, typename View>
    grid<DataStorage, View>& grid<DataStorage, View>::operator=( grid&& other ) noexcept {
        if ( this != &other ) {
            cell_states_ = std::move( other.cell_states_ );

            if ( std::holds_alternative<DataStorage>( cell_states_ ) )
                grid_ = View( std::get<DataStorage>( cell_states_ ).data() );
            else
                grid_ = View( std::get<typename DataStorage::pointer>( cell_states_ ) );
        }
        return *this;
    }

#ifndef NDEBUG
    // test for equality between two grids
    template<typename DataStorage, typename View>
    bool grid<DataStorage, View>::operator==( const grid& rhs ) const {

        const size_t ydim = get_dim( 0 );
        const size_t xdim = get_dim( 1 );

        for ( size_t y = 0; y < ydim; ++y )
            for ( size_t x = 0; x < xdim; ++x )
                if constexpr ( View::rank() == 3 ) {
                    for ( size_t q = 0; q < grid_.static_extent( 2 ); ++q ) {

                        if constexpr ( std::is_floating_point_v<value_type_> ) {
                            if ( std::fabs( grid_[ y, x, q ] - rhs.grid_[ y, x, q ] ) > 1e-6 ) {
                                return false;
                            }
                        } else {
                            if ( grid_[ y, x, q ] != rhs.grid_[ y, x, q ] ) {
                                return false;
                            }
                        }
                    }
                } else {
                    if ( grid_[ y, x ] != rhs.grid_[ y, x ] ) {
                        return false;
                    }
                }
        return true;
    }
#endif

    template<typename DataStorage, typename View>
    template<typename... Indices>
    decltype( auto ) grid<DataStorage, View>::get_cell_state( Indices... indices ) const {
        return grid_[ indices... ];
    }

    // returns cell's state as an array
    // useful when a cell's state is not a scalar
    template<typename DataStorage, typename View>
    template<typename... Indices>
    std::array<typename DataStorage::value_type, View::extents_type::static_extent( View::rank() - 1 )>
    grid<DataStorage, View>::get_cell_state_array( Indices... indices ) const {

        static_assert( sizeof...( indices ) == View::rank() - 1,
                       "Number of indices must match all dimensions except the last one" );

        constexpr auto array_size = View::extents_type::static_extent( View::rank() - 1 );
        std::array<typename DataStorage::value_type, array_size> cell_state;

        for ( size_t k = 0; k < array_size; ++k )
            cell_state[ k ] = grid_[ indices..., k ];

        return cell_state;
    }

    template<typename DataStorage, typename View>
    template<typename... Indices>
    void grid<DataStorage, View>::set_cell_state( const value_type_ state, Indices... indices ) {
        grid_[ indices... ] = state;
    }

    template<typename DataStorage, typename View>
    template<typename... Indices>
    void grid<DataStorage, View>::set_cell_state_array( const std::array<typename DataStorage::value_type, View::extents_type::static_extent( View::rank() - 1 )>& state, Indices... indices ) {

        static_assert( sizeof...( indices) == View::rank() - 1,
                       "Number of indices must match all dimension except the last one" );

        for ( size_t k = 0; k < state.size(); ++k )
            grid_[ indices..., k ] = state[ k ];
    }

    template<typename DataStorage, typename View>
    template<typename... Indices>
    void grid<DataStorage, View>::set_cell_state_array( const std::initializer_list<typename DataStorage::value_type> state, Indices... indices ) {

        static_assert( sizeof...( indices) == View::rank() - 1,
                       "Number of indices must match all dimension except the last one" );

        constexpr auto expected_len = View::extents_type::static_extent( View::rank() - 1 );
        
        if ( state.size() != expected_len ) {
            throw std::invalid_argument( "Initializer list size does not match the expected size" );
        }

        for ( size_t i = 0; auto& item: state )
            grid_[ indices..., i++ ] = item;
    }

    template<typename DataStorage, typename View>
    typename DataStorage::pointer grid<DataStorage, View>::get_data_handle() {
        if ( std::holds_alternative<DataStorage>( cell_states_ ) )
            return std::get<DataStorage>( cell_states_ ).data();
        else
            return std::get<typename DataStorage::pointer>( cell_states_ );
    }

    template<typename DataStorage, typename View>
    const typename DataStorage::pointer grid<DataStorage, View>::get_data_handle() const {
        if ( std::holds_alternative<DataStorage>( cell_states_ ) )
            return std::get<DataStorage>( cell_states_ ).data();
        else
            return std::get<typename DataStorage::pointer>( cell_states_ );
    }

}

#endif
