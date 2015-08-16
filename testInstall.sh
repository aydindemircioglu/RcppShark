# compile things
Rscript ./compile.R

# now build and test
R CMD build .
R CMD check --as-cran ./RcppShark_0.1.tar.gz


