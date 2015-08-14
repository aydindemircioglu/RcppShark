## RcppShark -- R interface to the Shark libraries
##
## Copyright (C) 2010  Shane Conway and Dirk Eddelbuettel
##
## This file is part of the RShark library for GNU R.
## It is made available under the terms of the GNU General Public
## License, version 2, or at your option, any later version,
## incorporated herein by reference.
##
## This program is distributed in the hope that it will be
## useful, but WITHOUT ANY WARRANTY; without even the implied
## warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
## PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public
## License along with this program; if not, write to the Free
## Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
## MA 02111-1307, USA


SharkBSGDTrain <- function(x, y = NULL, 
                 verbose = TRUE, budget = 500, useBias = TRUE,
                 strategy = "Merge",
                 C = 1, gamma=1,  epochs = 1)
{
    type <- match.arg(type)

    checkmate::assertMatrix(x)
	checkmate::assertVector(y)
	checkmate::checkNumber(C, lower = 0)
	checkmate::checkNumber(gamma, lower = 0)
	checkmate::assertCount(budget)
	checkmate::assertCount(epochs)
	checkmate::assertString (strategy)
	checkmate::assertFlag (useBias)
	checkmate::assertFlag (verbose)

    val <- .Call("RcppShark_BSGDWrapper",
				X=x, y=y,
				C=C,
				budget = budget,
				gamma=gamma,
				epochs = epochs,
				budgetMaintenanceStrategy = "Merge",
				useBias = useBias,
				verbose = verbose,
				PACKAGE="RcppShark")
    print("Finished..")
    class(val) <- c("RcppShark.BSGD")
    val
}



SharkBSGDPredict <- function(x, y = NULL, 
                 verbose = TRUE, budget = 500, useBias = TRUE,
                 strategy = "Merge",
                 C = 1, gamma=1,  epochs = 1)
{
}