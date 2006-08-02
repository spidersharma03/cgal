// Copyright (c) 2005,2006  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
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
//
// Author(s)     : Andreas Fabri, Sylvain Pion

#ifndef CGAL_LAZY_KERNEL_H
#define CGAL_LAZY_KERNEL_H

#include <CGAL/basic.h>
#include <CGAL/Filtered_predicate.h>
#include <CGAL/Cartesian_converter.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Interval_nt.h>
#include <CGAL/Kernel/Type_equality_wrapper.h>
#include <CGAL/Lazy.h>
#include <boost/mpl/if.hpp>

CGAL_BEGIN_NAMESPACE

// EK = exact kernel that will be made lazy
// Kernel = lazy kernel
template < typename EK_, typename AK_, typename E2A_, typename Kernel >
class Lazy_kernel_base {
public:
  typedef AK_   AK;
  typedef EK_   EK;
  typedef E2A_  E2A;

  template < typename Kernel2 >
  struct Base { typedef Lazy_kernel_base<EK, AK, E2A, Kernel2>  Type; };

  // What to do with the tag ?
  // Probably this should not exist, should it ?
  // struct filter_tag{};
  // typedef filter_tag                                     Kernel_tag;
  typedef typename EK::Kernel_tag                       Kernel_tag;
  // typedef typename CK::Rep_tag                          Rep_tag;

  // Types
  typedef CGAL::Lazy_exact_nt<typename EK::FT>  FT;
  typedef FT RT;

  typedef CGAL::Object Object_2;
  typedef CGAL::Object Object_3;

#define CGAL_Kernel_obj(X) \
  typedef Lazy<typename AK::X, typename EK::X, typename EK::FT, E2A>  X;

  CGAL_Kernel_obj(Data_accessor_2)
  CGAL_Kernel_obj(Conic_2)

  typedef Cartesian_coordinate_iterator_2<Kernel> Cartesian_const_iterator_2;
  typedef Cartesian_coordinate_iterator_3<Kernel> Cartesian_const_iterator_3;

  // Aff_transformation_2/3 operations are not functorized, so treat it as
  // an exterior object for now.
  // CGAL_Kernel_obj(Aff_transformation_2)
  // CGAL_Kernel_obj(Aff_transformation_3)
  typedef CGAL::Aff_transformationC2<Kernel>              Aff_transformation_2;
  typedef CGAL::Aff_transformationC3<Kernel>              Aff_transformation_3;


  // We don't touch the predicates.
  // FIXME TODO : better use a layer of Filtered_kernel on top of everything,
  //              so that semi-static filters are used as well (?).
#define CGAL_Kernel_pred(P, Pf)  \
    typedef Filtered_predicate<typename EK::P, typename AK::P, \
	                     Exact_converter, Approx_converter> P; \
    P Pf() const { return P(); }


    // We change the constructions.
#ifdef CGAL_INTERSECT_WITH_ITERATORS_2
#define CGAL_Kernel_cons(C, Cf) \
    typedef typename boost::mpl::if_<boost::is_same<typename AK::C, typename AK::Intersect_with_iterators_2>, \
                                     Lazy_intersect_with_iterators<Kernel,typename AK::C, typename EK::C>, \
                                     typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, Bbox_2>, \
                                                              Lazy_construction_bbox<Kernel,typename AK::C, typename EK::C>, \
                                                              typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, typename AK::FT>,\
                                                                                       Lazy_construction_nt<Kernel,typename AK::C, typename EK::C>,\
                                                                                       typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, Object >,\
                                                                                                                Lazy_construction_object<Kernel,typename AK::C, typename EK::C>,\
                                                                                                                Lazy_construction<Kernel,typename AK::C, typename EK::C> >::type >::type > ::type > ::type C; \
    C Cf() const { return C(); }

  CGAL_Kernel_cons(Intersect_with_iterators_2,
		   intersect_with_iterators_2_object)
#else 
#define CGAL_Kernel_cons(C, Cf) \
    typedef typename boost::mpl::if_<boost::is_same<typename AK::C, typename AK::Construct_cartesian_const_iterator_2>, \
                                     Lazy_cartesian_const_iterator_2<Kernel,typename AK::C, typename EK::C>, \
                                     typename boost::mpl::if_<boost::is_same<typename AK::C, typename AK::Construct_cartesian_const_iterator_3>, \
                                                              Lazy_cartesian_const_iterator_3<Kernel,typename AK::C, typename EK::C>, \
                                                              typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, Bbox_2>, \
                                                                                       Lazy_construction_bbox<Kernel,typename AK::C, typename EK::C>, \
                                                                                       typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, Bbox_3>, \
                                                                                                                Lazy_construction_bbox<Kernel,typename AK::C, typename EK::C>, \
                                                                                                                typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, typename AK::FT>,\
                                                                                                                                         Lazy_construction_nt<Kernel,typename AK::C, typename EK::C>,\
                                                                                                                                         typename boost::mpl::if_<boost::is_same<typename AK::C::result_type, Object >,\
                                                                                                                                                                  Lazy_construction_object<Kernel,typename AK::C, typename EK::C>,\
                                                                                                                                                                  Lazy_construction<Kernel,typename AK::C, typename EK::C> >::type >::type >::type > ::type > ::type > ::type C; \
    C Cf() const { return C(); }

#endif //CGAL_INTERSECT_WITH_ITERATORS_2


#include <CGAL/Kernel/interface_macros.h>

};

template <class EK, class AK, class E2A>
struct Lazy_kernel_adaptor
  : public Lazy_kernel_base< EK, AK, E2A, Lazy_kernel_adaptor<EK,AK, E2A> >
{};

template <class EK, class AK, class E2A>
struct Lazy_kernel_without_type_equality
  : public Lazy_kernel_base< EK, AK, E2A, Lazy_kernel_without_type_equality<EK,AK, E2A> >
{};

template <class EK, class AK = Simple_cartesian<Interval_nt_advanced>,
                    class E2A = Cartesian_converter<EK, AK,
                                To_interval<typename EK::RT> > >
struct Lazy_kernel
  : public Type_equality_wrapper< 
             Lazy_kernel_base< EK, AK, E2A, Lazy_kernel<EK, AK, E2A> >,
             Lazy_kernel<EK, AK, E2A> >
{};

CGAL_END_NAMESPACE

#endif // CGAL_LAZY_KERNEL_H
