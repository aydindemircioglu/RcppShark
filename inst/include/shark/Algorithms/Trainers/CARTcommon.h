//===========================================================================
/*!
 *
 *
 * \brief       General functions for Tree modeling.
 *
 *
 *
 * \author      K. N. Hansen, J. Kremer, J. Wrigley
 * \date        2011-2016
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
#ifndef SHARK_ALGORITHMS_TRAINERS_CARTCOMMON_H
#define SHARK_ALGORITHMS_TRAINERS_CARTCOMMON_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Models/Trees/General.h>
#include <shark/Data/DataView.h>
#include <shark/Models/Trees/CARTClassifier.h>
#include <vector>
#include <utility>
namespace shark {
namespace detail {
namespace cart {
/// ClassVector
using ClassVector = UIntVector;

class Split{
	using NodeInfo = CARTClassifier<RealVector>::NodeInfo;
public:
	std::size_t splitAttribute = 0, splitRow = 0;
	double splitValue=0;

	//static constexpr
	double WORST_IMPURITY = std::numeric_limits<double>::max();
	double impurity = WORST_IMPURITY;
	double purity = 0;
	RealVector sumAbove, sumBelow; // for regression
	ClassVector cAbove, cBelow;    // for classification
	inline friend NodeInfo& operator<<=(NodeInfo& node, Split const& split){
		node.attributeIndex = split.splitAttribute;
		node.attributeValue = split.splitValue;
		return node;
	}
	inline operator bool(){
		return impurity < WORST_IMPURITY || purity > 0;
	}
};


struct Attribute {
	double value;
	std::size_t id;

	inline bool operator<(Attribute const &v) const {
		return value < v.value;
	}
};

/**
 * An Index of a dataset providing fast sorted-order lookup of each attribute.
 */
class SortedIndex {
/// attribute table
	using AttributeTable = std::vector<Attribute>;
/// collection of attribute tables
	using AttributeTables = std::vector<AttributeTable>;
	using bit_vector = std::vector<bool>;

	std::size_t const m_noElements, m_totalElements, m_noInputDimensions;
	AttributeTables m_tables;
	bit_vector m_hash;

	explicit SortedIndex(AttributeTables &&tables, bit_vector &&hash)
			: m_noElements(tables[0].size()),
			  m_totalElements{hash.size()},
			  m_noInputDimensions(tables.size()),
			  m_tables(std::move(tables)),
			  m_hash(std::move(hash))
	{}
public:
/** Creates an index of the dataset
 *  A dataset consisting of m input variables has m sorted attribute tables.
 *  [attribute | rid ]
 */
	template<class Dataset>
	explicit SortedIndex(DataView<Dataset const> const &elements)
			: m_noElements{elements.size()},
			  m_totalElements{m_noElements},
			  m_noInputDimensions{elements[0].input.size()},
			  m_tables(m_noInputDimensions),
	          m_hash(bit_vector(m_noElements))
	{
		std::size_t n_elements = m_noElements;
		//Each entry in the outer vector is an attribute table
		//For each column
		for (std::size_t j = 0; j < m_noInputDimensions; j++) {
			auto &table = m_tables[j];
			table.reserve(n_elements);

			//For each row, store Attribute value, class and rowId
			for (std::size_t i = 0; i < n_elements; i++) {
				table.push_back(Attribute{elements[i].input[j], i});
			}
			std::sort(table.begin(), table.end());
		}
	}
	template<class Dataset>
	explicit SortedIndex(Dataset const &elements)
			: m_noElements{elements.numberOfElements()},
			  m_totalElements{m_noElements},
			  m_noInputDimensions{inputDimension(elements)},
			  m_tables(m_noInputDimensions),
			  m_hash(bit_vector(m_noElements))
	{
		std::size_t n_elements = m_noElements;
		//Each entry in the outer vector is an attribute table
		//For each column
		for (std::size_t j = 0; j < m_noInputDimensions; j++) {
			auto &table = m_tables[j];
			table.reserve(n_elements);

			//For each row, store Attribute value, class and rowId
			std::size_t i = 0;
			for(auto const& element: elements.elements()){
				table.push_back(Attribute{element.input[j], i++});
			}
			std::sort(table.begin(), table.end());
		}
	}

/**
 * Returns two Indices: left and right
 * Calculated from splitting tables at (index, valIndex)
 */
	std::pair<SortedIndex, SortedIndex> split(std::size_t index, std::size_t valIndex) {
		//Build a hash table for fast lookup
		for(std::size_t i = 0, s = m_tables[index].size(); i<s; ++i) {
			m_hash[m_tables[index][i].id] = (i<=valIndex);
		}

		AttributeTables RAttributeTables, LAttributeTables;
		for(auto && table : m_tables) {
			auto begin = table.begin(), end = table.end();
			auto middle = std::stable_partition(begin,end,[this](Attribute const& entry){
				return m_hash[entry.id];
			});
			RAttributeTables.emplace_back(AttributeTable{middle,end});
			table.resize(std::distance(begin,middle));
			LAttributeTables.emplace_back(std::move(table));
		}
		m_tables.clear(); m_tables.shrink_to_fit();
		if(valIndex > m_noElements/2)
			return std::make_pair(SortedIndex{std::move(LAttributeTables), std::move(m_hash)},
			                      SortedIndex(std::move(RAttributeTables), bit_vector(m_totalElements)));
		return std::make_pair(SortedIndex{std::move(LAttributeTables), bit_vector(m_totalElements)},
		                      SortedIndex{std::move(RAttributeTables), std::move(m_hash)});
	}

	std::size_t noTables() const { return m_noInputDimensions; }
	std::size_t noRows() const { return m_noElements; }
	std::size_t size() const { return noTables(); }
	inline AttributeTable const& operator[](std::size_t i) const {
		return m_tables[i];
	}
};

/// Generate a histogram from the count vector.
inline RealVector hist(ClassVector const& countVector) {
	return countVector/double(sum(countVector));
}

using ImpurityMeasureFn = double (*)(ClassVector const& countVector, std::size_t n);

enum class ImpurityMeasure {
	gini, misclassification, crossEntropy
};

/// Calculate the Gini impurity of the countVector
double gini(ClassVector const& countVector, std::size_t n);
double misclassificationError(ClassVector const& countVector, std::size_t n);
double crossEntropy(ClassVector const& countVector, std::size_t n);

ImpurityMeasureFn setImpurityFn(ImpurityMeasure im);

/// Create a count vector as used in the classification case.
ClassVector createCountVector(DataView<ClassificationDataset const> const& elements, std::size_t labelCardinality);
ClassVector createCountVector(ClassificationDataset const& dataset, std::size_t labelCardinality);

template<class DatasetType>
class Bag {
	DataView<DatasetType const> m_oobDataView; // out-of-bag
	DataView<DatasetType const> m_dataView; // in-bag
public:
	using InputType = typename DatasetType::InputType;
	using LabelType = typename DatasetType::LabelType;
	using InputContainer = typename DatasetType::InputContainer;
	using IndexSet = typename DatasetType::IndexSet;
	DataView<DatasetType const> const& data;
	std::size_t const bagSize;
	std::vector<std::size_t> counts;
	std::vector<size_t> oobIndices, ibIndices; // out-of-bag, in-bag
	static bool const withReplacement = true;
	static bool const withoutReplacement = false;
	explicit Bag<DatasetType>(DatasetType const& dataset)
			: data(dataset), bagSize(data.size()),
			  counts(bagSize)
	{}
	Bag<DatasetType>(DatasetType const& dataset, std::size_t bagSize)
			: data(dataset), bagSize(bagSize), counts(data.size())
	{}
	Bag<DatasetType>(DataView<DatasetType const> const& view)
			: data(view), bagSize(view.size()), counts(view.size())
	{}
	Bag<DatasetType>(DataView<DatasetType const> const& view, std::size_t bagSize)
			: data(view), bagSize(bagSize), counts(view.size())
	{}
	void splitBag(){
		for(std::size_t i=0, s = data.size(); i<s; i++){
			if(counts[i]) ibIndices.push_back(i);
			else oobIndices.push_back(i);
		}
	}
	DataView<DatasetType const> const& dataView(){
		if(!ibIndices.size()) splitBag();
		if(!m_dataView.size()) m_dataView = subset(data,ibIndices);
		return m_dataView;
	}
	DataView<DatasetType const> const& oobDataView(){
		if(!oobIndices.size()) splitBag();
		if(!m_oobDataView.size()) m_oobDataView = subset(data,oobIndices);
		return m_oobDataView;
	}
};

template<class DatasetType>
Bag<DatasetType> bootstrap(DataView<DatasetType const> const& view, Rng::rng_type rng, std::size_t bagSize = 0, bool withReplacement = Bag<DatasetType>::withoutReplacement) {
	if(withReplacement) {
		if (!bagSize) bagSize = view.size();
		DiscreteUniform<> discrete(rng, 0, bagSize - 1);
		Bag<DatasetType> bag(view, bagSize);
		for (std::size_t i = 0; i < bagSize; i++) bag.counts[discrete()]++;
		bag.splitBag();
		return bag;
	}
	if (!bagSize) bagSize = view.size();
	Bag<DatasetType> bag(view, bagSize);
	std::vector<std::size_t> subsetIndices(view.size());
	std::iota(subsetIndices.begin(), subsetIndices.end(), 0);
	std::random_shuffle(subsetIndices.begin(), subsetIndices.end(), DiscreteUniform<>{rng});
	for (std::size_t i = 0; i < bagSize; i++) bag.counts[subsetIndices[i]]++;
	bag.splitBag();
	return bag;
}
}}} // namespace shark::detail::cart

#endif //SHARK_ALGORITHMS_TRAINERS_CARTCOMMON_H

