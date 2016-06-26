// [[Rcpp::depends(BH)]]
/*!
 * 
 *
 * \brief       Discrete Uniform distribution
 * 
 * 
 *
 * \author      O. Krause
 * \date        2010-01-01
 *
 *
 * \par Copyright 1995-2015 Shark Development Team
 * 
 * <BR><HR>
 * This file is part of Shark.
 * <http://image.diku.dk/shark/>
 * 
 * Shark is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Shark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Shark.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SHARK_RNG_DISCRETEUNIFORM_H
#define SHARK_RNG_DISCRETEUNIFORM_H

#include <shark/Rng/Rng.h>

#include <boost/random.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace shark{
/** 
* \brief Implements the discrete uniform distribution.
*/
template<typename RngType = shark::DefaultRngType>
class DiscreteUniform : public boost::variate_generator<RngType*,boost::uniform_int<std::size_t> > {
private:
	/** \brief The base type this class inherits from. */
	typedef boost::variate_generator<RngType*,boost::uniform_int<std::size_t> > Base;
public:

	/**
	* \brief C'tor, initializes the interval the random numbers are sampled from and associates the distribution
	* with the supplied RNG.
	* \param [in,out] rng The RNG this distribution is associated with.
	* \param [in] low_ The lower bound of the interval, defaults to 0.
	* \param [in] high_ The upper bound of the interval, defaults to std::numeric_limits<std::size_t>::max().
	*/
	DiscreteUniform(RngType & rng, std::size_t low_ = 0,std::size_t high_ = std::numeric_limits<std::size_t>::max() )
		:Base(&rng,boost::uniform_int<std::size_t>(std::min(low_,high_),std::max(low_,high_)))
	{}

	/**
	* \brief Injects the default sampling operator.
	*/
	using Base::operator();

	/**
	* \brief Reinitializes the distribution for the supplied bounds and samples a new random number.
	* Default values are omitted to distinguish the operator from the default one.
	* 
	* \param [in] low_ The lower bound of the interval.
	* \param [in] high_ The upper bound of the interval.
	*/
	typename Base::result_type operator()(std::size_t low_,std::size_t high_)
	{
		boost::uniform_int<std::size_t> dist(std::min(low_,high_),std::max(low_,high_));
		return dist(Base::engine());
	}

	/** \brief Returns the lower bound of the interval. */
	std::size_t low()const
	{
		return Base::distribution().min();
	}

	/** \brief Adjusts the upper bound of the interval */
	std::size_t high()const
	{
		return Base::distribution().max();
	}

	/** \brief Adjusts the range of the interval. */
	void setRange(std::size_t low_,std::size_t high_)
	{
		boost::uniform_int<std::size_t> dist(std::min(low_,high_),std::max(low_,high_));
		Base::distribution()=dist;
	}

	/** \brief Calculates the probability of x. */
	double p( std::size_t x ) const {
		return 1.0/(high()-low()+1);
	}

};

///\brief Draws a discrete number in {low,low+1,...,high} by drawing random numbers from rng.
template<class RngType>
std::size_t discrete(RngType& rng, std::size_t low, std::size_t high){
	DiscreteUniform<RngType> dist(rng, low, high);
	return dist();
}

}
#endif

