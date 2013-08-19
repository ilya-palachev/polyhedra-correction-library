#!/bin/bash
module_version=0.01
echo " ---------------------------------"
echo " |         Perl5 building        |"
echo " ---------------------------------"
echo ""
echo "Generating interface files.."
swig -c++ -perl -fvirtual -o libgexf_wrap.cpp libgexf.i
mv LibGEXF.pm lib/Graph/

# adding module version (dirty way)
head lib/Graph/LibGEXF.pm -n -1 > lib/Graph/LibGEXF.pm.tmp
echo "use version;" >> lib/Graph/LibGEXF.pm.tmp
echo "our \$VERSION = '$module_version';" >> lib/Graph/LibGEXF.pm.tmp
echo "1;" >> lib/Graph/LibGEXF.pm.tmp
cat lib/Graph/LibGEXF.pm.tmp pmdoc > lib/Graph/LibGEXF.pm
rm lib/Graph/LibGEXF.pm.tmp

echo "Compiling.."
perl Makefile.PL
make

echo "Testing.."
make test

#echo "Cleaning.."

echo "Done!"

