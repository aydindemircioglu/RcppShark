#include <shark/Data/Csv.h> //load the csv file
#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h> //normalize

#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/Centroids.h>//model performing hard clustering of points
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points

#include <iostream>
#include <Rcpp.h>

#include "utils.h"

using namespace shark;
using namespace std;

using namespace Rcpp;

//' simple kmeans example
//' 
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
void applyKMeansFromCSVFile (std::string filename) {
		
	// read data
	UnlabeledData<RealVector> data;
	try {
		importCSV(data, filename, ' ');
	} 
	catch (...) {
		Rcout << "unable to read data from file " <<  filename << endl;
		Rcpp::stop("EXIT_FAILURE");
	}
	std::size_t elements = data.numberOfElements();

	// write statistics of input data
	Rcout << "number of data points: " << elements << " dimensions: " << dataDimension(data) << endl;

	// normalize data
	Normalizer<> normalizer;
	NormalizeComponentsUnitVariance<> normalizingTrainer(true);//zero mean
	normalizingTrainer.train(normalizer, data);
	data = normalizer(data);

	// compute centroids using k-means clustering
	Centroids centroids;
	size_t iterations = kMeans(data, 2, centroids);
	// report number of iterations by the clustering algorithm
	Rcout << "iterations: " << iterations << endl;

	// write cluster centers/centroids
	Data<RealVector> const& c = centroids.centroids();
	Rcout<<c<<std::endl;

	// cluster data
	HardClusteringModel<RealVector> model(&centroids);
	Data<unsigned> clusters = model(data);
	
	// write results to files
	ofstream c1("/tmp/cl1.csv");
	ofstream c2("/tmp/cl2.csv");
	ofstream cc("/tmp/clc.csv");
	ofstream cl("/tmp/cl.csv");
	for(std::size_t i=0; i != elements; i++) {
		if(clusters.element(i)) 
			c1 << data.element(i)(0) << " " << data.element(i)(1) << endl;
		else 
			c2 << data.element(i)(0) << " " << data.element(i)(1) << endl;
		cl << clusters.element(i) << endl;
	}
	cc << c.element(0)(0) << " " << c.element(0)(1) << endl;
	cc << c.element(1)(0) << " " << c.element(1)(1) << endl;
}




//' simple kmeans example
//' 
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
NumericVector applyKMeans (NumericMatrix X) {
	
	// convert data
	UnlabeledData<RealVector> data = NumericMatrixToUnlabeledData(X);

	std::size_t elements = data.numberOfElements();
	
	// write statistics of input data
	Rcout << "number of data points: " << elements << " dimensions: " << dataDimension(data) << endl;
	
	// normalize data
	Normalizer<> normalizer;
	NormalizeComponentsUnitVariance<> normalizingTrainer(true);//zero mean
	normalizingTrainer.train(normalizer, data);
	data = normalizer(data);
	
	// compute centroids using k-means clustering
	Centroids centroids;
	size_t iterations = kMeans(data, 2, centroids);
	// report number of iterations by the clustering algorithm
	Rcout << "iterations: " << iterations << endl;
	
	// write cluster centers/centroids
	Data<RealVector> const& c = centroids.centroids();
	Rcout<<c<<std::endl;
	
	// cluster data
	HardClusteringModel<RealVector> model(&centroids);
	Data<unsigned> clusters = model(data);
	
	// return cluster assignments
	return (LabelsToNumericVector (clusters));
}
