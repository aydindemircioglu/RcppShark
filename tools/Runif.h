/* boost random/linear_congruential.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id: linear_congruential.hpp 71018 2011-04-05 21:27:52Z steven_watanabe $
 *
 * Revision history
 *  2001-02-18  moved to individual header files
 */

#ifndef BOOST_47_H
#define BOOST_47_H

#include <iostream>
#include <stdexcept>
#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/static_assert.hpp>
#include <boost/integer/static_log2.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/random/detail/config.hpp>
#include <boost/random/detail/const_mod.hpp>
#include <boost/random/detail/seed.hpp>
#include <boost/random/detail/seed_impl.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/random/detail/disable_warnings.hpp>

#include <Rcpp.h>

using namespace Rcpp;

namespace boost {
namespace random {

/**
 * A random number generator that is based on the corresponding R functions
 */


class rand47
{
public:
    typedef boost::uint32_t result_type;

    BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);
    /**
     * Returns the smallest value that the generator can produce
     */
    static uint32_t min BOOST_PREVENT_MACRO_SUBSTITUTION () { return 0; }
    /**
     * Returns the largest value that the generator can produce
     */
    static uint32_t max BOOST_PREVENT_MACRO_SUBSTITUTION ()
    { return 0x7FFFFFFF; }
  
    /** Seeds the generator with the default seed. */
    rand47()  {}
    /**
     * Constructs a \rand47 generator with x(0) := (x0 << 16) | 0x330e.
     */
    BOOST_RANDOM_DETAIL_ARITHMETIC_CONSTRUCTOR(rand47, result_type, x0)
    { Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 478"); }
    /**
     * Seeds the generator with values produced by @c seq.generate().
     */
    BOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(rand47, SeedSeq, seq)
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 232"); }
    /**
     * Seeds the generator using values from an iterator range,
     * and updates first to point one past the last value consumed.
     */
    template<class It> rand47(It& first, It last) 
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 175"); }
	

    // compiler-generated copy ctor and assignment operator are fine

    /** Seeds the generator with the default seed. */
	void seed() { Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 555"); }
    /**
     * Changes the current value x(n) of the generator to (x0 << 16) | 0x330e.
     */
    BOOST_RANDOM_DETAIL_ARITHMETIC_SEED(rand47, result_type, x0)
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 311"); }
    /**
     * Seeds the generator using values from an iterator range,
     * and updates first to point one past the last value consumed.
     */
	template<class It> void seed(It& first, It last) { Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 151"); }
    /**
     * Seeds the generator with values produced by @c seq.generate().
     */
    BOOST_RANDOM_DETAIL_SEED_SEQ_SEED(rand47, SeedSeq, seq)
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 431"); }

    /**  Returns the next value of the generator. */
    uint32_t operator()() { return static_cast<uint32_t>( round(R::runif (0,1) * max() ) ); }
    
    /** Advances the state of the generator by @c z. */
	void discard(boost::uintmax_t z) { Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 665"); }
  
    /** Fills a range with random values */
    template<class Iter>
    void generate(Iter first, Iter last)
    {
        for(; first != last; ++first) {
            *first = (*this)();
        }
    }

    
    /**
     * Returns true if the two generators will produce identical
     * sequences of values.
     */
    friend bool operator==(const rand47& x, const rand47& y)
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 868"); return FALSE; }
    /**
     * Returns true if the two generators will produce different
     * sequences of values.
     */
    friend bool operator!=(const rand47& x, const rand47& y)
	{ Rcpp::stop ("Should not call this. Fix the random numbers generator if you need this. 498");; return TRUE; }
private:
    /// \cond show_private

    /// \endcond
};


} // namespace random

using random::rand47;

} // namespace boost

#include <boost/random/detail/enable_warnings.hpp>

#endif // BOOST_RANDOM_LINEAR_CONGRUENTIAL_HPP
