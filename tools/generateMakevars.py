#!/usr/bin/python

# 
# \file        generateMakevars.py
#
# \brief       a small tool that will prepare Makevars for the RcppShark package
#
# \author      Aydin Demircioglu
# \date        2015
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

print "\nRcppShark generate Makevar tool v0.1\n\n"


includeDir = os.path.abspath("../src/src") #os.path.abspath(args.path[0])
objects = ""
rules = ""

for root, subFolders, files in os.walk(includeDir):
	for file in files:
		filepath = os.path.abspath (os.path.join(root, file))
		filesize = os.path.getsize(filepath)
	
		# only accept .h .hpp .c .cpp .tpp .tut 
		r=re.compile('.*(\.cpp)$')
		if r.match (filepath):
			if (verbose == True):
				print filepath
		else:
			continue
		
		# for each file we have, we create one entry for the object list
		stem = os.path.splitext (file)[0]
		if (stem == "Version"):
			continue
		
		objects = objects + stem + ".o" + "\\\n"
		
		# add a rule
		relpath = os.path.relpath( filepath, "../src") 
		rules = rules + stem + ".o :\n"
		rules = rules + "		$(CXX) $(PKG_CPPFLAGS)  $(PKG_CXXFLAGS)   -c " + relpath + "\n\n"

# remove last newline
objects = re.sub(r'(?sm)(.*)\\', r'\1', objects)

if verbose == True:
	print objects
if verbose == True:
	print rules

# now load makefile
with open("./Makevars_template") as f:
	makefile = f.read()

makefile = re.sub('##OBJECTS##', objects, makefile)
makefile = re.sub('##RULES##', rules, makefile)

with open("../src/Makevars", "w") as f:
	f.write (makefile)

if verbose == True:
	print (makefile)

