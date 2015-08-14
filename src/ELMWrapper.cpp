//
// ELMWrapper.cpp
//			this is part of the RcppShark package (http://github.com/aydindemircioglu/RcppShark)
//
// Copyright (C) 2015 		Aydin Demircioglu <aydin.demircioglu/at/ini.rub.de>
//
// This file is part of the RcppShark library for GNU R.
// It is made available under the terms of the GNU General Public
// License, version 2, or at your option, any later version,
// incorporated herein by reference.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more
// details.
// 
// Please do not use this software to destroy or spy on people, environment or things.
// All negative use is prohibited.
//

#include <shark/Models/LinearModel.h>
#include <shark/Models/ConcatenatedModel.h>
#include <shark/Models/FFNet.h>
#include <shark/Rng/Normal.h>
#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h>
#include <shark/Algorithms/Trainers/LinearRegression.h>

//header needed for the problem
#include <shark/Data/DataDistribution.h>

//just for evaluation of the ELM
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>
#include <iostream>

using namespace std;
using namespace shark;

#include "utils.h"
#include <Rcpp.h>

using namespace Rcpp;

//' ELM wrapper, training
//'
//' @param X		matrix with input data
//' @param Y		labels
//' @param nHidden		number of hidden neurons 
//' @param verbose		verbose output?
//'
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List ELMWrapperTrain (NumericMatrix X, NumericVector Y, size_t nHidden, bool verbose = false) {

	try {
		// TODO: maybe do some checks?
		
		if (verbose == true) {
			Rcpp::Rcout << "Parameters:\n";
			Rcpp::Rcout<<"\thidden neurons: \t\t" << nHidden << "\n";
		}
		
		// convert data
		if (verbose == true) Rcpp::Rcout << "Converting data.. " << std::endl;
		UnlabeledData<RealVector> inputs = NumericMatrixToUnlabeledData(X);
		Data<RealVector> labels = NumericVectorToDataRealVector (Y);
		
		RegressionDataset data;
		data.inputs() = inputs;
		data.labels() = labels;
		
		
		//usually an elm uses zero mean unit variance inputs. so we should
		//normalize the data first
		Normalizer<> normalizer;
		NormalizeComponentsUnitVariance<> normalizingTrainer(true);
		normalizingTrainer.train(normalizer,data.inputs());
		
		// now we construct the hidden layer of the elm
		// we create a two layer network and initialize it randomly. By keeping the random
		// hidden weights and only learning the visible later, we will create the elm
		size_t inputDim = inputDimension (data);
		FFNet<LogisticNeuron, LogisticNeuron> elmNetwork;
		elmNetwork.setStructure (inputDim, nHidden, labelDimension(data));
		initRandomNormal (elmNetwork, 1);
		
		//We need to train the linear part. in this simple example we use the elm standard
		//technique: linear regression. For this we need to propagate the data first 
		// through the normalization and the hidden layer of the elm
		RegressionDataset transformedData = transformInputs (data, normalizer);
		transformedData.inputs() = elmNetwork.evalLayer (0, transformedData.inputs());
		LinearModel<> elmOutput;
		LinearRegression trainer;
		trainer.train(elmOutput,transformedData);
		
		//we need to set the learned weights of the hidden layer of the elm
		elmNetwork.setLayer(1,elmOutput.matrix(),elmOutput.offset());
		
		// thats all, we do not need the model here
/*
		// now create model
		ConcatenatedModel<RealVector,RealVector> elm = normalizer >> elmNetwork;
		
		//to test whether everything works, we will evaluate the elm and the elmOutput layer separately
		//both results should be identical
		SquaredLoss<> loss;
		double outputResult = loss(transformedData.labels(), elmOutput(transformedData.inputs()));
		double elmResult = loss(transformedData.labels(), elm(data.inputs()));
		
		if (verbose) 
			Rcout<<"ELM: "<< elmResult<<std::endl;
		*/

		// list of data
		double trainError = 0;
		Rcpp::List rl = R_NilValue;
		rl = Rcpp::List::create(
			Rcpp::Named("inputSize") = inputDimension(data),
			Rcpp::Named("outputSize") = labelDimension(data),
			Rcpp::Named("nHidden") = nHidden,
			Rcpp::Named("weights") = RealVectorToNumericVector (elmNetwork.parameterVector()),
			Rcpp::Named("normalizerWeights") = RealVectorToNumericVector (normalizer.parameterVector())
		);
		return rl;
    } catch(...) {
		stop ("Unknown exception occured. Check also your budget strategy.");
	}
}





//' ELM Network wrapper, predict.
//'
//' @param X		matrix with input data
//' @param Y		true labels. if an empty vector is given, no predictionError is computed
//' @param weights		weights for the network (part of network model)
//' @param nHidden1		number of nodes of first hidden layer (part of network model)
//' @param nHidden2		number of nodes of second hidden layer (part of network model)
//' @param inputSize		number of nodes of input layer (part of network model)
//' @param outputSize		number of nodes of output layer (part of network model)
//' @param verbose		print extra information?
//'
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List ELMWrapperPredict (Rcpp::NumericMatrix X, 
								Rcpp::NumericVector Y,
								Rcpp::NumericVector weights,
								Rcpp::NumericVector normalizerWeights,
								size_t nHidden,
								size_t inputSize,
								size_t outputSize,
								bool verbose = false) {
	
	try {
		// TODO: maybe do some checks?
		
		// convert data
		if (verbose) Rcpp::Rcout << "Converting data.. " << std::endl;
		UnlabeledData<RealVector> inputs = NumericMatrixToUnlabeledData(X);
		Data<RealVector> labels = NumericVectorToDataRealVector (Y);

		RegressionDataset data;
		data.inputs() = inputs;
		data.labels() = labels;
		
		// recreate network
		FFNet<LogisticNeuron,LogisticNeuron> elmNetwork;
		elmNetwork.setStructure (inputSize, nHidden, outputSize);
		elmNetwork.setParameterVector (NumericVectorToRealVector(weights));
		
		// recreate normalizer
		Normalizer<> normalizer;
		normalizer.setStructure (inputSize);
		normalizer.setParameterVector (NumericVectorToRealVector(normalizerWeights));
		
		// now create model
 		ConcatenatedModel<RealVector,RealVector> elm = normalizer >> elmNetwork;

// 		//to test whether everything works, we will evaluate the elm and the elmOutput layer separately
// 		//both results should be identical
		double predictionError = -1;
		Data<RealVector> prediction = elm (inputs);
		if (Y.size () > 0) {
			SquaredLoss<> loss;
			
			for (int x = 0; x < 25; x++) {
				std::cout << prediction.element(x)[0] << "-" << labels.element(x)[0] << "\n";
			}
			std::cout << "\n";
			
			
			predictionError = loss (labels, prediction);
			if (verbose) {
				Rcpp::Rcout << "Prediction Error is " << predictionError << "\n";
			}
		}
				
		Rcpp::List rl = R_NilValue;
		rl = Rcpp::List::create(
			Rcpp::Named("prediction") = DataRealVectorToNumericMatrix (prediction),
			Rcpp::Named("predictionError") = predictionError
		);
		
		return rl;
	} catch(...) {
		Rcpp::stop ("Unknown exception occured.");
	}
								}
								
								