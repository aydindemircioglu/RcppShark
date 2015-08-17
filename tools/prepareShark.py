#!/usr/bin/python

# 
# \file        prepareShark.py
#
# \brief       a small tool that will prepare Shark for the rShark package
#
# \author      Aydin Demircioglu
# \date        2014
#
#
# \par Copyright 1995-2015 Shark Development Team
# 
# <BR><HR>
# This file is part of Shark.
# <http://image.diku.dk/shark>
# 
# Shark is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published 
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Shark is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with Shark.  If not, see <http:#www.gnu.org/licenses/>.
#

import os
import hashlib
import shutil
import argparse
import re
import time
import pyparsing
from pyparsing import *



verbose = False

print "\nShark preparation tool v0.1\n\n"


# parse arguments
#parser = argparse.ArgumentParser (description='Replace headers.')
#parser.add_argument('path', metavar='path', type=str, nargs='+', help='path')
#args = parser.parse_args()

# parse working directory
originalDir = os.path.abspath( os.path.expanduser ("~/Shark")) #os.path.abspath(args.path[0])
print 'Working in directory: ', originalDir

verbose = True

# we need to work on include and src subdirs, the rest is not important for us  (or will be dealt with later)

storagePath = os.path.abspath("../src/")
print "Storage path for modified source is: ", storagePath


# remove ALL old things
import shutil
try:
	shutil.rmtree(os.path.abspath("../src/shark"))
except:
	pass

try:
	shutil.rmtree(os.path.abspath("../src/src"))
except:
	pass



originalFiles = []
count = 0
includeDirs = [os.path.abspath(originalDir + "/include"), os.path.abspath(originalDir + "/src")]


for  includeDir in includeDirs: 
	print 'Working in directory: ', includeDir
	for root, subFolders, files in os.walk(includeDir):
		for file in files:
			filepath = os.path.abspath (os.path.join(root, file))
			filesize = os.path.getsize(filepath)
		
			# only accept .h .hpp .c .cpp .tpp .tut 
			r=re.compile('.*(\.h|\.hpp|\.c|\.cpp|\.tpp|\.inl)$')
			if r.match (filepath):
				if (verbose == True):
					print filepath
			else:
				continue

			# read whole file
			with open(filepath) as f:
				data = f.read()

			try:
				# check whether we have any ISerializable
				modified = False
				
				# check whether we have a brief and a copyright
				if True == True:
					# do some replacement
					modified = True


					for i in range(5):
						enclosed = Forward()
						nestedBrackets = SkipTo ("SHARK_EXPORT_SYMBOL void read", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True)("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass


						enclosed = Forward()
						nestedBrackets = SkipTo ("SHARK_EXPORT_SYMBOL void write", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(";") + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )

						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass

						enclosed = Forward()
						nestedBrackets = SkipTo ("SHARK_EXPORT_SYMBOL virtual void read", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True)("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass


						enclosed = Forward()
						nestedBrackets = SkipTo ("SHARK_EXPORT_SYMBOL virtual void write", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(";") + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )

						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass

					for i in range (5):
						enclosed = Forward()
						nestedBrackets = SkipTo ("virtual void read", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True)("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass



						enclosed = Forward()
						nestedBrackets = SkipTo ("virtual void write", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass
					


					# just do it multiple times..
					for i in range(4):
						enclosed = Forward()
						nestedBrackets = SkipTo ("void read", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True)("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass



						enclosed = Forward()
						nestedBrackets = SkipTo ("void write", include = True)("pre") + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass
					


						enclosed = Forward()
						nestedBrackets = SkipTo ("void" + Word(alphanums) + "::read", include = True)("pre")  + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass



						enclosed = Forward()
						nestedBrackets = SkipTo ("void" + Word(alphanums) + "::write", include = True)("pre")  + nestedExpr( '(',  ')' ) + ZeroOrMore("const" ) + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass



#namespace boost {
#namespace serialization {

#template< typename T >
#struct tracking_level< shark::TypedFlags<T> > {
    #typedef mpl::integral_c_tag tag;
    #BOOST_STATIC_CONSTANT( int, value = track_always );
#};

#}
#}


					enclosed = Forward()
					nestedBrackets = SkipTo ("namespace serialization", include = True)("pre")  + nestedExpr( '{',  '}' ) + SkipTo (StringEnd(), include = True).setResultsName("post")
					enclosed << ( nestedBrackets )
					
					try:
						matchS = enclosed.parseString (data)
						data = (matchS['pre'][0] + matchS['post'])
					except:
						pass


        #template<class Archive>
        #void serialize(Archive &ar, const unsigned int file_version) {
                #boost::serialization::collection_size_type count(size());
                #ar & count;
                #if(!Archive::is_saving::value){
                        #resize(count);
                #}
                #if (!empty())
                        #ar & boost::serialization::make_array(storage(),size());
                #(void) file_version;//prevent warning
        #}


					for i in range(4):
						enclosed = Forward()
						nestedBrackets = SkipTo ("template" + "<" + Word(alphanums)   + "Archive" + ">"+ "void" + "serialize" + nestedExpr( '(',  ')' ), include = True)("pre") + ZeroOrMore(nestedExpr( '{',  '}' ) ) + SkipTo (StringEnd(), include = True).setResultsName("post")
						enclosed << ( nestedBrackets )
						
						try:
							matchS = enclosed.parseString (data)
							data = (matchS['pre'][0] + matchS['post'])
						except:
							pass


					# fix for 		shark::blas::diag(covariance) = blas::repeat(1.0,dim); 
					# probably only in DataDistribution.h. not really, we have one more in  CrossEntropyLoss
					data = re.sub(r'(?i)([^>][\s]*)diag([\s]*\([^>]+\))', r'\1shark::blas::diag \2', data)
					
					# ?					noalias(shark::blas::diag(hessian)) += gradient;
					# data = re.sub(r'(?i)([\s]*)diag[\s]*\(', r'\1shark::blas::diag (', data)


					# hard fix
					data = re.sub(r'(?ims)SHARK_EXPORT_SYMBOL[\s]*;', r'', data)

					data = re.sub(r'(?i).include.*?serialization.*?[\n]', r'', data)

					
					# 1. remove  #include <shark/Core/ISerializable.h>
					data = re.sub(r'(?i).include..shark.Core.ISerializable.h.', r'', data)
					
					# 2. case: class LineSearch:public ISerializable {
					data = re.sub(r'(?i).include..shark.Core.ISerializable.h.', r'', data)

					# 3. case public Ithing. hard way again
					data = re.sub(r'(?i):.?public ISerializable[\s\n]*\{', r'{', data)
					data = re.sub(r'(?i):.?public ISerializable[\s\n]*,', r': ', data)

					# 3. case public Ithing. hard way again
					data = re.sub(r'(?i)(public .*?), public ISerializable', r'\1', data)

					# 4. standard comment FIXME somehow produces dots in /home/drunkeneye/ST/rShark/include/shark/Models/Clustering/ClusteringModel.h:76:9:
					if len(re.findall("std::cout", data)) > 0:
						data = re.sub(r'(?is)std::cout', r'Rcpp::Rcout', data)
						data = re.sub(r'(?ism)^#include', r'#include <Rcpp.h>\n#include', data, 1)

					if len(re.findall("rand[\s]*\(", data)) > 0:
						data = re.sub(r'(?is)rand[\s]*\([\s]*\)', r' int( round (R::runif(0,RAND_MAX)) ) ', data)
						data = re.sub(r'(?ism)^#include', r'#include <Rcpp.h>\nusing namespace Rcpp;\n#include', data, 1)

					# could be done for Rng.h only, more specific, but heck.
					if len(re.findall("boost::mt19937", data)) > 0:
						data = re.sub(r'(?i)boost::mt19937', r'boost::rand47', data)
						data = re.sub(r'(?ism)^#include', r'#include <shark/Rng/Runif.h>\n#include', data, 1)

					if len(re.findall("boost::random_shuffle", data)) > 0:
						data = re.sub(r'(?i)boost::random_shuffle.subsetIndices.;', r'DiscreteUniform < shark::Rng::rng_type > uniform(shark::Rng::globalRng, 0, RAND_MAX ); std::random_shuffle(subsetIndices.begin(), subsetIndices.end(), uniform);', data)
						data = re.sub(r'(?ism)^#include <boost/range/algorithm/random_shuffle.hpp>', r'', data)
						data = re.sub(r'(?ism)^#include', r'#include <shark/Rng/GlobalRng.h>\n#include', data, 1)

					# there is another place where std::random_shuffle is being used :(
					if len(re.findall("std::random_shuffle", data)) > 0:
						data = re.sub(r'(?i)std::random_shuffle[\s]*.v.begin[\s]*.[\s]*., v.end[\s]*.[\s]*.[\s]*.;', r'DiscreteUniform < shark::Rng::rng_type > uniform(shark::Rng::globalRng, 0, RAND_MAX ); std::random_shuffle(v.begin(), v.end(), uniform);', data)
						data = re.sub(r'(?ism)^#include <boost/range/algorithm/random_shuffle.hpp>', r'', data)
						data = re.sub(r'(?ism)^#include', r'#include <shark/Rng/GlobalRng.h>\n#include', data, 1)
					

					#print (data)
				# save file to our local path
				newPath = os.path.relpath(filepath, originalDir )
				dumpFile = os.path.join(storagePath, newPath)
				try:
					os.makedirs (os.path.dirname(dumpFile))
				except:
					pass
				#print (dumpFile)

				if modified == True:
					with open(dumpFile, "w") as text_file:
						text_file.write(data)
					print "  Processed", filepath
					count = count + 1
					#if count > 1:
						#exit()
				
			except (StopIteration) as e:
				# no header found, decide if we should add one
				
				continue
			else:
				pass

# move the include/shark directory to ./shark
shutil.move(os.path.abspath("../src/include/shark"), os.path.abspath("../src/shark"))
			
# finally we need to copy over our random generator
shutil.copy ("./Runif.h", os.path.join(storagePath, "shark", "Rng"))
os.remove("../src/shark/Core/ISerializable.h")
shutil.rmtree("../src/include")

		
print "\nProcessed", count, "files.\n\n"

