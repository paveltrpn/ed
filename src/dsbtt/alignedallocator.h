//
//  alignedallocator.h
//  tracer
//
//  Created by Rasmus Barringer on 2013-02-20.
//  Copyright (c) 2013 Lund University. All rights reserved.
//

#ifndef tracer_alignedallocator_h
#define tracer_alignedallocator_h

#include <new>
#include <cstdio>
#include <xmmintrin.h>

// ============================================================================
// ============================= AlignedAllocator =============================
// ============================================================================
template <typename T, std::size_t Alignment>
class AlignedAllocator {
public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

// ============================================================================
// ================================== rebind ==================================
// ============================================================================
    template <typename U>
    struct rebind {
        typedef AlignedAllocator<U, Alignment> other;
    };

    AlignedAllocator() {}

    AlignedAllocator( const AlignedAllocator& ) {}

    template <typename U>
    AlignedAllocator( const AlignedAllocator<U, Alignment>& ) {}

    auto max_size() const -> std::size_t {
        //
        return ( static_cast<std::size_t>( 0 ) - static_cast<std::size_t>( 1 ) ) / sizeof( T );
    }

    auto address( T& r ) const -> T* {
        //
        return &r;
    }

    auto address( const T& s ) const -> const T* {
        //
        return &s;
    }

    auto construct( T* const p, const T& t ) const -> void {
        auto* const pv{ static_cast<void*>( p ) };
        new ( pv ) T{ t };
    }

    auto destroy( T* const p ) const -> void {
        //
        p->~T();
    }

    auto operator==( const AlignedAllocator& other ) const -> bool {
        //
        return true;
    }

    auto operator!=( const AlignedAllocator& other ) const -> bool {
        //
        return !( *this == other );
    }

    auto allocate( const std::size_t n ) const -> T* {
        if ( n == 0 ) {
            return NULL;
        }

        if ( n > max_size() ) {
            throw std::bad_alloc();
        }

        auto* const pv{ _mm_malloc( n * sizeof( T ), Alignment ) };

        if ( pv == NULL ) {
            throw std::bad_alloc();
        }

        return static_cast<T*>( pv );
    }

    auto deallocate( T* const p, const std::size_t n ) const -> void {
        //
        _mm_free( p );
    }

    template <typename U>
    auto allocate( const std::size_t n, const U* ) const -> T* {
        //
        return allocate( n );
    }
};

#endif
