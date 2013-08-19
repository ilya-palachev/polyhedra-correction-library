# see http://perldoc.perl.org/Test/More.html
#     http://search.cpan.org/~adie/Test-Exception-0.27/lib/Test/Exception.pm

use strict;
use Error qw(:try);
use Test::More tests => 2;
use Test::Exception;


BEGIN { use_ok 'Graph::LibGEXF' }

# test XSD validation
my $valid = Graph::LibGEXF::SchemaValidator::run("../../../t/attributes.gexf", "../../../resources/xsd/gexf.xsd");
is($valid, 1, "SchemaValidator");

