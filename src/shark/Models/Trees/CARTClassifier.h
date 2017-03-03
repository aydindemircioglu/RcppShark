// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(BH)]]
//===========================================================================
/*!
 * 
 *
 * \brief       Cart Classifier
 * 
 * 
 *
 * \author      K. N. Hansen, J. Kremer
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

#ifndef SHARK_MODELS_TREES_CARTCLASSIFIER_H
#define SHARK_MODELS_TREES_CARTCLASSIFIER_H


#include <shark/Rng/GlobalRng.h>
#include <Rcpp.h>
#include <shark/Models/AbstractModel.h>
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h>
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>                                  
#include <shark/Data/Dataset.h>

namespace shark {


///
/// \brief CART Classifier.
///
/// \par
/// The CARTClassifier predicts a class label
/// using the CART algorithm.
///
/// \par
/// It is a decision tree algorithm.
///
template<class LabelType>
class CARTClassifier : public AbstractModel<RealVector,LabelType>
{
private:
        typedef AbstractModel<RealVector, LabelType> base_type;
public:
        typedef typename base_type::BatchInputType BatchInputType;
        typedef typename base_type::BatchOutputType BatchOutputType;
//      Information about a single split. misclassProp, r and g are variables used in the cost complexity step
        struct NodeInfo {
                std::size_t nodeId = 0;
                std::size_t attributeIndex = 0;
                double attributeValue = 0.;
                std::size_t leftNodeId = 0;
                std::size_t rightNodeId = 0;
                LabelType label;
                double misclassProp = 0.;//TODO: remove this
                std::size_t r = 0;//TODO: remove this
                double g = 0.;//TODO: remove this

           NodeInfo() {}
                explicit NodeInfo(std::size_t nodeId) : nodeId(nodeId) {}
                NodeInfo(std::size_t nodeId, LabelType label) : nodeId(nodeId), label(std::move(label)) {}
                NodeInfo(NodeInfo const&) = default;
                NodeInfo& operator=(NodeInfo const&) = default;
                NodeInfo(NodeInfo &&n) BOOST_NOEXCEPT_IF(std::is_nothrow_constructible<LabelType>::value)
                                : nodeId{n.nodeId}, attributeIndex{n.attributeIndex},
                                  attributeValue{n.attributeValue}, leftNodeId{n.leftNodeId},
                                  rightNodeId{n.rightNodeId}, label(std::move(n.label)),
                                  misclassProp{n.misclassProp}, r{n.r}, g{n.g}
                {}
                NodeInfo& operator=(NodeInfo &&n) BOOST_NOEXCEPT_IF((std::is_nothrow_assignable<LabelType,LabelType>::value))
                {
                        nodeId = n.nodeId;
                        attributeIndex = n.attributeIndex;
                        attributeValue = n.attributeValue;
                        leftNodeId = n.leftNodeId;
                        rightNodeId = n.rightNodeId;
                        label = std::move(n.label);
                        misclassProp = n.misclassProp;
                        r = n.r;
                        g = n.g;
                        return *this;
                }
        };

        /// Vector of structs that contains the splitting information and the labels.
        /// The class label is a normalized histogram in the classification case.
        /// In the regression case, the label is the regression value.
        typedef std::vector<NodeInfo> TreeType;

        /// Constructor
        CARTClassifier()
        {}

        /// Constructor taking the tree as argument
        explicit CARTClassifier(TreeType const& tree)
                        : m_tree(tree)
        { }
        explicit CARTClassifier(TreeType&& tree)
                        : m_tree(std::move(tree))
        { }

        /// Constructor taking the tree as argument and optimize it if requested
        CARTClassifier(TreeType const& tree, bool optimize)
        {
                if (optimize)
                        setTree(tree);
                else
                        m_tree=tree;
        }

        /// Constructor taking the tree as argument as well as maximum number of attributes
        CARTClassifier(TreeType const& tree, std::size_t d)
                        : m_tree{tree}, m_inputDimension{d}
        {
                optimizeTree(m_tree);
        }

        CARTClassifier(TreeType&& tree, std::size_t d) BOOST_NOEXCEPT_IF((std::is_nothrow_constructible<TreeType,TreeType>::value))
                        : m_tree(std::move(tree)), m_inputDimension{d}
        {
                optimizeTree(m_tree);
        }

        /// \brief From INameable: return the class name.
        std::string name() const
        { return "CARTClassifier"; }

        boost::shared_ptr<State> createState() const{
                return boost::shared_ptr<State>(new EmptyState());
        }

        using base_type::eval;
        /// \brief Evaluate the Tree on a batch of patterns
        void eval(BatchInputType const& patterns, BatchOutputType & outputs) const{
                std::size_t numPatterns = shark::size(patterns);
                //evaluate the first pattern alone and create the batch output from that
                LabelType const& firstResult = evalPattern(row(patterns,0));
                outputs = Batch<LabelType>::createBatch(firstResult,numPatterns);
                get(outputs,0) = firstResult;
                
                //evaluate the rest
                for(std::size_t i = 0; i != numPatterns; ++i){
                        get(outputs,i) = evalPattern(row(patterns,i));
                }
        }
        
        void eval(BatchInputType const& patterns, BatchOutputType & outputs, State& state) const{
                eval(patterns,outputs);
        }
        /// \brief Evaluate the Tree on a single pattern
        void eval(RealVector const& pattern, LabelType& output){
                output = evalPattern(pattern);          
        }

        /// Set the model tree.
        void setTree(TreeType const& tree){
                m_tree = tree;
                optimizeTree(m_tree);
        }
        
        /// Get the model tree.
        TreeType getTree() const {
                return m_tree;
        }

        /// \brief The model does not have any parameters.
        std::size_t numberOfParameters() const{
                return 0;
        }

        /// \brief The model does not have any parameters.
        RealVector parameterVector() const {
                return RealVector();
        }

        /// \brief The model does not have any parameters.
        void setParameterVector(RealVector const& param) {
                SHARK_ASSERT(param.size() == 0);
        }

        /// from ISerializable, reads a model from an archive
        /// from ISerializable, writes a model to an archive
        //Count how often attributes are used
        UIntVector countAttributes() const {
                SHARK_ASSERT(m_inputDimension > 0);
                UIntVector r(m_inputDimension, 0);
                typename TreeType::const_iterator it;
                for(it = m_tree.begin(); it != m_tree.end(); ++it) {
                        //Rcpp::Rcout << "NodeId: " <<it->leftNodeId << std::endl;
                        if(it->leftNodeId != 0) { // not a label 
                                r(it->attributeIndex)++;
                        }
                }
                return r;
        }

        ///Return input dimension
        std::size_t inputSize() const {
                return m_inputDimension;
        }

        //Set input dimension
        void setInputDimension(std::size_t d) {
                m_inputDimension = d;
        }

        /// Compute oob error, given an oob dataset (Classification)
        void computeOOBerror(const ClassificationDataset& dataOOB){
                // define loss
                ZeroOneLoss<unsigned int, RealVector> lossOOB;

                // predict oob data
                Data<RealVector> predOOB = (*this)(dataOOB.inputs());

                // count average number of oob misclassifications
                m_OOBerror = lossOOB.eval(dataOOB.labels(), predOOB);
        }

        /// Compute oob error, given an oob dataset (Regression)
        void computeOOBerror(RegressionDataset const& dataOOB){
                // define loss
                SquaredLoss<RealVector, RealVector> lossOOB;

                // predict oob data
                Data<RealVector> predOOB = (*this)(dataOOB.inputs());

                // Compute mean squared error
                m_OOBerror = lossOOB.eval(dataOOB.labels(), predOOB);
        }

        /// Return OOB error
        double OOBerror() const {
                return m_OOBerror;
        }

        /// Return feature importances
        RealVector const& featureImportances() const {
                return m_featureImportances;
        }

        /// Compute feature importances, given an oob dataset (Classification)
        void computeFeatureImportances(ClassificationDataset const& dataOOB, Rng::rng_type& rng){
                m_featureImportances.resize(m_inputDimension);

                // define loss
                ZeroOneLoss<unsigned int, RealVector> lossOOB;

                // compute oob error
                computeOOBerror(dataOOB);

                // count average number of correct oob predictions
                double accuracyOOB = 1. - m_OOBerror;

                // go through all dimensions, permute each dimension across all elements and train the tree on it
                for(std::size_t i=0;i!=m_inputDimension;++i) {
                        // create permuted dataset by copying
                        ClassificationDataset pDataOOB(dataOOB);
                        pDataOOB.makeIndependent();

                        // permute current dimension
                        RealVector v = getColumn(pDataOOB.inputs(), i);
                        std::random_shuffle(v.begin(), v.end(), DiscreteUniform<>{rng});
                        setColumn(pDataOOB.inputs(), i, v);

                        // evaluate the data set for which one feature dimension was permuted with this tree
                        Data<RealVector> pPredOOB = (*this)(pDataOOB.inputs());

                        // count the number of correct predictions
                        double accuracyPermutedOOB = 1. - lossOOB.eval(pDataOOB.labels(),pPredOOB);
                        
                        // store importance
                        m_featureImportances[i] = std::fabs(accuracyOOB - accuracyPermutedOOB);
                }
        }

        /// Compute feature importances, given an oob dataset (Regression)
        void computeFeatureImportances(RegressionDataset const& dataOOB, Rng::rng_type& rng){
                m_featureImportances.resize(m_inputDimension);

                // define loss
                SquaredLoss<RealVector, RealVector> lossOOB;

                // compute oob error
                computeOOBerror(dataOOB);

                // mean squared error for oob sample
                double mseOOB = m_OOBerror;

                // go through all dimensions, permute each dimension across all elements and train the tree on it
                for(std::size_t i=0;i!=m_inputDimension;++i) {
                        // create permuted dataset by copying
                        RegressionDataset pDataOOB(dataOOB);
                        pDataOOB.makeIndependent();

                        // permute current dimension
                        RealVector v = getColumn(pDataOOB.inputs(), i);
                        std::random_shuffle(v.begin(), v.end(), DiscreteUniform<>{rng});
                        setColumn(pDataOOB.inputs(), i, v);

                        // evaluate the data set for which one feature dimension was permuted with this tree
                        Data<RealVector> pPredOOB = (*this)(pDataOOB.inputs());

                        // mean squared error of permuted oob sample
                        double msePermutedOOB = lossOOB.eval(pDataOOB.labels(),pPredOOB);
                        
                        // store importance
                        m_featureImportances[i] = std::fabs(msePermutedOOB - mseOOB);
                }
        }

protected:
        /// tree of the model
        TreeType m_tree;
        
        /// \brief Finds the index of the node with a certain nodeID in an unoptimized tree.
        std::size_t findNode(std::size_t nodeId) const{
                std::size_t index = 0;
                for(; nodeId != m_tree[index].nodeId; ++index);
                return index;
        }

        /// Optimize a tree, so constant lookup can be used.
        /// The optimization is done by changing the index of the children
        /// to use indices instead of node ID.
        /// Furthermore, the node IDs are converted to index numbers.
        void optimizeTree(TreeType & tree) const{
                for(std::size_t i = 0; i < tree.size(); i++){
                        tree[i].leftNodeId = findNode(tree[i].leftNodeId);
                        tree[i].rightNodeId = findNode(tree[i].rightNodeId);
                }
                for(std::size_t i = 0; i < tree.size(); i++){
                        tree[i].nodeId = i;
                }
        }
        
        /// Evaluate the CART tree on a single sample
        template<class Vector>
        LabelType const& evalPattern(Vector const& pattern) const{
                std::size_t nodeId = 0;
                while(m_tree[nodeId].leftNodeId != 0){
                        if(pattern[m_tree[nodeId].attributeIndex]<= m_tree[nodeId].attributeValue){
                                //Branch on left node
                                nodeId = m_tree[nodeId].leftNodeId;
                        }else{
                                //Branch on right node
                                nodeId = m_tree[nodeId].rightNodeId;
                        }
                }
                return m_tree[nodeId].label;
        }


        ///Number of attributes (set by trainer)
        std::size_t m_inputDimension = 0;

        // feature importances
        RealVector m_featureImportances;

        // oob error
        double m_OOBerror = 0.;
};


}
#endif

