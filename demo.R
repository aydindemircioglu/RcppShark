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
	#	s = bsgd (x, y, gamma = 1, epochs = 3, budget = 500)


# simple kmeans 
	c = applyKMeans (x)
	print(c)
	
	
# four-layer (deep) neural network with pretraining
	cat("\nDeep network example.\n")
	
	# train and predict on the same set 
	model = DeepNetworkTrain (x, y, nHidden1 = 32, nHidden2 = 32, verbose = FALSE)
	results = DeepNetworkPredict (x, y, model, verbose = FALSE)

	# prediction is not argmaxed yet
	networkPrediction = apply (results$prediction, 1, which.max) - 1
	errors = sum(y - networkPrediction)/length(y)
	cat("Network produced ", errors, "errors.\n")


# Extreme learning machine
	cat("\nELM network example.\n")
	
	# create a swiss role
	x = SwissRoll (2000)
	y = x[,4]/max(x[,4])
	x = x[,1:3]
	model = ELMNetworkTrain (x, y)
	results = ELMNetworkPredict (x, y, model, verbose = TRUE)

	# regression, use squared loss
	print (y[1:10])
	print (results$prediction[1:10])
	error = sum(y - results$prediction)^2/length(y)
	cat("Network error on test set is ", error, "\n")

