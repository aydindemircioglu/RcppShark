// RShark -- R interface to the Shark libraries
//
// Copyright (C) 2010  Shane Conway	<shane.conway@gmail.com>
//
// This file is part of the RShark library for GNU R.
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
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA

#ifndef RcppShark_hpp
#define RcppShark_hpp

using namespace std;

#include <shark/Data/Dataset.h>
#include <Rcpp.h>

using namespace shark;
using namespace Rcpp;



Data<unsigned int> NumericVectorToLabels (NumericVector X, bool verbose = false) {
	if (verbose) Rcout  << "Converting data.. \n";
	
	// probably stupid and slow, but for now its ok
	std::vector<unsigned int> outputStd (X.size());
	for (unsigned int i = 0; i < X.size(); i++) {
		outputStd [i] = round (X[i]);
	}
	
	return (createDataFromRange (outputStd));
}



UnlabeledData<RealVector> NumericMatrixToUnlabeledData (NumericMatrix X, bool verbose = false) {
	if (verbose) Rcout  << "Converting data.. \n";

	std::vector<RealVector> outputStd;

	// probably stupid and slow, but for now its ok
	unsigned int examples = X.rows();
	for (size_t e = 0; e < examples; e++) {
		NumericMatrix::Row zzrow = X( e, _);
		RealVector tmpRV (zzrow.size());
		std::copy (zzrow.begin(), zzrow.end(), tmpRV.begin());
		outputStd.push_back(tmpRV);
	}

	return (createDataFromRange (outputStd));
}


NumericMatrix UnlabeledDataToNumericMatrix (UnlabeledData<RealVector> X, bool verbose = false) {
	if (verbose) Rcout  << "Converting data.. \n";

	NumericMatrix C(X.numberOfElements(), dataDimension(X));
	
	
	///typedef Data<RealVector> Set;
	/// Set data;
	/// for(Set::element_iterator pos=data.elemBegin();pos!= data.elemEnd();++pos){
	///     Rcpp::Rcout<<*pos<<" ";
	///     Set::element_reference ref=*pos;
	
	
	/*
	typedef UnlabeledData<RealVector>::element_range Elements;
	Elements elements = X.inputs().elements();

	*/
	
//	DataView<Data<RealVector> > view (X);
//	for(std::size_t i = 0; i!=view.size(); ++i){
//	std::copy(view.begin(), view.end(), C.begin());
	
	// probably stupid and slow, but for now its ok
	unsigned int examples = X.numberOfElements();
	for (size_t e = 0; e < examples; e++) {
		RealVector p = X.inputs().element(e);
		NumericMatrix::Row zzrow = C( e, _);
		std::copy (p.begin(), p.end(), zzrow.begin());
	}

	return (C);
}



//	std::vector<unsigned int> labels(yR.begin(),yR.end());
//	ClassificationDataset trainingData = createLabeledDataFromRange(inputs, labels);


NumericVector LabelsToNumericVector (Data<unsigned int> X, bool verbose = false) {
	if (verbose) Rcout  << "Converting data.. \n";

	NumericVector C(X.numberOfElements());
	
	// probably stupid and slow, but for now its ok
	for (unsigned int i = 0; i < X.numberOfElements(); i++) {
		C [i] = X.element(i);
	}
	
	return (C);
}	




#endif
