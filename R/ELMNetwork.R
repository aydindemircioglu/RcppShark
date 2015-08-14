


ELMNetworkTrain <- function(X, Y, nHidden = 8192, verbose = FALSE) 
{
	checkmate::checkMatrix (X)
	checkmate::checkVector (Y)
	checkmate::checkCount (nHidden, positive = TRUE)
	checkmate::checkFlag (verbose)
	
	model = .Call('RcppShark_ELMWrapperTrain', PACKAGE = 'RcppShark', 
		X, Y, nHidden, verbose)
	
	return (model)
}



ELMNetworkPredict <- function(X, Y, model, verbose = FALSE) {
	checkmate::checkMatrix (X)
	checkmate::checkVector (Y)
	checkmate::checkFlag (verbose)

	weights = model$weights
	normalizerWeights = model$normalizerWeights
	nHidden = model$nHidden
	inputSize = model$inputSize 
	outputSize = model$outputSize

    .Call('RcppShark_ELMWrapperPredict', PACKAGE = 'RcppShark', X, Y, weights, normalizerWeights, nHidden, inputSize, outputSize, verbose)
}


