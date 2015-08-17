## Submission
This is a new submission. In this version I have:

* The package is larger than usual, as it contains the whole Shark machine learning library. Shark is programmed in C++/Boost and contains hundred of classes and methods.

 
## Test environments
* local ubuntu 15.10 alpha2 install (64bit), R 3.2.2 RC r68609
* win-builder (devel and release)
* virtual sun os 11 (32bit) R latest



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
