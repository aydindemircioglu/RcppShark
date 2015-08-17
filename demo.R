	library(devtools)
	load_all(".")
		
	
# convert iris to matrix
	x = as.matrix(iris[,1:4])
	y = as.vector(as.numeric(iris[,5]))
	# make sure its binary
	y = replace(y, y == 2, 0)
	y = replace(y, y == 3, 0)


# SVM:  budgeted SGD
	cat ("\n# Budgeted SGD example \n")
	model = SharkBSGDTrain (x, y, C = 0.0001, budget = 5, gamma = 1, epochs = 1, strategy = "Merge")
	results = SharkBSGDPredict (x, model)
	cat ("BSGD training error is ", sum(abs(y - results$predictions))/length(y), "\n")
	
# four-layer (deep) neural network with pretraining
	cat("\n# Deep network example.\n")
	model = DeepNetworkTrain (x, y, nHidden1 = 32, nHidden2 = 32)
	results = DeepNetworkPredict (x, model)
	# prediction is not argmaxed yet
	networkPrediction = apply (results$prediction, 1, which.max) - 1
	errors = sum(abs(y - networkPrediction))/length(y)
	cat("Network produced ", errors, "errors.\n")

