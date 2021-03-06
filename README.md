## RcppShark

Goal of this package is to allow users to use the 
[Shark C++ machine learning library](http://image.diku.dk/shark/sphinx_pages/build/html/index.html)
in their own R projects. Currently the github version from 19th februrary 2017 is used.



### Basic Example 

This package is meant to be used as a basis for other packages. Nonetheless you can use this directly.
It can be loaded by 

> library(RcppShark)

We added only two basic examples, Budgeted Stochastic Gradient Descend (BSGD) and a four layered Deep Neural Network, to give you a feeling how RcppShark works. Remember, the goal of the package is not to wrap all existing functionality of Shark in R, but to give you access to a very powerful and fast machine learning library (We might add more pre-wrapped code in future, if there is enough interest). Now to use the Budgeted Stochastic Gradient Descend algorithm from Shark on your data, perpare a binary classification problem. Then you can call the wrapper with

> model = SharkBSGDTrain (x, y, gamma = 0.5, C = 1.0, budget = 500)

The data has to be binary, with 0-1 labels (We do not check for that!). Notice that SharkBSGDTrain is just a nice wrapper for the underlying C++ implementation called BSGDWrapperTrain in BSGDWrapper.cpp. We exported this function too, in case you want to meddle directly with the underlying code. To predict, you simply go ahead and push your test data as well as the model the training produced to the predict function:

> results = SharkBSGDPredict (x, model)

The results are now the predicted labels. You can go ahead and compute with your favorite R function statistics like test error etc.


## Changes

The source code of Shark is largly unchanged. Apart from removing asserts (via -DNDEBUG) and replacing std::cout and similar things, some files had to be renamed, as there sulting path is either too long or does conflict with crossplatform compability. Notably:

- SimulatedBinaryCrossover.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Recombination is renamed to BinaryCrossover.h
- HypervolumeContributionApproximator.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Hypervolume/ is renamed to HVContrApproximator.h
- HypervolumeContribution2D.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Hypervolume/  is renamed to  HVContribution2D.h
- HypervolumeContribution3D.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Hypervolume/  is renamed to  HVContribution3D.h
- HypervolumeContributionMD.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Hypervolume/  is renamed to  HVContributionMD.h
- HypervolumeContribution2D.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Hypervolume/  is renamed to  HVContribution2D.h
- PartiallyMappedCrossover.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Recombination/ is renamed to Recombination/PMXCrossover.h
- PartiallyMappedCrossover.h in ./inst/include/shark/Algorithms/DirectSearch/Operators/Recombination/ is renamed to Recombination/PMXCrossover.h
- aligned_alloc_android.hpp in ./inst/include/shark/LinAlg/BLAS/kernels/default/boost_align/detail/aligned_alloc_a.hpp
- is_alignment_constant.hpp in ./inst/include/shark/LinAlg/BLAS/kernels/default/boost_align/detail/is_alignment_c.hpp





## Notes

- Shark is heavily templated. Therefore compiling is rather slow. Luckily, running times make up for this mess.

- Serialization is turned off. We wanted to avoid hassle with all the different boost::serialization problems, therefore we created a special Shark version with no serialization at all. Therefore, we only need headers from Boost. This allows us to use the BH package, which again makes hassle-free installation of this package possible.

- It is not guaranteed (yet) that Shark will compile all examples and tutorials from the Shark page properly. Currently we need much more tests. In case you find some error, please report it to us so that we can fix it asap.

- This was forked from the very old RcppShark project (see https://github.com/eddelbuettel/RcppShark), which was a prototype of an RcppBridge for the old Shark 2.x version. But as the package is nearly independent of the old project, there is no link on github anymore.

- The random number generator was replaced, as R packages must use the random generator from R and not the C/C++ internal one. Thereffore, a direct comparison of results of algorithms that depend on (pseudo) random numbers cannot be done.



### Internal notes

- To transform the Shark code to RcppShark, simply call python ./prepareShark.sh. This will assume that the Shark sources can be found under ~/Shark. It will then extract the parts it needs and put them under ./src (previous sources in src/shark and src/src will be deleted, so be sure not to have individual changes to these files). Currently this process is 100% (as of 19th februrary 2017), there is no manual step involved. This might change, if later versions of Shark become available.

- To create a proper Makevars, or to change things, edit the template found under ./tools/Makevars_template. Then call the python script ./generateMakevars.py. This script will find all *.cpp files below /src/src and will add them to the Makevars. 

- Finally make sure that the Shark you have checked out below ~/Shark was compiled without LAPACK support, as R packages with LAPACK are not really supported by CRAN. If you forget to do so, an error message like "clapack.h not found" will be thrown. 

- If you want to compile it by yourself, follow these instructions:

1. Download Shark into ~/Shark
2. Compile shark via cmake .
3. Call ./prepareShark.py in ./tools
4. Call ./generateMakevars.py in ./tools
5. Call ./testInstall.py in ./
6. Call ./postTestInstall.py in ./


### Authors

Aydin Demircioglu (aydin.demircioglu /at/ ini.rub.de)



### Credits

This package uses infrastructure from different packages. Thanks for their efforts go to

- Shane Conway and Dirk Eddelbuettel (original rShark package)
- Qiang Kou (original RcppMLPACK package)

and obviously to all the R and Shark people for creating such nice packages.



### License

GPL 3.0 for the R package, Shark 3.x itself is licensed under LGPL.

