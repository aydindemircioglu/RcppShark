## Resubmission
This is a resubmission. In this version I have:

* Updated Shark source code to a current git version (i.e. to Shark 3.1 head)

* Along the way, fixed the build warning regarding dates of vignette and inst/doc


 
## Test environments
* local ubuntu 16.10 development (64bit), R 3.3.1 patched
* win-builder (devel and release)



## R CMD check results
Status: 1 NOTE

R CMD check results
0 errors | 0 warnings | 1 note 
checking installed package size ... NOTE
  installed size is 170.2Mb
  sub-directories of 1Mb or more:
    include    4.6Mb
    lib      129.3Mb
    libs      36.1Mb

	The installed size is quite large, as the whole Shark Library 3.1 is wrapped,
	which is a machine learning library programmed in templated C++.
	For more information on Shark, please refer to http://image.diku.dk/shark/.

	
## Downstream dependencies
There are currently no downstream dependencies for this package.
