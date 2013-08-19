# see http://perldoc.perl.org/Test/More.html
#     http://search.cpan.org/~adie/Test-Exception-0.27/lib/Test/Exception.pm

use strict;
use Error qw(:try);
use Test::More tests => 2;
use Test::Exception;


BEGIN { use_ok 'Graph::LibGEXF' }

# test import file
my $reader = Graph::LibGEXF::FileReader->new();
$reader->init("../../../t/attributes.gexf");
$reader->slurp();
my $gexf = $reader->getGEXFCopy();
my $valid = $gexf->checkIntegrity();
is($valid, 1, "Integrity");

if( $valid ) {
    # test export file
    my $writer = Graph::LibGEXF::FileWriter->new();
    $writer->init("../../../t/writer_test.gexf", $gexf);
    $writer->write();
}

