# 
	library(MASS)
	library(devtools)
	load_all(".")
	


SwissRoll <- function(N=2000, Height=30) {
    ## build manifold
    p = (3 * pi / 2) * (1 + 2 * runif(N, 0, 1));  
    y = Height * runif(N, 0 , 1);
    samples = cbind(p * cos(p), y, p * sin(p));
	samples = cbind(samples, p)
    return(samples)
}
	
	
# convert iris to matrix
	x = as.matrix(iris[,1:4])
	y = as.vector(as.numeric(iris[,5]))
	# make sure its binary
	y = replace(y, y == 2, 0)
	y = replace(y, y == 3, 0)


# SVM:  budgeted SGD
	model = BSGDWrapperTrain (x, y, C = 1, budget = 15, gamma = 1, epochs = 5, budgetMaintenanceStrategy = "Merge")
	model$gamma = 1
	BSGDWrapperPredict (x, model$alpha, model$SV, model$offset, model$gamma)

	
# four-layer (deep) neural network with pretraining
	cat("\nDeep network example.\n")
	
	# train and predict on the same set 
	model = DeepNetworkTrain (x, y, nHidden1 = 32, nHidden2 = 32, verbose = FALSE)
	results = DeepNetworkPredict (x, y, model, verbose = FALSE)

	# prediction is not argmaxed yet
	networkPrediction = apply (results$prediction, 1, which.max) - 1
	errors = sum(y - networkPrediction)/length(y)
	cat("Network produced ", errors, "errors.\n")

