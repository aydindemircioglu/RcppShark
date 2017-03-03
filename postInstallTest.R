
	devtools::install()
	library(RcppShark)

	packageName = "testPackage"
	oldDir = getwd ()
	tmpfolder = tempdir()
	tmpfolder = "/tmp"
	setwd (tmpfolder)
	unlink (packageName, recursive=TRUE)
	RcppShark.package.skeleton (packageName)
	
	library(devtools)
	load_all (packageName)
	
	

