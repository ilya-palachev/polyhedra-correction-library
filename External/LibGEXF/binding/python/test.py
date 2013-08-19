#! /usr/bin/env python
import libgexf
from libgexf import *
input_file = "../../../../t/attributes.gexf"
output_file = "../../../../t/writer_test.gexf"
output_legacy_file = "../../../../t/writer_test_legacy.gexf"

""" Import """
reader = FileReader()
reader.init(input_file)
reader.slurp()
gexf = reader.getGEXFCopy()
gexf.checkIntegrity()

""" Neighbors """
v = gexf.getDirectedGraph().getNeighbors("n0")
print v

""" Export (gexf 1.1) """
writer = FileWriter()
writer.init(output_file,gexf)
writer.write()

""" Legacy Export (gexf 1.0) """
lwriter = LegacyWriter()
lwriter.init(output_legacy_file,gexf)
lwriter.write()

""" XSD validation """
valid = SchemaValidator.run("../../../../t/writer_test.gexf", "../../../../resources/xsd/1.1draft.xsd")
print "File written valid: %s." %valid

