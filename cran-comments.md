## Resubmission
This is a resubmission. In this version I have:

* Updated Shark source code to a current git version (i.e. to Shark 3.1)

* Fixed the build warnings, Prof. Brian Ripley pointed out


 
## Test environments
* local ubuntu 16.10 development (64bit), R 3.3.1 patched
* win-builder (devel and release)



## R CMD check results
Status: 1 NOTE
See
  ‘/tmp/Rtmp7OTpDp/RcppShark.Rcheck/00check.log’
for details.


R CMD check results
0 errors | 0 warnings | 1 note 
checking installed package size ... NOTE
  installed size is 167.4Mb
  sub-directories of 1Mb or more:
    include    4.4Mb
    lib      127.2Mb
    libs      35.7Mb

	The installed size is quite large, as the whole Shark Library 3.0 (beta) is wrapped,
	which is a machine learning library programmed in templated C++.
	For more information on Shark, please refer to http://shark-project.sourceforge.net.

	
## Downstream dependencies
There are currently no downstream dependencies for this package.
