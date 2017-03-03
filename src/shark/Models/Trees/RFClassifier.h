//===========================================================================
/*!
 * 
 *
 * \brief       Random Forest Classifier.
 * 
 * 
 *
 * \author      K. N. Hansen, O.Krause, J. Kremer
 * \date        2011-2012
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

#ifndef SHARK_MODELS_TREES_RFCLASSIFIER_H
#define SHARK_MODELS_TREES_RFCLASSIFIER_H

#include <shark/Models/Trees/CARTClassifier.h>
#include <shark/Models/Trees/General.h>
#include <shark/Models/MeanModel.h>
#include <shark/Data/DataView.h>

namespace shark {

typedef CARTClassifier<RealVector>::TreeType TreeType;
typedef std::vector<TreeType> ForestInfo;

///
/// \brief Random Forest Classifier.
///
/// \par
/// The Random Forest Classifier predicts a class label
/// using the Random Forest algorithm as described in<br/>
/// Random Forests. Leo Breiman. Machine Learning, 1(45), pages 5-32. Springer, 2001.<br/>
///
/// \par
/// It is an ensemble learner that uses multiple decision trees built
/// using the CART methodology.
///
class RFClassifier : public MeanModel<CARTClassifier<RealVector> >
{
public:
	using SubmodelType = CARTClassifier<RealVector>;
	/// \brief From INameable: return the class name.
	std::string name() const
	{ return "RFClassifier"; }

	// compute the oob error for the forest
	void computeOOBerror(
			UIntMatrix const& oobClassTally,
			DataView<ClassificationDataset const>& elements){
		auto n_elements = elements.size();
		m_OOBerror = detail::cart::sum<double>(n_elements, [&](size_t i){
			auto y = elements[i].label;
			auto z = arg_max(row(oobClassTally,i));
			return static_cast<double>(y!=z);
		})/n_elements;
	}
	void computeOOBerror(
			RealMatrix const& oobTally,
			DataView<RegressionDataset const>& elements){
		auto n_elements = elements.size();
		m_OOBerror = detail::cart::sum<double>(n_elements,[&](size_t i){
			return distanceSqr(elements[i].label,row(oobTally,i));
		})/n_elements;
	}


	// compute the feature importances for the forest
	void computeFeatureImportances(){
		m_featureImportances.resize(m_inputDimension);
		std::size_t n_trees = numberOfModels();

		for(std::size_t i=0;i!=m_inputDimension;++i){
			m_featureImportances[i] = 0;
			for(std::size_t j=0;j!=n_trees;++j){
				m_featureImportances[i] += m_models[j].featureImportances()[i];
			}
			m_featureImportances[i] /= n_trees;
		}
	}

	double const OOBerror() const {
		return m_OOBerror;
	}

	// returns the feature importances
	RealVector const& featureImportances() const {
		return m_featureImportances;
	}

	//Count how often attributes are used
	UIntVector countAttributes() const {
		std::size_t n = m_models.size();
		if(!n) return UIntVector();
		UIntVector r = m_models[0].countAttributes();
		for(std::size_t i=1; i< n; i++ ) {
			noalias(r) += m_models[i].countAttributes();
		}
		return r;
	}

	/// Set the dimension of the labels
	void setLabelDimension(std::size_t in){
		m_labelDimension = in;
	}

	// Set the input dimension
	void setInputDimension(std::size_t in){
		m_inputDimension = in;
	}

	ForestInfo getForestInfo() const {
		ForestInfo finfo(m_models.size());
		for (std::size_t i=0; i<m_models.size(); ++i)
			finfo[i]=m_models[i].getTree();
		return finfo;
	}

	void setForestInfo(ForestInfo const& finfo, std::vector<double> const& weights = std::vector<double>()) {
		std::size_t n_tree = finfo.size();
		std::vector<double> we(weights);
		m_models.resize(n_tree);
		if (weights.empty()) // set default weights to 1
			we.resize(n_tree, 1);
		else if (weights.size() != n_tree)
			throw SHARKEXCEPTION("Weights must be the same number as trees");

		for (std::size_t i=0; i<n_tree; ++i){
			m_models[i]=SubmodelType{finfo[i]};
			m_weight.push_back(we[i]);
			m_weightSum+=we[i];
		}
	}

protected:
	// Dimension of label in the regression case, number of classes in the classification case.
	std::size_t m_labelDimension;

	// Input dimension
	std::size_t m_inputDimension;

	// oob error for the forest
	double m_OOBerror = 0.;

	// feature importances for the forest
	RealVector m_featureImportances;

};


}
#endif

