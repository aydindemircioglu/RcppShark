
/*
 * Get yourself r-devel. Just build it, or use a Docker instance, or ...
 * Run it on a package. Eg I just did for (the GitHub version of) RcppArmadillo as I knew I had not (yet) registered its fastLm() function. And surely, RD CMD check (using r-devel) warns.
 * The key: be lazy and just call tools::package_native_routine_registration_skeleton(".") from r-devel as has been suggested before. In my case it identifies four entry points, and sets up the file. I copied that to src/init.c.
 * Rebuild, re-check. Worst case you need to also adapt NAMESPACE.
 */

#include <R.h>                                                                                                                                                                                                        
#include <Rinternals.h>                                                                                                                                                                                               
#include <stdlib.h> // for NULL                                                                                                                                                                                       
#include <R_ext/Rdynload.h>                                                                                                                                                                                           

/* FIXME:                                                                                                                                                                                                             
 *   Check these declarations against the C/Fortran source code.                                                                                                                                                        
 */                                                                                                                                                                                                                    

/* .Call calls */                                                                                                                                                                                                     
extern SEXP RcppShark_BSGDWrapperPredict(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);                                                                                                                                         
extern SEXP RcppShark_BSGDWrapperTrain(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);                                                                                                                               
extern SEXP RcppShark_DeepNetworkWrapperPredict(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);                                                                                                                            
extern SEXP RcppShark_DeepNetworkWrapperTrain(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);                                                                                                            

static const R_CallMethodDef CallEntries[] = {
	{"RcppShark_BSGDWrapperPredict",        (DL_FUNC) &RcppShark_BSGDWrapperPredict,         6},
	{"RcppShark_BSGDWrapperTrain",          (DL_FUNC) &RcppShark_BSGDWrapperTrain,           8},
	{"RcppShark_DeepNetworkWrapperPredict", (DL_FUNC) &RcppShark_DeepNetworkWrapperPredict,  7},
	{"RcppShark_DeepNetworkWrapperTrain",   (DL_FUNC) &RcppShark_DeepNetworkWrapperTrain,   10},
	{NULL, NULL, 0}
};

void R_init_RcppShark(DllInfo *dll)
{
	R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
	R_useDynamicSymbols(dll, FALSE);
}
