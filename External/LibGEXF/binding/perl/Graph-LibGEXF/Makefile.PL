use inc::Module::Install;
name 'Graph-LibGEXF';
all_from 'lib/Graph/LibGEXF.pm';

# requires '';

tests 't/*.t';
author_tests 'xt';

build_requires 'Test::More';
use_test_base;
auto_include;



cc_lib_links
  'm',
  'stdc++', # IMPORTANT: linker to lib STD
  'xml2'
  ;

# if object files are not in the same dir as source files
makemaker_args
  DEFINE => '-o $*$(OBJ_EXT) -c';

cc_inc_paths
  '.',
  '/usr/include/libxml2';

cc_files
    #io::input
    '../../../libgexf/filereader.o',
    '../../../libgexf/abstractparser.o',
    '../../../libgexf/gexfparser.o',
    '../../../libgexf/legacyparser.o',
    '../../../libgexf/rngvalidator.o',
    '../../../libgexf/schemavalidator.o',
    
    #io::output
    '../../../libgexf/filewriter.o',
    '../../../libgexf/legacywriter.o',
    
    #io::utils
    '../../../libgexf/conv.o',
    
    #db::topo
    '../../../libgexf/graph.o',
    '../../../libgexf/directedgraph.o',
    '../../../libgexf/undirectedgraph.o',
    '../../../libgexf/nodeiter.o',
    '../../../libgexf/edgeiter.o',
    
    #db::data
    '../../../libgexf/data.o',
    '../../../libgexf/metadata.o',
    '../../../libgexf/attributeiter.o',
    '../../../libgexf/attvalueiter.o',
    
    #main
    '../../../libgexf/gexf.o',
    '../../../libgexf/memoryvalidator.o',

    # chemin du wrapper généré automatiquement par SWIG (ce wrapper doit déjà exister donc)
    'libgexf_wrap.o';

can_cc or die 'This module requires C++ compiler.';




WriteAll;

