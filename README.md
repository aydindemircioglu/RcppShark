## RcppShark

R package to wrap the 
[Shark C++ machine learning library](http://image.diku.dk/shark/sphinx_pages/build/html/index.html)



### Status

This was forked from the very old rShark project https://github.com/eddelbuettel/RcppShark
It is just a very small prototype to see, if linking works with Shark 3.0 too.
Indeed, it does. Now this knowledge will be gathered in a dedicated Shark R-wrapper.
(As soon as we have some coding zombies doing this task).

The package can be loaded by 

> library(devtools)

> load_all(".")

> ssvm(x, y, gamma = 0.5, C = 1.0, epsilon = 1e-7)

The data has  to be binary, with 0-1 labels. There is no check for that currently.
Obviously the whole thing might have many bugs, for now it seems to work with my setup.


## tutorials

Most often you will not be interested in the prewrapped functions of the package. Instead the aim is to use the underlying Shark library to code new things directly into R. For this reason, RcppShark contains (the usual) package skeleton creator. To develop your own package using RcppShark, you simply call

> library (RcppShark)

> RcppShark.package.skeleton ("myOwnPackage")

(make sure your own name is valid). This will create a empty package structure where you can freely use Shark in the C++ part. For example, to get your own..



## NOTE

- Shark is heavily templated. Therefore compiling is rather slow. Luckily, running times make up for this mess.

- Serialization is turned off. We wanted to avoid hassle with all the different boost::serialization problems, therefore we created a special Shark version with no serialization at all. Therefore, we only need headers from Boost. This allows us to use the BH package, which again makes hassle-free installation of this package possible.

- It is not guaranteed (yet) that Shark will compile all examples and tutorials from the Shark page properly. Currently we need much more tests. In case you find some error, please report it to us so that we can fix it asap.


### Internal notes

To transform the Shark code to RcppShark, simply call python ./prepareShark.sh. This will assume that the Shark sources can be found under ~/Shark. It will then extract the parts it needs and put them under ./src (previous sources will be deleted, so be sure not to have individual changes to these files). Currently this process is 100p (as of 8th august 2015), there is no manual step involved. This might change, if later versions of Shark become available.



### Authors

Aydin Demircioglu)



### Credits

This package uses infrastructure from different packages. Thanks for their efforts go to
- Shane Conway and Dirk Eddelbuettel (original rShark package)
- Qiang Kou (original RcppMLPACK package)



### License

GPL 3.0 for the R package, Shark 3.0 itself is license under the LGPL

