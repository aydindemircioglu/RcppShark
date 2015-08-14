library(methods)
library(devtools)
library(roxygen2)

roxygenize(".")
load_all (".")
build_vignettes(".")
document(".")
#devtools::build_win() 

# is done by document/loadall anyway?
library(Rcpp)
compileAttributes()

devtools::test()
devtools::check()
