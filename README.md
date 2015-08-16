## RcppShark

Goal of this package is to allow users to use the 
[Shark C++ machine learning library](http://image.diku.dk/shark/sphinx_pages/build/html/index.html)
in their own R projects.


### Example 

The package can be loaded by 

> library(RcppShark)

We added only two basic examples, Budgeted Stochastic Gradient Descend (BSGD) and a four layered Deep Neural Network, to give a feeling how RcppShark works. Remember, the goal of the package is not to wrap all existing functionality of Shark in R, but to give you access to a very powerful and fast machine learning library (We might add more pre-wrapped code in future, if enough interest and time is given). Now to use the Budgeted Stochastic Gradient Descend algorithm from Shark on your data, you can call

> SharkBSGDTrain (x, y, gamma = 0.5, C = 1.0, budget = 500)

The data has  to be binary, with 0-1 labels. (TODO: check this in code).


## Tutorial

RcppShark is basically a bridge to allow creating packages based on Shark. It also allows for rapid prototyping using Shark as the underlying machine learning library. Therefore, the most important part is to understand how to use RcppShark in your own packages. For this reason, RcppShark contains (the usual) package skeleton creator. To develop your own package using RcppShark, you simply call after installing RcppShark

> library (RcppShark)

> RcppShark.package.skeleton ("myOwnPackage")

(make sure your own name is valid). This will create a empty package structure where you can freely use Shark in the C++ part. Let us wrap the KMeans algorithm, following the tutorial from the Shark homepage (http://image.diku.dk/shark/sphinx_pages/build/html/rest_sources/tutorials/algorithms/kmeans.html). 

For this we want basically two functions: One that will 'learn' the clustering and cluster the given training data, and one that will 'predict' the clustering to new data. Let us start with the first part. We start with the C++ code, and will only add R code, if it is necessary. Rcpp will export our C++ function to R in a very nice fashion, so it is not always necessary to add R code, at least not for our little tutorial.

Now we can write the corresponding C++ function. For this we follow the Shark tutorial by creating a new file in the src folder of our new package, called KMeans.cpp, and importing all the headers we need:

```C
#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/Centroids.h>//model performing hard clustering of points
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points

#include <Rcpp.h>
#include "utils.h" // some conversion helpers

using namespace shark;
using namespace std;

using namespace Rcpp;
```

Note that we have skipped the CSV and normalizer header, as we have full control over both in R, and added Rcpp as well as our small utils.h header which contains some code to convert from the Shark to the Rcpp data format (and back). Lets continue with the main routine. This will take a matrix, the number of clusters $k$ to find and will return a list with the following two information: The final clustering in form of a model consisting of the centroids and a vector with the cluster assignments. So we start with the header:

```C
//' Simple KMeans Train
//' 
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List SharkKMeansTrain (NumericMatrix X, ssize_t k) {
```

The first thing to do is to convert the given data in form of a matrix into an object, Shark can work with. For this, we call the corresponding function from the utils.h class:

```C
	// convert data
	UnlabeledData<RealVector> data = NumericMatrixToUnlabeledData(X);
	std::size_t elements = data.numberOfElements();
```

Now comes the Shark code. For more details on this, please consult the Shark tutorial. We drop here everything that we do not need, e.g. printing information.

```C
	// compute centroids using k-means clustering
	Centroids centroids;
	kMeans (data, k, centroids);

	// convert cluster centers/centroids
	Data<RealVector> const& c = centroids.centroids();
	NumericMatrix cM = DataRealVectorToNumericMatrix (c);
	
	// cluster training data we are given and convert to Rcpp object
	HardClusteringModel<RealVector> model(&centroids);
	Data<unsigned> clusters = model(data);
	NumericVector labels = LabelsToNumericVector (clusters);
```

Finally we just need to put all the bits into a list and return it to R:
	
```C
	// return solution found 
	Rcpp::List rl = R_NilValue;
	rl = Rcpp::List::create(
		Rcpp::Named("labels") = labels,
		Rcpp::Named("centroids") = cM );
	return (rl);

```

Please note that for sake of brevity we dropped all kind of checks. For production code such checks obviously should be done.

Before we test this code, let us quickly write the corresponding predict function (which is more or less already part of the training function, so we could drop that there):

```C
//' Simple KMeans Predict
//' 
//' @export
//'
// [[Rcpp::depends(BH)]]    
// [[Rcpp::export]]	
List SharkKMeansPredict (NumericMatrix X, NumericMatrix centroids) {

	// convert data
	UnlabeledData<RealVector> data = NumericMatrixToUnlabeledData (X);
	std::size_t elements = data.numberOfElements();

	// convert centroids
	Centroids c (NumericMatrixToDataRealVector (centroids));
	
	// cluster data we are given and convert to Rcpp object
	HardClusteringModel<RealVector> model (&c);
	Data<unsigned> clusters = model (data);
	NumericVector labels = LabelsToNumericVector (clusters);

	// return solution found 
	rl = Rcpp::List::create(Rcpp::Named("labels") = labels);
	return (rl);
}
```

Running this code is now easy, as by Rcpp::export tag, Rcpp will automatically create a wrapper to be called from R. This is good enough for us, though usually you really want to wrap the C++ function in a nice R routine. So we can now write a small function that will use our KMeans routine by creating first some example data and then calling our KMeans:

```R
library (devtools)
load_all (".")

# convert iris to matrix
data = as.matrix(iris[,1:4])

model = SharkKMeansTrain (data, 3)
labels = SharkKMeansPredict (data, model$centroids)
```

Note that we need the devtools package, as we develop a new package.  The output should be look similar to this:

```R
> model
$labels
  [1] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 [38] 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 [75] 1 1 1 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 1 2 2 2 2 1 2 2 2 2
[112] 2 2 1 1 2 2 2 2 1 2 1 2 1 2 2 1 1 2 2 2 2 2 1 2 2 2 2 1 2 2 2 1 2 2 2 1 2
[149] 2 1

$centroids
         [,1]     [,2]     [,3]     [,4]
[1,] 5.006000 3.428000 1.462000 0.246000
[2,] 5.901613 2.748387 4.393548 1.433871
[3,] 6.850000 3.073684 5.742105 2.071053

> labels
$labels
  [1] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 [38] 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 [75] 1 1 1 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 1 2 2 2 2 1 2 2 2 2
[112] 2 2 1 1 2 2 2 2 1 2 1 2 1 2 2 1 1 2 2 2 2 2 1 2 2 2 2 1 2 2 2 1 2 2 2 1 2
[149] 2 1
```

For convenience every new created skeleton contains the above KMeans example.



## Notes

- Shark is heavily templated. Therefore compiling is rather slow. Luckily, running times make up for this mess.

- Serialization is turned off. We wanted to avoid hassle with all the different boost::serialization problems, therefore we created a special Shark version with no serialization at all. Therefore, we only need headers from Boost. This allows us to use the BH package, which again makes hassle-free installation of this package possible.

- It is not guaranteed (yet) that Shark will compile all examples and tutorials from the Shark page properly. Currently we need much more tests. In case you find some error, please report it to us so that we can fix it asap.

- This was forked from the very old RcppShark project (see https://github.com/eddelbuettel/RcppShark), which was a prototype of an RcppBridge for the old Shark 2.x version. But as the package is nearly independent of the old project, there is no link on github anymore.

- To avoid crossplatform problems, the header SimulatedBinaryCrossover.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Recombination is renamed to BinaryCrossover.h



### Internal notes

To transform the Shark code to RcppShark, simply call python ./prepareShark.sh. This will assume that the Shark sources can be found under ~/Shark. It will then extract the parts it needs and put them under ./src (previous sources in src/shark and src/src will be deleted, so be sure not to have individual changes to these files). Currently this process is 100% (as of 8th august 2015), there is no manual step involved. This might change, if later versions of Shark become available.



### Authors

Aydin Demircioglu (aydin.demircioglu /at/ ini.rub.de)



### Credits

This package uses infrastructure from different packages. Thanks for their efforts go to
- Shane Conway and Dirk Eddelbuettel (original rShark package)
- Qiang Kou (original RcppMLPACK package)



### License

GPL 3.0 for the R package, Shark 3.0 itself is license under the LGPL

