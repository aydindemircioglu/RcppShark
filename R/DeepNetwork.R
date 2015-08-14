

DeepNetworkTrain <- function(X, Y, nHidden1 = 8L, nHidden2 = 8L, unsupRegularisation = 0.001, noiseStrength = 0.3, unsupIterations = 100L, regularisation = 0.0001, iterations = 200L, verbose = FALSE) 
{
	checkmate::checkMatrix (X)
	checkmate::checkVector (Y)
	checkmate::checkCount (nHidden1, positive = TRUE)
	checkmate::checkCount (nHidden2, positive = TRUE)
	checkmate::checkNumber (unsupRegularisation, lower = 0)
	checkmate::checkNumber (noiseStrength, lower = 0)
	checkmate::checkCount (unsupIterations, positive = TRUE)
	checkmate::checkNumber (regularisation, lower = 0)
	checkmate::checkCount (iterations, positive = TRUE)
	checkmate::checkFlag (verbose)
	
	model = .Call('RcppShark_DeepNetworkWrapperTrain', PACKAGE = 'RcppShark', 
		X, Y, nHidden1, nHidden2, unsupRegularisation, noiseStrength, unsupIterations, regularisation, iterations, verbose)
	
	return (model)
}



DeepNetworkPredict <- function(X, Y, model, verbose = FALSE) {
	checkmate::checkMatrix (X)
	checkmate::checkVector (Y)

	weights = model$weights
	nHidden1 = model$nHidden1
	nHidden2 = model$nHidden2
	inputSize = model$inputSize 
	outputSize = model$outputSize

    .Call('RcppShark_DeepNetworkWrapperPredict', PACKAGE = 'RcppShark', X, Y, weights, nHidden1, nHidden2, inputSize, outputSize, verbose)
}