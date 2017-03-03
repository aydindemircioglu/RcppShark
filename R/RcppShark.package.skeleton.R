## RcppShark -- An R interface to the Shark machine learning library
##
## Copyright (C)  2014  Qiang Kou
## This file was part of RcppMLPACK. 
## Parts borrowed from Rcpp.
##
## Copyright (C) 2017  Aydin Demircioglu
##
## This file is part of the RcppShark library for GNU R.
## It is made available under the terms of the GNU General Public
## License, version 3, or at your option, any later version,
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




#' create a skeleton for a package using RcppShark
#'
#' @param name    character string: the package name and directory name for your package.
#' @param list	character vector naming the R objects to put in the package. 
#'				Usually, at most one of "list", "environment", or "code_files" will be supplied.  See "Details" of package.skeleton.
#' @param environment 	an environment where objects are looked for.  See "Details" of package.skeleton.
#' @param path		 path to put the package directory in.
#' @param force	 If "FALSE" will not overwrite an existing directory.
#' @param code_files	 a character vector with the paths to R code files to build the package around.  See "Details" of package.skeleton.
#' @param example_code		add example code to package?
#' @param author		author in the created DESCRIPTIONS file
#' @param maintainer		maintainer in the created DESCRIPTIONS file
#' @param email		email in the created DESCRIPTIONS file
#' @param license		licence in the created DESCRIPTIONS file
#'
#' @importFrom utils package.skeleton packageDescription
#' @export
RcppShark.package.skeleton <- function(name="anRpackage", 
										list=character(),
										environment=.GlobalEnv,
										path=".", 
										force=FALSE, 
										code_files = character(), 
										example_code = TRUE,
										author = "Your Name",
										maintainer = if (missing(author)) "Your Name" else author,
										email = "your@email.com",
										license = "GPL (>= 2)")	
{
    call <- match.call()
    call[[1]] <- as.name("package.skeleton")
    env <- parent.frame(1)

    if (!is.character(code_files))
        stop("'code_files' must be a character vector")

    if (!length(list)) {
        fake <- TRUE
        assign("Rcpp.fake.fun", function() {}, envir = env)
        if (example_code && !isTRUE(attributes)) {
            assign("rcpp_hello_world", function() {}, envir = env)
            remove_hello_world <- TRUE
        } else {
            remove_hello_world <- FALSE
        }
    } else {
        if (example_code && !isTRUE(attributes)) {
            if (!"rcpp_hello_world" %in% list) {
                assign( "rcpp_hello_world", function() {}, envir = env)
                call[["list"]] <- as.call(c(as.name("c"),
                                            as.list(c("rcpp_hello_world", list))))
            }
            remove_hello_world <- TRUE
        } else {
            remove_hello_world <- FALSE
        }
        fake <- FALSE
    }

    ## first let the traditional version do its business
    ## remove Rcpp specific arguments

    call <- call[ c(1L, which(names(call) %in% names(formals(package.skeleton)))) ]

    if (fake) {
        call[["list"]] <- c(if(isTRUE(example_code)
                               && !isTRUE(attributes)) "rcpp_hello_world", "Rcpp.fake.fun")
    }

    tryCatch(eval(call, envir = env), error = function(e){
        stop(sprintf("error while calling `package.skeleton` : %s", conditionMessage(e)))
    })

	message("\nAdding RcppShark settings")

	## now pick things up 
	root <- file.path(path, name)

	## Add Rcpp to the DESCRIPTION
	DESCRIPTION <- file.path(root, "DESCRIPTION")
	if (file.exists(DESCRIPTION)) {
		imports <- c(sprintf("Rcpp (>= %s)", packageDescription("Rcpp")[["Version"]]))
		x <- cbind(read.dcf(DESCRIPTION),
				"Imports" = paste(imports, collapse = ", "),
				"LinkingTo" = "Rcpp, RcppShark, BH")
		x[, "Author"] <- author
		x[, "Maintainer"] <- sprintf("%s <%s>", maintainer, email)
		x[, "License"] <- license
		x[, "Title"] <- "What the Package Does in One 'Title Case' Line"
		x[, "Description"] <- "One paragraph description of what the package does as one or more full sentences."
		message( " >> added Imports: Rcpp" )
		message( " >> added LinkingTo: Rcpp, RcppShark, BH" )
		write.dcf(x, file = DESCRIPTION)
	}


	## add useDynLib and importFrom to NAMESPACE
	NAMESPACE <- file.path(root, "NAMESPACE")
	lines <- readLines(NAMESPACE)
	ns <- file(NAMESPACE, open="w")
	if (! grepl("useDynLib", lines)) {
		lines <- c(sprintf( "useDynLib(%s)", name), lines)
		writeLines(lines, con = ns)
		message(" >> added useDynLib directive to NAMESPACE" )
	}
	writeLines('importFrom(Rcpp, evalCpp)', ns)
	message(" >> added importFrom(Rcpp, evalCpp) directive to NAMESPACE" )
	close( ns )

	
	## lay things out in the src directory
	src <- file.path(root, "src")
	if (!file.exists(src)) {
		dir.create(src)
	}
	skeleton <- system.file("skeleton", package="RcppShark")
			
	if (length(code_files) > 0L) {
		for (file in code_files) {
			file.copy(file, src)
			message(" >> copied ", file, " to src directory" )
		}
	}

	Makevars <- file.path(src, "Makevars")
	if (!file.exists(Makevars)) {
		file.copy(file.path(skeleton, "Makevars"), Makevars)
		message(" >> added Makevars from ", file.path(skeleton, "Makevars"), " to ", Makevars)
	}

	Makevars.win <- file.path(src, "Makevars.win")
	if (! file.exists( Makevars.win)) {
		file.copy(file.path(skeleton, "Makevars.win"), Makevars.win)
		message(" >> added Makevars.win file with RcppShark settings")
	}

	if (example_code) {
		file.copy(file.path(skeleton, "KMeansTutorial.cpp"), src)
		file.copy(file.path(skeleton, "utils.h"), src)
		file.copy(file.path(skeleton, "utils.cpp"), src)
		message(" >> added example src file using RcppShark classes")
	}

	if (fake) {
		rm("Rcpp.fake.fun", envir=env)
		unlink(file.path(root, "R"  , "Rcpp.fake.fun.R"))
		unlink(file.path(root, "man", "Rcpp.fake.fun.Rd"))
	}

	invisible(NULL)
}

