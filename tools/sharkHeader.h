#include <shark/Algorithms/AbstractMultiObjectiveOptimizer.h>
#include <shark/Algorithms/AbstractOptimizer.h>
#include <shark/Algorithms/AbstractSingleObjectiveOptimizer.h>
#include <shark/Algorithms/DirectSearch/AGE2.h>
#include <shark/Algorithms/DirectSearch/CMA/Chromosome.h>
#include <shark/Algorithms/DirectSearch/CMA/CMAIndividual.h>
#include <shark/Algorithms/DirectSearch/CMA.h>
#include <shark/Algorithms/DirectSearch/ElitistCMA.h>
#include <shark/Algorithms/DirectSearch/Experiments/Experiment.h>
#include <shark/Algorithms/DirectSearch/FastNonDominatedSort.h>
#include <shark/Algorithms/DirectSearch/FitnessExtractor.h>
#include <shark/Algorithms/DirectSearch/GridSearch.h>
#include <shark/Algorithms/DirectSearch/HypervolumeApproximator.h>
#include <shark/Algorithms/DirectSearch/HypervolumeCalculator.h>
#include <shark/Algorithms/DirectSearch/Indicators/AdditiveEpsilonIndicator.h>
#include <shark/Algorithms/DirectSearch/Indicators/HypervolumeIndicator.h>
#include <shark/Algorithms/DirectSearch/Individual.h>
#include <shark/Algorithms/DirectSearch/InterruptibleAlgorithmRunner.h>
#include <shark/Algorithms/DirectSearch/MOCMA.h>
#include <shark/Algorithms/DirectSearch/Operators/Evaluation/PenalizingEvaluator.h>
#include <shark/Algorithms/DirectSearch/Operators/Mutation/PolynomialMutation.h>
#include <shark/Algorithms/DirectSearch/Operators/PopulationBasedStepSizeAdaptation.h>
#include <shark/Algorithms/DirectSearch/Operators/Recombination/SimulatedBinaryCrossover.h>
#include <shark/Algorithms/DirectSearch/Operators/Recombination/UniformCrossover.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/ElitistSelection.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/IndicatorBasedSelection.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/RouletteWheelSelection.h>
#include <shark/Algorithms/DirectSearch/Operators/Selection/TournamentSelection.h>
#include <shark/Algorithms/DirectSearch/ParetoDominanceComparator.h>
#include <shark/Algorithms/DirectSearch/SteadyStateMOCMA.h>
#include <shark/Algorithms/GradientDescent/AbstractLineSearchOptimizer.h>
#include <shark/Algorithms/GradientDescent/CG.h>
#include <shark/Algorithms/GradientDescent/LBFGS.h>// the L-BFGS optimization algorithm
#include <shark/Algorithms/GradientDescent/LineSearch.h>
#include <shark/Algorithms/GradientDescent/Rprop.h>
#include <shark/Algorithms/GradientDescent/SteepestDescent.h>
#include <shark/Algorithms/JaakkolaHeuristic.h>
#include <shark/Algorithms/KMeans.h>
#include <shark/Algorithms/NearestNeighbors/AbstractNearestNeighbors.h>
#include <shark/Algorithms/NearestNeighbors/SimpleNearestNeighbors.h>
#include <shark/Algorithms/NearestNeighbors/TreeNearestNeighbors.h>
#include <shark/Algorithms/QP/BoxConstrainedProblems.h>
#include <shark/Algorithms/QP/Impl/AnalyticProblems.h>
#include <shark/Algorithms/QP/QpBoxLinear.h>
#include <shark/Algorithms/QP/QpMcBoxDecomp.h>
#include <shark/Algorithms/QP/QpMcDecomp.h>
#include <shark/Algorithms/QP/QpMcLinear.h>
#include <shark/Algorithms/QP/QpMcSimplexDecomp.h>
#include <shark/Algorithms/QP/QpSolver.h>
#include <shark/Algorithms/QP/QpSparseArray.h>
#include <shark/Algorithms/QP/QuadraticProgram.h>
#include <shark/Algorithms/QP/SvmProblems.h>
#include <shark/Algorithms/StoppingCriteria/AbstractStoppingCriterion.h>
#include <shark/Algorithms/StoppingCriteria/GeneralizationQuotient.h> //Uses the validation error to track the progress
#include <shark/Algorithms/StoppingCriteria/MaxIterations.h>
#include <shark/Algorithms/StoppingCriteria/TrainingError.h> //Stops when the algorithm seems to converge
#include <shark/Algorithms/StoppingCriteria/ValidatedStoppingCriterion.h> //Adds the validation error to the value of the point
#include <shark/Algorithms/Trainers/AbstractSvmTrainer.h>
#include <shark/Algorithms/Trainers/AbstractTrainer.h>
#include <shark/Algorithms/Trainers/AbstractWeightedTrainer.h>
#include <shark/Algorithms/Trainers/Budgeted/AbstractBudgetMaintenanceStrategy.h>
#include <shark/Algorithms/Trainers/Budgeted/KernelBudgetedSGDTrainer.h> // the KernelBudgetedSGD trainer
#include <shark/Algorithms/Trainers/Budgeted/MergeBudgetMaintenanceStrategy.h> // the strategy the trainer will use 
#include <shark/Algorithms/Trainers/CARTTrainer.h>     // the CART trainer
#include <shark/Algorithms/Trainers/CSvmTrainer.h>
#include <shark/Algorithms/Trainers/EpsilonSvmTrainer.h>
#include <shark/Algorithms/Trainers/KernelSGDTrainer.h>
#include <shark/Algorithms/Trainers/LassoRegression.h>
#include <shark/Algorithms/Trainers/LDA.h>
#include <shark/Algorithms/Trainers/LinearRegression.h>
#include <shark/Algorithms/Trainers/McReinforcedSvmTrainer.h>
#include <shark/Algorithms/Trainers/McSvmADMTrainer.h>
#include <shark/Algorithms/Trainers/McSvmATMTrainer.h>
#include <shark/Algorithms/Trainers/McSvmATSTrainer.h>
#include <shark/Algorithms/Trainers/McSvmCSTrainer.h>
#include <shark/Algorithms/Trainers/McSvmLLWTrainer.h>
#include <shark/Algorithms/Trainers/McSvmMMRTrainer.h>
#include <shark/Algorithms/Trainers/McSvmOVATrainer.h>
#include <shark/Algorithms/Trainers/McSvmWWTrainer.h>
#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h>
#include <shark/Algorithms/Trainers/NormalizeComponentsWhitening.h>
#include <shark/Algorithms/Trainers/NormalizeKernelUnitVariance.h>
#include <shark/Algorithms/Trainers/OneClassSvmTrainer.h>
#include <shark/Algorithms/Trainers/OptimizationTrainer.h>
#include <shark/Algorithms/Trainers/PCA.h>
#include <shark/Algorithms/Trainers/RegularizationNetworkTrainer.h>
#include <shark/Algorithms/Trainers/RFTrainer.h>
#include <shark/Algorithms/Trainers/SigmoidFit.h>
#include <shark/Core/DLLSupport.h>
#include <shark/Core/Exception.h>
#include <shark/Core/Flags.h>
#include <shark/Core/INameable.h>
#include <shark/Core/IParameterizable.h>
#include <shark/Core/ISerializable.h>
#include <shark/Core/Math.h>
#include <shark/Core/OpenMP.h>
#include <shark/Core/ResultSets.h>
#include <shark/Core/State.h>
#include <shark/Core/Timer.h>
#include <shark/Core/Traits/ProxyReferenceTraits.h>
#include <shark/Core/utility/CanBeCalled.h>
#include <shark/Core/utility/functional.h>
#include <shark/Core/utility/Iterators.h>
#include <shark/Core/utility/KeyValuePair.h>
#include <shark/Core/utility/Range.h>
#include <shark/Core/utility/ZipPair.h>
#include <shark/Data/BatchInterfaceAdaptStruct.h>
#include <shark/Data/BatchInterface.h>
#include <shark/Data/Csv.h>
#include <shark/Data/CVDatasetTools.h>
#include <shark/Data/DataDistribution.h>
#include <shark/Data/Dataset.h>
#include <shark/Data/DataView.h>
#include <shark/Data/Pgm.h>
#include <shark/Data/SparseData.h>
#include <shark/Data/Statistics.h>
#include <shark/Data/WeightedDataset.h>
#include <shark/LinAlg/Base.h>
#include <shark/LinAlg/BLAS/blas.h>
#include <shark/LinAlg/BLAS/Initialize.h>
#include <shark/LinAlg/BLAS/io.hpp>
#include <shark/LinAlg/BLAS/kernels/atlas/potrf.hpp>
#include <shark/LinAlg/BLAS/kernels/syev.hpp>
#include <shark/LinAlg/BLAS/kernels/trsm.hpp>
#include <shark/LinAlg/BLAS/kernels/trsv.hpp>
#include <shark/LinAlg/BLAS/lu.hpp>
#include <shark/LinAlg/BLAS/matrix_expression.hpp>
#include <shark/LinAlg/BLAS/matrix.hpp>
#include <shark/LinAlg/BLAS/matrix_proxy.hpp>
#include <shark/LinAlg/BLAS/matrix_set.hpp>
#include <shark/LinAlg/BLAS/matrix_sparse.hpp>
#include <shark/LinAlg/BLAS/operation.hpp>
#include <shark/LinAlg/BLAS/permutation.hpp>
#include <shark/LinAlg/BLAS/traits/matrix_raw.hpp>
#include <shark/LinAlg/BLAS/traits/vector_raw.hpp>
#include <shark/LinAlg/BLAS/vector_expression.hpp>
#include <shark/LinAlg/BLAS/vector.hpp>
#include <shark/LinAlg/BLAS/vector_proxy.hpp>
#include <shark/LinAlg/BLAS/vector_sparse.hpp>
#include <shark/LinAlg/BlockMatrix2x2.h>
#include <shark/LinAlg/CachedMatrix.h>
#include <shark/LinAlg/Cholesky.h>
#include <shark/LinAlg/eigenvalues.h>
#include <shark/LinAlg/ExampleModifiedKernelMatrix.h>
#include <shark/LinAlg/GaussianKernelMatrix.h>
#include <shark/LinAlg/Initialize.h>
#include <shark/LinAlg/KernelMatrix.h>
#include <shark/LinAlg/LRUCache.h>
#include <shark/LinAlg/Metrics.h>
#include <shark/LinAlg/PartlyPrecomputedMatrix.h>
#include <shark/LinAlg/PrecomputedMatrix.h>
#include <shark/LinAlg/RegularizedKernelMatrix.h>
#include <shark/LinAlg/rotations.h>
#include <shark/LinAlg/solveSystem.h>
#include <shark/LinAlg/solveTriangular.h>
#include <shark/LinAlg/Tools.h>
#include <shark/Models/AbstractModel.h>
#include <shark/Models/Autoencoder.h>
#include <shark/Models/Clustering/AbstractClustering.h>
#include <shark/Models/Clustering/Centroids.h>
#include <shark/Models/Clustering/ClusteringModel.h>
#include <shark/Models/Clustering/HardClusteringModel.h>
#include <shark/Models/Clustering/HierarchicalClustering.h>
#include <shark/Models/ConcatenatedModel.h>
#include <shark/Models/Converter.h>
#include <shark/Models/FFNet.h>
#include <shark/Models/ImpulseNoiseModel.h>// model adding noise to the inputs
#include <shark/Models/Kernels/AbstractKernelFunction.h>
#include <shark/Models/Kernels/AbstractMetric.h>
#include <shark/Models/Kernels/ArdKernel.h>
#include <shark/Models/Kernels/CSvmDerivative.h>
#include <shark/Models/Kernels/DiscreteKernel.h>
#include <shark/Models/Kernels/EvalSkipMissingFeatures.h>
#include <shark/Models/Kernels/GaussianRbfKernel.h>
#include <shark/Models/Kernels/GaussianRbfKernel.h> //the used kernel for the SVM
#include <shark/Models/Kernels/KernelExpansion.h>
#include <shark/Models/Kernels/KernelHelpers.h>
#include <shark/Models/Kernels/LinearKernel.h>
#include <shark/Models/Kernels/MklKernel.h>
#include <shark/Models/Kernels/MonomialKernel.h>
#include <shark/Models/Kernels/MultiTaskKernel.h>
#include <shark/Models/Kernels/PolynomialKernel.h>
#include <shark/Models/Kernels/ProductKernel.h>
#include <shark/Models/Kernels/ScaledKernel.h>
#include <shark/Models/Kernels/SubrangeKernel.h>
#include <shark/Models/Kernels/WeightedSumKernel.h>
#include <shark/Models/LinearClassifier.h>
#include <shark/Models/LinearModel.h>
#include <shark/Models/MeanModel.h>
#include <shark/Models/NearestNeighborClassifier.h>
#include <shark/Models/Neurons.h>
#include <shark/Models/Normalizer.h>
#include <shark/Models/OneVersusOneClassifier.h>
#include <shark/Models/RBFLayer.h>
#include <shark/Models/RecurrentStructure.h>
#include <shark/Models/SigmoidModel.h>
#include <shark/Models/TiedAutoencoder.h>
#include <shark/Models/TiedAutoencoder.h>//autoencoder with tied weights
#include <shark/Models/Trees/BinaryTree.h>
#include <shark/Models/Trees/CARTClassifier.h>
#include <shark/Models/Trees/KDTree.h>
#include <shark/Models/Trees/KHCTree.h>
#include <shark/Models/Trees/LCTree.h>
#include <shark/Models/Trees/RFClassifier.h>
#include <shark/ObjectiveFunctions/AbstractConstraintHandler.h>
#include <shark/ObjectiveFunctions/AbstractCost.h>
#include <shark/ObjectiveFunctions/AbstractObjectiveFunction.h>
#include <shark/ObjectiveFunctions/Benchmarks/Ackley.h>
#include <shark/ObjectiveFunctions/Benchmarks/Benchmarks.h>
#include <shark/ObjectiveFunctions/Benchmarks/CigarDiscus.h>
#include <shark/ObjectiveFunctions/Benchmarks/Cigar.h>
#include <shark/ObjectiveFunctions/Benchmarks/CIGTAB1.h>
#include <shark/ObjectiveFunctions/Benchmarks/CIGTAB2.h>
#include <shark/ObjectiveFunctions/Benchmarks/DiffPowers.h>
#include <shark/ObjectiveFunctions/Benchmarks/Discus.h>
#include <shark/ObjectiveFunctions/Benchmarks/Discus.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ1.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ2.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ3.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ4.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ5.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ6.h>
#include <shark/ObjectiveFunctions/Benchmarks/DTLZ7.h>
#include <shark/ObjectiveFunctions/Benchmarks/ELLI1.h>
#include <shark/ObjectiveFunctions/Benchmarks/ELLI2.h>
#include <shark/ObjectiveFunctions/Benchmarks/Ellipsoid.h>
#include <shark/ObjectiveFunctions/Benchmarks/Fonseca.h>
#include <shark/ObjectiveFunctions/Benchmarks/GSP.h>
#include <shark/ObjectiveFunctions/Benchmarks/Himmelblau.h>
#include <shark/ObjectiveFunctions/Benchmarks/IHR1.h>
#include <shark/ObjectiveFunctions/Benchmarks/IHR2.h>
#include <shark/ObjectiveFunctions/Benchmarks/IHR3.h>
#include <shark/ObjectiveFunctions/Benchmarks/IHR4.h>
#include <shark/ObjectiveFunctions/Benchmarks/IHR6.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ1.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ2.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ3.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ4.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ5.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ6.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ7.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ8.h>
#include <shark/ObjectiveFunctions/Benchmarks/LZ9.h>
#include <shark/ObjectiveFunctions/Benchmarks/Rosenbrock.h>
#include <shark/ObjectiveFunctions/Benchmarks/Sphere.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT1.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT2.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT3.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT3.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT4.h>
#include <shark/ObjectiveFunctions/Benchmarks/ZDT6.h>
#include <shark/ObjectiveFunctions/BoxConstraintHandler.h>
#include <shark/ObjectiveFunctions/CombinedObjectiveFunction.h>
#include <shark/ObjectiveFunctions/CrossValidationError.h>
#include <shark/ObjectiveFunctions/ErrorFunction.h>
#include <shark/ObjectiveFunctions/EvaluationArchive.h>
#include <shark/ObjectiveFunctions/KernelBasisDistance.h>
#include <shark/ObjectiveFunctions/KernelTargetAlignment.h>
#include <shark/ObjectiveFunctions/Loss/AbstractLoss.h>
#include <shark/ObjectiveFunctions/Loss/CrossEntropy.h>
#include <shark/ObjectiveFunctions/Loss/HingeLoss.h> // the loss we want to use for the SGD machine
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h>
#include <shark/ObjectiveFunctions/RadiusMarginQuotient.h>
#include <shark/ObjectiveFunctions/Regularizer.h>
#include <shark/ObjectiveFunctions/SparseAutoencoderError.h>//the error function performing the regularisation of the hidden neurons
#include <shark/ObjectiveFunctions/SvmLogisticInterpretation.h>
#include <shark/Rng/Bernoulli.h>
#include <shark/Rng/Binomial.h>
#include <shark/Rng/Cauchy.h>
#include <shark/Rng/DiffGeometric.h>
#include <shark/Rng/Dirichlet.h>
#include <shark/Rng/DiscreteUniform.h>
#include <shark/Rng/Entropy.h>
#include <shark/Rng/Erlang.h>
#include <shark/Rng/Gamma.h>
#include <shark/Rng/Geometric.h>
#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/HyperGeometric.h>
#include <shark/Rng/KullbackLeiberDivergence.h>
#include <shark/Rng/LogNormal.h>
#include <shark/Rng/NegExponential.h>
#include <shark/Rng/Normal.h>
#include <shark/Rng/Poisson.h>
#include <shark/Rng/Rng.h>
#include <shark/Rng/TruncatedExponential.h>
#include <shark/Rng/Uniform.h>
#include <shark/Rng/Weibull.h>
#include <shark/Statistics/Distributions/MultiVariateNormalDistribution.h>
#include <shark/Statistics/Statistics.h>
#include <shark/Unsupervised/RBM/analytics.h>
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include <shark/Unsupervised/RBM/ConvolutionalRBM.h>
#include <shark/Unsupervised/RBM/Energy.h>
#include <shark/Unsupervised/RBM/GradientApproximations/ContrastiveDivergence.h>
#include <shark/Unsupervised/RBM/GradientApproximations/MultiChainApproximator.h>
#include <shark/Unsupervised/RBM/GradientApproximations/SingleChainApproximator.h>
#include <shark/Unsupervised/RBM/Impl/AverageEnergyGradient.h>
#include <shark/Unsupervised/RBM/Impl/ConvolutionalEnergyGradient.h>
#include <shark/Unsupervised/RBM/Neuronlayers/BinaryLayer.h>
#include <shark/Unsupervised/RBM/Neuronlayers/BipolarLayer.h>
#include <shark/Unsupervised/RBM/Neuronlayers/GaussianLayer.h>
#include <shark/Unsupervised/RBM/Neuronlayers/TruncatedExponentialLayer.h>
#include <shark/Unsupervised/RBM/Problems/BarsAndStripes.h>
#include <shark/Unsupervised/RBM/RBM.h>
#include <shark/Unsupervised/RBM/Sampling/EnergyStoringTemperedMarkovChain.h>
#include <shark/Unsupervised/RBM/Sampling/GibbsOperator.h>
#include <shark/Unsupervised/RBM/Sampling/MarkovChain.h>
#include <shark/Unsupervised/RBM/Sampling/TemperedMarkovChain.h>
#include <shark/Unsupervised/RBM/StateSpaces/RealSpace.h>
#include <shark/Unsupervised/RBM/StateSpaces/TwoStateSpace.h>
#include <shark/Unsupervised/RBM/Tags.h>
