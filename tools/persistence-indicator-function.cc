#include <iostream>
#include <string>
#include <vector>

#include "persistenceDiagrams/PersistenceDiagram.hh"
#include "persistenceDiagrams/PersistenceIndicatorFunction.hh"

#include "persistenceDiagrams/io/Raw.hh"

#include "utilities/Filesystem.hh"

using DataType           = double;
using PersistenceDiagram = aleph::PersistenceDiagram<DataType>;

int main( int argc, char** argv )
{
  if( argc <= 1 )
  {
    // TODO: Show usage
    return -1;
  }

  // Get filenames -----------------------------------------------------

  std::vector<std::string> filenames;
  filenames.reserve( argc - 1 );

  for( int i = 1; i < argc; i++ )
    filenames.push_back( argv[i] );

  // Load persistence diagrams -----------------------------------------

  std::vector<PersistenceDiagram> persistenceDiagrams;
  persistenceDiagrams.reserve( filenames.size() );

  for( auto&& filename : filenames )
  {
    std::cerr << "* Processing '" << filename << "'...";

    PersistenceDiagram persistenceDiagram = aleph::io::load<DataType>( filename );

    // FIXME: This is only required in order to ensure that the
    // persistence indicator function has a finite integral; it
    // can be solved more elegantly by using a special value to
    // indicate infinite intervals.
    persistenceDiagram.removeUnpaired();

    persistenceDiagrams.push_back( persistenceDiagram );

    std::cerr << "finished\n";
  }

  // Calculate persistence indicator functions -------------------------

  unsigned i = 0;
  for( auto&& D : persistenceDiagrams )
  {
    auto f        = aleph::persistenceIndicatorFunction( D );
    auto filename = filenames.at(i);

    using namespace aleph::utilities;

    auto outputFilename = "/tmp/PIF_" + stem( basename( filename ) ) + ".txt";

    std::cerr << "* Writing persistence indicator function to '" << outputFilename << "'...\n";

    std::ofstream out( outputFilename );
    out << f << "\n";

    ++i;
  }
}
