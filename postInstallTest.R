
	devtools::install()
	library(RcppShark)

	packageName = "testPackage"
	oldDir = getwd ()
	setwd ("./tmp")
	unlink (packageName)
	RcppShark.package.skeleton (packageName)
	
	library(devtools)
	load_all (packageName)
	
	

