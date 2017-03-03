//===========================================================================
/*!
 * 
 *
 * \brief       CART
 * 
 * 
 *
 * \author      K. N. Hansen
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


#ifndef SHARK_ALGORITHMS_TRAINERS_CARTTRAINER_H
#define SHARK_ALGORITHMS_TRAINERS_CARTTRAINER_H

#include <shark/Core/DLLSupport.h>
#include <shark/Models/Trees/CARTClassifier.h>
#include <shark/Algorithms/Trainers/CARTcommon.h>
#include <shark/Algorithms/Trainers/AbstractTrainer.h>
#include <unordered_map>

namespace shark {
/*!
 * \brief Classification And Regression Trees CART
 *
 * CART is a decision tree algorithm, that builds a binary decision tree
 * The decision tree is built by partitioning a dataset recursively
 *
 * The partitioning is done, so that the partition chosen at a single
 * node, is the partition the produces the largest decrease in node
 * impurity.
 *
 * The node impurity is measured by the Gini criteria in the classification
 * case, and the total sum of squares error in the regression case
 *
 * The tree is grown, until all leafs are pure. A node is considered pure
 * when it only consist of identical cases in the classification case
 * and identical or single values in the regression case
 *
 * After the maximum sized tree is grown, the tree is pruned back from the leafs upward.
 * The pruning is done by cost complexity pruning, as described by L. Breiman
 *
 * The algorithm used is based on the SPRINT algorithm, as shown by J. Shafer et al.
 *
 * For more detailed information about CART, see \e Classification \e And \e Regression
 * \e Trees written by L. Breiman et al. 1984.
 */
class CARTTrainer 
: public AbstractTrainer<CARTClassifier<RealVector>, unsigned int>
, public AbstractTrainer<CARTClassifier<RealVector>, RealVector >
{
public:
	typedef CARTClassifier<RealVector> ModelType;

	/// Constructor
	CARTTrainer(){
		m_nodeSize = 1;
		m_numberOfFolds = 10;
	}

	/// \brief From INameable: return the class name.
	std::string name() const
	{ return "CARTTrainer"; }

	///Train classification
	SHARK_EXPORT_SYMBOL void train(ModelType& model, ClassificationDataset const& dataset);
	
	///Train regression
	SHARK_EXPORT_SYMBOL void train(ModelType& model, RegressionDataset const& dataset);

	///Sets the number of folds used for creation of the trees.
	void setNumberOfFolds(unsigned int folds){
		m_numberOfFolds = folds;
	}
protected:
	using Split = detail::cart::Split;

	///Types frequently used
	struct TableEntry{
		double value;
		std::size_t id;

		bool operator<( TableEntry const& v2)const {
			return value < v2.value;
		}
	};
	typedef std::vector < TableEntry > AttributeTable;
	typedef std::vector < AttributeTable > AttributeTables;

	typedef ModelType::TreeType TreeType;

	// LabelVector
	using ClassVector = UIntVector;
	using LabelVector = std::vector<RealVector>;

	///Number of attributes in the dataset
	std::size_t m_inputDimension;

	///Dimension of a label. Used in Regression
	std::size_t m_labelDimension;
	///Holds the number of distinct labels. Used in Classification
	std::size_t m_labelCardinality;

	///Controls the number of samples in the terminal nodes
	std::size_t m_nodeSize;

	///Number of folds used to create the tree.
	unsigned int m_numberOfFolds;

	//Classification functions
	///Builds a single decision tree from a classification dataset
	///The method requires the attribute tables,
	SHARK_EXPORT_SYMBOL TreeType buildTree(detail::cart::SortedIndex&& tables, ClassificationDataset const& dataset, ClassVector& cFull, std::size_t nodeId );

	///Regression functions
	SHARK_EXPORT_SYMBOL TreeType buildTree(detail::cart::SortedIndex&& tables, RegressionDataset const& dataset, RealVector const& sumFull, std::size_t nodeId, std::size_t trainSize);
	///Calculates the total sum of squares
	SHARK_EXPORT_SYMBOL double totalSumOfSquares(std::vector<RealVector> const& labels, std::size_t start, std::size_t length, RealVector const& sumLabel);

	///Pruning
	///Prunes decision tree
	SHARK_EXPORT_SYMBOL void pruneTree(TreeType & tree);
	///Prunes a single node, including the child nodes of the decision tree
	SHARK_EXPORT_SYMBOL void pruneNode(TreeType & tree, std::size_t nodeId);
	///Updates the node variables used in the cost complexity pruning stage
	SHARK_EXPORT_SYMBOL void measureStrength(TreeType & tree, std::size_t nodeId, std::size_t parentNodeId);

	///Returns the index of the node with node id in tree.
	SHARK_EXPORT_SYMBOL std::size_t findNode(TreeType & tree, std::size_t nodeId);

	SHARK_EXPORT_SYMBOL Split findSplit(detail::cart::SortedIndex const& tables, ClassificationDataset const& dataset, ClassVector const& cFull) const;
	SHARK_EXPORT_SYMBOL Split findSplit(detail::cart::SortedIndex const& tables, RegressionDataset const& dataset, RealVector const& sumFull) const;

};


}
#endif


