//
// BSGDWrapper.cpp
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


//noisy AutoencoderModel model and deep network
#include <shark/Models/FFNet.h>// neural network for supervised training
#include <shark/Models/Autoencoder.h>// the autoencoder to train unsupervised
#include <shark/Models/ImpulseNoiseModel.h>// model adding noise to the inputs
#include <shark/Models/ConcatenatedModel.h>// to concatenate Autoencoder with noise adding model

//training the  model
#include <shark/ObjectiveFunctions/ErrorFunction.h>//the error function performing the regularisation of the hidden neurons
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h> // squared loss used for unsupervised pre-training
#include <shark/ObjectiveFunctions/Loss/CrossEntropy.h> // loss used for supervised training
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h> // loss used for evaluation of performance
#include <shark/ObjectiveFunctions/Regularizer.h> //L1 and L2 regularisation
#include <shark/Algorithms/GradientDescent/SteepestDescent.h> //optimizer: simple gradient descent.
#include <shark/Algorithms/GradientDescent/Rprop.h> //optimizer for autoencoders


#include "utils.h"
#include <Rcpp.h>

using namespace std;
using namespace shark;
using namespace Rcpp;



// helper function:
//  training of an auto encoder with one hidden layer
template<class AutoencoderModel>
AutoencoderModel trainAutoencoderModel(
	UnlabeledData<RealVector> const& data,//the data to train with
	std::size_t numHidden,//number of features in the AutoencoderModel
	double regularisation,//strength of the regularisation
	double noiseStrength, // strength of the added noise
	std::size_t iterations, //number of iterations to optimize
	bool verbose = false
){
	//create the model
	std::size_t inputs = dataDimension(data);
	AutoencoderModel baseModel;
	baseModel.setStructure(inputs, numHidden);
	initRandomUniform(baseModel,-0.1*std::sqrt(1.0/inputs),0.1*std::sqrt(1.0/inputs));
	ImpulseNoiseModel noise(noiseStrength,0.0);//set an input pixel with probability p to 0
	ConcatenatedModel<RealVector,RealVector> model = noise>> baseModel;

	//create the objective function
	LabeledData<RealVector,RealVector> trainSet(data,data);//labels identical to inputs
	SquaredLoss<RealVector> loss;
	ErrorFunction error(trainSet, &model, &loss);
	TwoNormRegularizer regularizer(error.numberOfVariables());
	error.setRegularizer(regularisation,&regularizer);

	//set up optimizer
	IRpropPlusFull optimizer;
	optimizer.init(error);
	if (verbose == true) Rcout<<"Optimizing model: "+model.name()<<std::endl;
	for(std::size_t i = 0; i != iterations; ++i){
		optimizer.step(error);
		if (verbose == true) Rcout<<i<<" "<<optimizer.solution().value<<std::endl;
	}

	model.setParameterVector(optimizer.solution().point);
	return baseModel;
}

typedef Autoencoder<RectifierNeuron,LinearNeuron> AutoencoderModel;//type of autoencoder
typedef FFNet<RectifierNeuron,LinearNeuron> Network;//final supervised trained structure



// helper function:
//   unsupervised pre training of a network with two hidden layers
Network unsupervisedPreTraining(
	UnlabeledData<RealVector> const& data,
	std::size_t numHidden1,std::size_t numHidden2, std::size_t numOutputs,
	double regularisation, double noiseStrength, std::size_t iterations,
	bool verbose = false)
{	
	//train the first hidden layer
	if (verbose == true) Rcout<< "Training first layer"<<std::endl;
	AutoencoderModel layer =  trainAutoencoderModel<AutoencoderModel>(
		data,numHidden1,
		regularisation, noiseStrength,
		iterations, verbose
	);
	
	//compute the mapping onto the features of the first hidden layer
	UnlabeledData<RealVector> intermediateData = layer.evalLayer(0,data);
	
	//train the next layer
	if (verbose == true) Rcout<< "Training second layer"<<std::endl;
	AutoencoderModel layer2 =  trainAutoencoderModel<AutoencoderModel>(
		intermediateData,numHidden2,
		regularisation, noiseStrength,
		iterations, verbose
	);
	
	//create the final network
	Network network;
	network.setStructure(dataDimension(data),numHidden1,numHidden2, numOutputs);
	initRandomNormal(network,0.1);
	network.setLayer(0,layer.encoderMatrix(),layer.hiddenBias());
	network.setLayer(1,layer2.encoderMatrix(),layer2.hiddenBias());
	
	return network;
}



//' Deep Network wrapper.
//'
//' @param X		matrix with input data
//' @param Y		labels
//' @param nHidden1		number of nodes of first hidden layer (part of network model)
//' @param nHidden2		number of nodes of second hidden layer (part of network model)
//' @param unsupRegularisation 	regularization factor of supervised training
//' @param noiseStrength		noise strength for unsupervised training
//' @param unsupIterations			iteration number for unsupervised training
//' @param regularisation		regularisation factor for supervised training
//' @param iterations		iteration number for supervised training
//' @param verbose		print extra information?
//'
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List DeepNetworkWrapperTrain (Rcpp::NumericMatrix X, Rcpp::NumericVector Y, 
							  size_t nHidden1 = 8,
							  size_t nHidden2 = 8,
							  double unsupRegularisation = 0.001,
							  double noiseStrength = 0.3,
							  std::size_t unsupIterations = 100,
							  double regularisation = 0.0001,
							  std::size_t iterations = 200,
							  bool verbose = false) {

	try {
		// TODO: maybe do some checks?
		
		if (verbose == true) {
			Rcpp::Rcout << "Parameters:\n";
			Rcpp::Rcout<<"\titerations: \t\t" << iterations << "\n";
		}
		
		// convert data
		if (verbose == true) Rcpp::Rcout << "Converting data.. " << std::endl;
		UnlabeledData<RealVector> inputs = NumericMatrixToUnlabeledData(X);
		Data<unsigned int> labels = NumericVectorToLabels (Y);
		
		ClassificationDataset data;
		data.inputs() = inputs;
		data.labels() = labels;
		
		//unsupervised pre training
		Network network = unsupervisedPreTraining(
			data.inputs(), nHidden1, nHidden2, numberOfClasses(data), unsupRegularisation, noiseStrength, unsupIterations, verbose);
		
		//create the supervised problem. Cross Entropy loss with one norm regularisation
		CrossEntropy loss;
		ErrorFunction error(data, &network, &loss);
		OneNormRegularizer regularizer(error.numberOfVariables());
		error.setRegularizer(regularisation,&regularizer);
		
		//optimize the model
		if (verbose == true) Rcpp::Rcout << "Training supervised model" << std::endl;
		IRpropPlusFull optimizer;
		optimizer.init(error);
		for(std::size_t i = 0; i != iterations; ++i){
			optimizer.step(error);
			if (verbose == true) Rcout << "  " << i << " " << optimizer.solution().value << std::endl;
		}
		network.setParameterVector(optimizer.solution().point);

		double trainError = 0;
		
		Rcpp::List rl = R_NilValue;
		rl = Rcpp::List::create(
			Rcpp::Named("error") = trainError,
			Rcpp::Named("weights") = optimizer.solution().point,
			Rcpp::Named("nHidden1") = nHidden1,
			Rcpp::Named("nHidden2") = nHidden2,
			Rcpp::Named("inputSize") = dataDimension(inputs),
			Rcpp::Named("outputSize") = numberOfClasses(data)
		);
		return rl;
    } catch(...) {
		Rcpp::stop ("Unknown exception occured.");
	}
}



//' Deep Network wrapper.
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
List DeepNetworkWrapperPredict (Rcpp::NumericMatrix X, 
								Rcpp::NumericVector Y,
								Rcpp::NumericVector weights,
								size_t nHidden1,
								size_t nHidden2,
								size_t inputSize,
								size_t outputSize,
								bool verbose = false) {
	
	try {
		// TODO: maybe do some checks?
		
		// convert data
		if (verbose) Rcpp::Rcout << "Converting data.. " << std::endl;
		UnlabeledData<RealVector> inputs = NumericMatrixToUnlabeledData(X);
	
		// recreate network
		Network network;
		network.setStructure (inputSize, nHidden1, nHidden2, outputSize);
		network.setParameterVector(NumericVectorToRealVector (weights));
		
		double predictionError = -1;
		Data<RealVector> prediction = network (inputs);
		if (Y.size () > 0) {
			ZeroOneLoss <unsigned int, RealVector> loss01;
			predictionError = loss01.eval (NumericVectorToLabels (Y), prediction);
			if (verbose) {
				Rcpp::Rcout << "Prediction Error is " << predictionError;
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

