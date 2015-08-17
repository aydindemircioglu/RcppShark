## Resubmission

This is a resubmission. In this version I have:

* Replace all "unsigned long long" with size_t to avoid "warning: 'long long' is a C++11 extension [-Wc++11-long-long]". Note that this code is part of the underlying Shark library, and actually should be handled by the Shark community. 

* Remove dummy test and add a very basic check for budget size of BSGD (Note that BSGD is only an example on how to use the package)


 
## Test environments

* local ubuntu 15.10 alpha2 install (64bit), R 3.2.2 RC r68609
* win-builder (devel and release)
* virtual sun os 11 (32bit) R 3.1.1



## R CMD check results
Status: 2 NOTEs

* checking CRAN incoming feasibility ... NOTE
Maintainer: ‘Aydin Demircioglu <aydin.demircioglu@ini.rub.de>’
New submission

	This is a new submission.

	
* checking installed package size ... NOTE
  installed size is 171.9Mb
  sub-directories of 1Mb or more:
    include    4.2Mb
    lib      129.8Mb
    libs      37.7Mb

	The installed size is quite large, as the whole Shark Library 3.0 (beta) is wrapped,
	which is a machine learning library programmed in templated C++.
	For more information on Shark, please refer to http://shark-project.sourceforge.net.

	
## Downstream dependencies
There are currently no downstream dependencies for this package.
