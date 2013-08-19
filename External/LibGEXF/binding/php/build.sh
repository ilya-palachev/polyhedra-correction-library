#!/bin/bash
extension_dir=/usr/lib/php5/20060613+lfs #edit it
echo " ------------------------------"
echo " |        PHP building        |"
echo " ------------------------------"
echo ""
echo "Generating interface files.."
swig -c++ -php -fvirtual -o libgexf_wrap.cpp libgexf.i

echo "Compiling.."
gcc `php-config --includes` -fPIC -c libgexf_wrap.cpp \
	../../libgexf/gexf.cpp \
    ../../libgexf/memoryvalidator.cpp \
    ../../libgexf/filereader.cpp \
    ../../libgexf/abstractparser.cpp \
    ../../libgexf/gexfparser.cpp \
    ../../libgexf/legacyparser.cpp \
    ../../libgexf/rngvalidator.cpp \
    ../../libgexf/schemavalidator.cpp \
    ../../libgexf/filewriter.cpp \
    ../../libgexf/legacywriter.cpp \
    ../../libgexf/conv.cpp \
    ../../libgexf/graph.cpp \
    ../../libgexf/directedgraph.cpp \
    ../../libgexf/undirectedgraph.cpp \
    ../../libgexf/nodeiter.cpp \
    ../../libgexf/edgeiter.cpp \
    ../../libgexf/data.cpp \
    ../../libgexf/metadata.cpp \
    ../../libgexf/attributeiter.cpp \
    ../../libgexf/attvalueiter.cpp \
    -I/usr/include/libxml2

g++ -shared -lxml2 libgexf_wrap.o \
    gexf.o \
    memoryvalidator.o \
    filereader.o \
    abstractparser.o \
    gexfparser.o \
    legacyparser.o \
    rngvalidator.o \
    schemavalidator.o \
    filewriter.o \
    legacywriter.o \
    conv.o \
    graph.o \
    directedgraph.o \
    undirectedgraph.o \
    nodeiter.o \
    edgeiter.o \
    data.o \
    metadata.o \
    attributeiter.o \
    attvalueiter.o \
    -o libgexf.so

chmod -x libgexf.so

echo "Cleaning.."
./clean.sh

echo "Installing libgexf.so to the PHP extension directory.."
echo -n "Type the PHP extension directory path, followed by [ENTER]: "
read extension_dir
sudo cp libgexf.so $extension_dir

if [ "$extension_dir" != "" ]; then
  echo "Done!"
else
  echo "Installation cancelled."
fi
