/*! \file : libgexf.i
    \brief SWIG interface
 */
%module "Graph::LibGEXF"

%ignore operator<<;
/*%nodefaultdtor;*/

%{
#define SWIG_FILE_WITH_INIT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
/* main */
#include "../../../libgexf/typedefs.h"
#include "../../../libgexf/exceptions.h"
#include "../../../libgexf/gexf.h"
#include "../../../libgexf/abstractiter.h"
#include "../../../libgexf/memoryvalidator.h"
/* io::input */
#include "../../../libgexf/filereader.h"
#include "../../../libgexf/abstractparser.h"
#include "../../../libgexf/gexfparser.h"
#include "../../../libgexf/legacyparser.h"
#include "../../../libgexf/rngvalidator.h"
#include "../../../libgexf/schemavalidator.h"
/* io::output */
#include "../../../libgexf/filewriter.h"
#include "../../../libgexf/legacywriter.h"
/* io::utils */
#include "../../../libgexf/conv.h"
/* db::topo */
#include "../../../libgexf/graph.h"
#include "../../../libgexf/directedgraph.h"
#include "../../../libgexf/undirectedgraph.h"
#include "../../../libgexf/nodeiter.h"
#include "../../../libgexf/edgeiter.h"
/* db::data */
#include "../../../libgexf/data.h"
#include "../../../libgexf/metadata.h"
#include "../../../libgexf/attributeiter.h"
#include "../../../libgexf/attvalueiter.h"
%}

%include typemaps.i
%include stl.i
%include exception.i
%include std_except.i

%exception {
  try {
    $action
  }
  catch (const std::exception& e) {
    SWIG_croak(e.what());
  } catch(...) {
    SWIG_croak("Unknown error");
  }
}

namespace std {
  %template(vectors) std::vector<std::string>;
}


/* Let's just grab the original header file here */
/* main */
%include "../../../libgexf/typedefs.h"
%include "../../../libgexf/exceptions.h"
%include "../../../libgexf/gexf.h"
%include "../../../libgexf/abstractiter.h"
%include "../../../libgexf/memoryvalidator.h"
/* io::input */
%include "../../../libgexf/filereader.h"
%include "../../../libgexf/abstractparser.h"
%include "../../../libgexf/gexfparser.h"
%include "../../../libgexf/legacyparser.h"
%include "../../../libgexf/rngvalidator.h"
%include "../../../libgexf/schemavalidator.h"
/* io::output */
%include "../../../libgexf/filewriter.h"
%include "../../../libgexf/legacywriter.h"
/* io::utils */
%include "../../../libgexf/conv.h"
/* db::topo */
%include "../../../libgexf/graph.h"
%include "../../../libgexf/directedgraph.h"
%include "../../../libgexf/undirectedgraph.h"
%include "../../../libgexf/nodeiter.h"
%include "../../../libgexf/edgeiter.h"
/* db::data */
%include "../../../libgexf/data.h"
%include "../../../libgexf/metadata.h"
%include "../../../libgexf/attributeiter.h"
%include "../../../libgexf/attvalueiter.h"

