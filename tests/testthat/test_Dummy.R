context("Dummy")


test_that(" if true is true", {
# convert iris to matrix
	x = as.matrix(iris[,1:4])
	y = as.vector(as.numeric(iris[,5]))
	# make sure its binary
	y = replace(y, y == 2, 0)
	y = replace(y, y == 3, 0)

	
	# expect two clusters, either the one is 1 or zero.
	for (i in seq(1,1000)) {
	# work directly
		set.seed(575)
	#	A = applyKMeans (x)
#		expect_equal (sum(A[1:50]), 0)
		#expect_equal (sum(A[51:150]), 100)
	}
})

