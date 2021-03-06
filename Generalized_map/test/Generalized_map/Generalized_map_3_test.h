// Copyright (c) 2016 CNRS and LIRIS' Establishments (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s)     : Guillaume Damiand <guillaume.damiand@liris.cnrs.fr>
//
#ifndef CGAL_GMAP_3_TEST_H
#define CGAL_GMAP_3_TEST_H

#include "Generalized_map_2_test.h"
#include "GMap_test_insertions.h"

#include <iostream>
#include <fstream>

using namespace std;

template<typename GMAP>
bool check_number_of_cells_3(GMAP& gmap, unsigned int nbv, unsigned int nbe,
                             unsigned int nbf, unsigned int nbvol,
                             unsigned int nbcc)
{
  if ( !gmap.is_valid() )
  {
    std::cout<<"ERROR: the gmap is not valid."<<std::endl;
    assert(false);
    return false;
  }

  std::vector<unsigned int> nbc;
  nbc=gmap.count_all_cells();

  if (nbv!=nbc[0] || nbe!=nbc[1] || nbf!=nbc[2] || nbvol!=nbc[3] ||
      nbcc!=nbc[4])
  {
    std::cout<<"ERROR: the number of cells is not correct. We must have "
             <<" ("<<nbv<<", "<<nbe<<", "<<nbf<<", "<<nbvol<<", "<<nbcc
             <<") and we have"<<" ("<<nbc[0]<<", "<<nbc[1]<<", "<<nbc[2]<<", "
             <<nbc[3]<<", "<<nbc[4]<<")."
             <<std::endl;
    assert(false);
    return false;
  }

  trace_test_end();

  return true;
}

template<typename GMAP>
bool test_GMAP_3()
{
  GMAP gmap;

  typedef typename GMAP::Dart_handle Dart_handle;

  // Construction operations
  trace_test_begin();
  Dart_handle dh1=gmap.make_edge();
  Dart_handle dh2=gmap.make_edge();
  Dart_handle dh3=gmap.make_edge();
  if ( !check_number_of_cells_3(gmap, 6, 3, 3, 3, 3) )
    return false;

  trace_test_begin();
  gmap.template sew<1>(dh1,dh2);
  gmap.template sew<1>(gmap.alpha(dh2,0),dh3);
  if ( !check_number_of_cells_3(gmap, 4, 3, 1, 1, 1) )
    return false;

  trace_test_begin();
  Dart_handle dh5=gmap.make_combinatorial_polygon(3);
  Dart_handle dh6=gmap.make_combinatorial_polygon(3);
  if ( !check_number_of_cells_3(gmap, 10, 9, 3, 3, 3) )
    return false;

  trace_test_begin();
  gmap.template sew<2>(dh5,dh6);
  if ( !check_number_of_cells_3(gmap, 8, 8, 3, 2, 2) )
    return false;

  trace_test_begin();
  gmap.clear();
  Dart_handle dh7=gmap.make_combinatorial_hexahedron(); // f1
  Dart_handle dh8=gmap.template alpha<2,1,0,1,2>(dh7); // f2 opposite to f1
  Dart_handle dh9=gmap.template alpha<2>(dh7); // face incident to f1 and d2

  gmap.template remove_cell<2>(dh7);
  if ( !check_number_of_cells_3(gmap, 8, 12, 5, 1, 1) )
    return false;

  trace_test_begin();
  gmap.template remove_cell<2>(dh8);
  if ( !check_number_of_cells_3(gmap, 8, 12, 4, 1, 1) )
    return false;

  trace_test_begin();
  Dart_handle dh10=gmap.make_combinatorial_hexahedron();
  gmap.template sew<3>(dh9,dh10);
  if ( !check_number_of_cells_3(gmap, 12, 20, 9, 2, 1) )
    return false;

  trace_test_begin();
  GMAP gmap2(gmap);
  if ( !check_number_of_cells_3(gmap2, 12, 20, 9, 2, 1) )
    return false;
  if ( !gmap.is_isomorphic_to(gmap2) )
  {
    std::cout<<"Error: gmap and gmap2 are not isomorphic (after copy).\n";
    assert(false);
    return false;
  }

  trace_test_begin();
  gmap.template close<2>();
  if ( !check_number_of_cells_3(gmap, 12, 20, 11, 2, 1) )
    return false;
  if ( !gmap.is_volume_combinatorial_hexahedron(dh9) )
  {
    std::cout<<"Error: the closed volume is not a combinatorial hexahedron.\n";
    assert(false);
    return false;
  }

  trace_test_begin();
  gmap.template close<3>();
  if ( !check_number_of_cells_3(gmap, 12, 20, 11, 3, 1) )
    return false;

  trace_test_begin();
  gmap.template remove_cell<3>(gmap.alpha(dh9, 2, 3));
  if ( !check_number_of_cells_3(gmap, 12, 20, 11, 2, 1) )
    return false;

  trace_test_begin();
  gmap.template remove_cell<2>(gmap.alpha(dh9, 2));
  gmap.template remove_cell<2>(gmap.alpha(dh9, 1, 0, 1, 2));
  if ( !check_number_of_cells_3(gmap, 12, 20, 9, 2, 1) )
    return false;
  if ( !gmap.is_isomorphic_to(gmap2) )
  {
    std::cout<<"Error: gmap and gmap2 are not isomorphic (after close and removals).\n";
    assert(false);
    return false;
  }
  gmap.clear();

  if (!test_vertex_insertion(gmap))
    return false;

  if (!test_edge_insertion(gmap))
    return false;

  if (!test_face_insertion(gmap))
    return false;

  return true;
}


#endif // CGAL_GMAP_3_TEST_H
