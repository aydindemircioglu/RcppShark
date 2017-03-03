// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(BH)]]
//===========================================================================
/*!
 * 
 *
 * \brief       Implementation of Naive Bayes classifier
 * 
 * 
 * 
 *
 * \author      B. Li
 * \date        2012
 *
 *
 * \par Copyright 1995-2017 Shark Development Team
 * 
 * <BR><HR>
 * This file is part of Shark.
 * <http://shark-ml.org/>
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
//===========================================================================
#ifndef SHARK_MODEL_NB_CLASSIFIER_H
#define SHARK_MODEL_NB_CLASSIFIER_H

#include "shark/Core/Exception.h"
#include "shark/Core/Math.h"
#include "shark/Models/AbstractModel.h"

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <limits>
#include <utility>
namespace shark {

/// @brief Naive Bayes classifier
///
/// This model summarizes a Naive Bayes classifier, which assumes that the data X is generated by a mixture
/// of class-conditional (i.e., dependent on the value of the class variable Y) distributions. Furthermore, the Naive Bayes
/// assumption introduces the additional constraint that the attribute values Xi are independent of one another within
/// each of these mixture components.
template <class InputType = RealVector, class OutputType = unsigned int>
class NBClassifier :
	public AbstractModel<InputType, OutputType>,
	private boost::noncopyable
{
private:

	typedef AbstractModel<InputType, OutputType> base_type;

public:

	typedef typename base_type::BatchInputType BatchInputType;
	typedef typename base_type::BatchOutputType BatchOutputType;

	/// Type of class distribution
	typedef std::vector<double> ClassPriorsType;

	typedef boost::shared_ptr<AbstractDistribution> AbstractDistPtr;

	/// Type of features distribution
	typedef std::vector<std::vector<AbstractDistPtr> > FeatureDistributionsType;

	/// Size of distribution in format of (number of classes, number of features)
	typedef std::pair<std::size_t, std::size_t> DistSizeType;

	/// Ctor
	/// Will build hypothesis that all features in each class follows Normal distribution
	/// @param classSize size of class
	/// @param featureSize size of feature
	NBClassifier(std::size_t classSize, std::size_t featureSize)
	{
		SIZE_CHECK(classSize > 0u);
		SIZE_CHECK(featureSize > 0u);
		for (std::size_t i = 0; i < classSize; ++i)
		{
			std::vector<AbstractDistPtr> featureDist;
			for (std::size_t j = 0; j < featureSize; ++j)
				featureDist.push_back(AbstractDistPtr(new Normal<DefaultRngType>(Rng::globalRng)));
			m_featureDistributions.push_back(featureDist);
		}
	}

	/// Ctor
	/// The distributions for each feature in each class are given by @a featureDists
	/// @param featureDists the distribution of features
	explicit NBClassifier(FeatureDistributionsType const& featureDists)
	: m_featureDistributions(featureDists)
	{
		SIZE_CHECK(m_featureDistributions.size() > 0u);
	}

	/// \brief From INameable: return the class name.
	std::string name() const
	{ return "NBClassifier"; }

	/// Get a feature distribution for feature @a featureIndex given class @a classIndex
	/// @param classIndex index of class
	/// @param featureIndex index of feature
	/// @return the distribution for feature @a featureIndex given class @a classIndex
	AbstractDistribution& getFeatureDist(std::size_t classIndex, std::size_t featureIndex) const
	{
		SIZE_CHECK(classIndex < m_featureDistributions.size());
		SIZE_CHECK(featureIndex < m_featureDistributions[0].size());

		AbstractDistPtr const& featureDist = m_featureDistributions[classIndex][featureIndex];
		SHARK_ASSERT(featureDist);
		return *featureDist;
	}

	/// Get the size of distribution in format of (class size, feature size)
	/// @return the size of distribution
	DistSizeType getDistSize() const
	{
		SIZE_CHECK(m_featureDistributions.size() > 0u);
		return std::make_pair(m_featureDistributions.size(), m_featureDistributions[0].size());
	}

	using base_type::eval;
	
	boost::shared_ptr<State> createState()const{
		return boost::shared_ptr<State>(new EmptyState());
	}

	/// see AbstractModel::eval
	void eval(BatchInputType const& patterns, BatchOutputType& outputs, State& state)const{
		SIZE_CHECK(m_featureDistributions.size() == m_classPriors.size());
		SIZE_CHECK(m_classPriors.size() > 0u);
		SIZE_CHECK(size(patterns) > 0);
		
		outputs.resize(size(patterns));

		for(std::size_t p = 0; p != size(patterns); ++p){
			OutputType bestProbClass = 0; // just initialized to avoid warning 
			double maxLogProb = - std::numeric_limits<double>::max(); // initialized as smallest negative value

			// For each of possible output values, calculate its corresponding sum-up log prob and return the max one
			for(OutputType classIndex = 0; classIndex != m_classPriors.size(); ++classIndex){
				SIZE_CHECK(patterns.size2() == m_featureDistributions[classIndex].size());
				double const classDistribution = m_classPriors[classIndex];
				// Sum up log prob of each features and prior prob of current class
				// We use log to ensure that the result stays in a valid range of double, even when the propability is very low
				double currentLogProb = safeLog(classDistribution); 
				std::size_t featureIndex = 0u;
				for(auto const& featureDistribution: m_featureDistributions[classIndex])
					currentLogProb += featureDistribution->logP(patterns(p,featureIndex++));

				// Record the greater one
				if (currentLogProb > maxLogProb)
				{
					maxLogProb = currentLogProb;
					bestProbClass = classIndex;
				}
			}
			SHARK_ASSERT(maxLogProb != - std::numeric_limits<double>::max());//should never happen!
			outputs(p) = bestProbClass;
		}
	}

	/// Set prior distribution of @a class to be @a probability
	/// @param classToAdd the class of which probability will be updated
	/// @param probability the probability of the class
	/// @tparam OutputType the type of output class
	void setClassPrior(OutputType classToAdd, double probability)
	{
		if (classToAdd == m_classPriors.size())
			m_classPriors.push_back(probability);
		else
			throw SHARKEXCEPTION("[NBClassifier] class probability must be added in ascending order.");
	}
	
	/// This model does not have any parameters.
	RealVector parameterVector() const {
		return RealVector();
	}

	/// This model does not have any parameters
	void setParameterVector(const RealVector& param) {
		SHARK_ASSERT(param.size() == 0);
	}

protected:

	/// Feature and class distributions
	///@{
	FeatureDistributionsType m_featureDistributions;
	ClassPriorsType m_classPriors;
	///@}
};

} // namespace shark {

#endif // SHARK_MODEL_NB_CLASSIFIER_H

