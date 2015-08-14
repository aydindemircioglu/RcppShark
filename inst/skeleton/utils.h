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


Data<unsigned int> NumericVectorToLabels (NumericVector X, bool verbose = false);
UnlabeledData<RealVector> NumericMatrixToUnlabeledData (NumericMatrix X, bool verbose = false);
NumericMatrix UnlabeledDataToNumericMatrix (UnlabeledData<RealVector> X, bool verbose = false);
NumericVector LabelsToNumericVector (Data<unsigned int> X, bool verbose = false);


#endif
