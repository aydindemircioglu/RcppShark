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


#include <shark/Algorithms/Trainers/Budgeted/KernelBudgetedSGDTrainer.h> // the KernelBudgetedSGD trainer
#include <shark/Algorithms/Trainers/Budgeted/MergeBudgetMaintenanceStrategy.h> // the strategy the trainer will use
#include <shark/Algorithms/Trainers/Budgeted/AbstractBudgetMaintenanceStrategy.h> 

#include <shark/Algorithms/Trainers/Budgeted/MergeBudgetMaintenanceStrategy.h>
#include <shark/Algorithms/Trainers/Budgeted/ProjectBudgetMaintenanceStrategy.h>
#include <shark/Algorithms/Trainers/Budgeted/RemoveBudgetMaintenanceStrategy.h>

#include <shark/Data/DataDistribution.h> //includes small toy distributions
#include <shark/Models/Kernels/GaussianRbfKernel.h> //the used kernel for the SVM
#include <shark/ObjectiveFunctions/Loss/HingeLoss.h> // the loss we want to use for the SGD machine
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h> //used for evaluation of the classifier

#include "utils.h"
#include <Rcpp.h>


using namespace std;
using namespace Rcpp;

//' Budgeted SGD wrapper.
//'
//' @param X		matrix with input data
//' @param Y		labels
//' @param C		regularization constant
//'
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List BSGDWrapper(NumericMatrix X, NumericVector Y, double C, size_t budget, double gamma, 
				 double epochs, std::string budgetMaintenanceStrategy, bool useBias = true, bool verbose = false) {

	try {
		AbstractBudgetMaintenanceStrategy <RealVector> *strategy = NULL;
		// check for strategy
		if (budgetMaintenanceStrategy == "RemoveSmallest") {
			strategy = new RemoveBudgetMaintenanceStrategy<RealVector> (RemoveBudgetMaintenanceStrategy<RealVector>::SMALLEST);
		}
		
		if (budgetMaintenanceStrategy == "RemoveRandom") {
			strategy = new RemoveBudgetMaintenanceStrategy<RealVector> (RemoveBudgetMaintenanceStrategy<RealVector>::RANDOM);
		}
		
		if (budgetMaintenanceStrategy == "Merge") {
			strategy = new MergeBudgetMaintenanceStrategy<RealVector>();
		}
		
		if (budgetMaintenanceStrategy == "Project") {
			strategy = new ProjectBudgetMaintenanceStrategy<RealVector>();
		}
		
		if (strategy == NULL)
			stop ("Unknown budget strategy.");
		
	
		if (verbose == true) {
			Rcout << "Parameters:\n";
			Rcout<<"\tC: \t\t" << C << "\n";
			Rcout<<"\tgamma: \t\t" << gamma << "\n";
			Rcout<<"\tbudget: \t\t" << budget << "\n";
			Rcout<<"\tepochs: \t\t" << epochs << "\n";
			Rcout<<"\tbias: \t\t" << useBias << "\n";
			Rcout<<"\tstrategy: \t\t" << budgetMaintenanceStrategy << "\n";
		}
		
		// probably stupid, but for now its ok
		if (verbose) Rcout << "Converting data.. " << std::endl;
		UnlabeledData<RealVector> inputs = NumericMatrixToUnlabeledData(X);
		Data<unsigned int> labels = NumericVectorToLabels (Y);
			
		ClassificationDataset trainingData ;
		trainingData.inputs() = inputs;
		trainingData.labels() = labels;
		
		// define things	
		KernelClassifier<RealVector> kc;
		GaussianRbfKernel<> kernel(gamma);


		// train
		HingeLoss hingeLoss;
		KernelBudgetedSGDTrainer<RealVector> *kernelBudgetedSGDtrainer = 
			new KernelBudgetedSGDTrainer<RealVector> (&kernel, &hingeLoss, C, useBias, false, budget, strategy);
		kernelBudgetedSGDtrainer -> setEpochs (epochs);
		kernelBudgetedSGDtrainer -> setMinMargin (1.0);
		kernelBudgetedSGDtrainer->train (kc, trainingData);
		
		// compute loss
		ZeroOneLoss<unsigned int> loss; // 0-1 loss
		Data<unsigned int> output = kc (trainingData.inputs()); // evaluate on training set
		double trainError = loss.eval(trainingData.labels(), output);
		if (verbose) Rcout << "Training error:\t" <<  trainError << endl;
		
		// Find the support vector
		if (verbose) Rcout << "Obtaining support vectors:\t" << endl;
		RealMatrix fAlpha = kc.decisionFunction().alpha();
		size_t nSV = fAlpha.size1();
		if (verbose) Rcout << "\t" << nSV << " support vectors" << endl;

		Rcpp::NumericVector alpha(nSV);
		for (size_t x = 0; x < nSV; x++) {
			double currentAlpha = row (fAlpha, x)[0];
			alpha[x] = currentAlpha;
		}

		double offset = kc.decisionFunction().offset()[0];
		
		Rcpp::List rl = R_NilValue;
		rl = Rcpp::List::create(Rcpp::Named("error") = trainError,
        		Rcpp::Named("offset") = offset,
        		Rcpp::Named("nSV") = nSV,
        		Rcpp::Named("alpha") = alpha);
		return rl;
    } catch(...) {
		stop ("Unknown exception occured. Check also your budget strategy.");
	}
}

