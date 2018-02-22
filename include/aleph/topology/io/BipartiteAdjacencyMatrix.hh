#ifndef ALEPH_TOPOLOGY_IO_MATRIX_HH__
#define ALEPH_TOPOLOGY_IO_MATRIX_HH__

#include <algorithm>
#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include <aleph/utilities/String.hh>

namespace aleph
{

namespace topology
{

namespace io
{

/**
  @class BipartiteAdjacencyMatrixReader
  @brief Reads bipartite adjacency matrices in text format

  This reader class is meant to load bipartite adjacency matrices in
  text format. Every row of the matrix represents edges that connect
  nodes from the first class with nodes of the second class. Weights
  that are non-zero are used to indicate the presence of an edge.

  The number of rows and columns must not vary over the file. An *empty*
  line is permitted, though. Likewise, lines starting with `#` will just
  be ignored. An example of a 2-by-3 matrix follows:

  \code
  0 1 2
  3 4 5
  \endcode
*/

class BipartiteAdjacencyMatrixReader
{
public:

  /**
    Reads a simplicial complex from a file.

    @param filename Input filename
    @param  K       Simplicial complex
  */

  template <class SimplicialComplex> void operator()( const std::string& filename, SimplicialComplex& K )
  {
    std::ifstream in( filename );
    if( !in )
      throw std::runtime_error( "Unable to read input file" );

    this->operator()( filename, K );
  }

  /** @overload operator()( const std::string&, SimplicialComplex& ) */
  template <class SimplicialComplex> void operator()( std::istream& in, SimplicialComplex& K )
  {
    using Simplex    = typename SimplicialComplex::ValueType;
    using DataType   = typename Simplex::DataType;

    auto position = in.tellg();

    std::size_t height = 0;
    std::size_t width  = 0;

    using namespace aleph::utilities;

    {
      std::string line;
      while( std::getline( in, line ) )
      {
        line        = trim( line );
        auto tokens = split( line );

        if( width == 0 )
          width = tokens.size();
        else if( width != tokens.size() )
          throw std::runtime_error( "Format error: number of columns must not vary" );

        ++height;
      }

      in.clear();
      in.seekg( position );
    }

    _height = height;
    _width  = width;

    using Simplex    = typename SimplicialComplex::ValueType;
    using DataType   = typename Simplex::DataType;
    using VertexType = typename Simplex::VertexType;

    std::vector<DataType> values;
    values.reserve( _height * _width );

    std::copy( std::istream_iterator<DataType>( in ), std::istream_iterator<DataType>(),
               std::back_inserter( values ) );

    std::vector<Simplex> simplices;

    // Vertices --------------------------------------------------------
    //
    // Create a vertex for every node in the input data. An (n,m)-matrix
    // thus gives rise to n+m nodes.

    {
      VertexType v = VertexType();

      for( std::size_t i = 0; i < _height + _width; i++ )
        simplices.push_back( Simplex( VertexType( i ) ) );
    }

    // Edges -----------------------------------------------------------
    //
    // Vertex indices start go from [0,rows-1] for the nodes of class one,
    // and from [rows,rows+columns] for the nodes of class two.

    for( std::size_t y = 0; y < _height; y++ )
    {
      for( std::size_t x = 0; x < _width; x++ )
      {
        auto i = static_cast<VertexType>( width * y + x   );
        auto w = values.at( i );

        // Map matrix indices to the corresponding vertex indices as
        // outline above.
        auto u = VertexType(y);
        auto v = VertexType(x + _height);

        simplices.push_back( Simplex( {u,v}, w ) );
      }
    }

    K = SimplicialComplex( simplices.begin(), simplices.end() );
  }

  /** @returns Height of matrix that was read last */
  std::size_t height() const noexcept { return _height; }

  /** @returns Width of matrix that was read last */
  std::size_t width()  const noexcept { return _width;  }

private:
  std::size_t _height = 0;
  std::size_t _width  = 0;
};

} // namespace io

} // namespace topology

} // namespace aleph

#endif
