// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2015 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "parula.h"
#include "colormap.h"

template <typename T>
IGL_INLINE void igl::parula(const T x, T * rgb)
{
  igl::colormap(igl::COLOR_MAP_TYPE_PARULA,x, rgb);
}

template <typename T>
IGL_INLINE void igl::parula(const T f, T & r, T & g, T & b)
{
  igl::colormap(igl::COLOR_MAP_TYPE_PARULA, f, r, g, b);
}

template <typename DerivedZ, typename DerivedC>
IGL_INLINE void igl::parula(
  const Eigen::MatrixBase<DerivedZ> & Z,
  const bool normalize,
  Eigen::PlainObjectBase<DerivedC> & C)
{
  igl::colormap(igl::COLOR_MAP_TYPE_PARULA, Z, normalize, C);
}
template <typename DerivedZ, typename DerivedC>
IGL_INLINE void igl::parula(
  const Eigen::MatrixBase<DerivedZ> & Z,
  const double min_z,
  const double max_z,
  Eigen::PlainObjectBase<DerivedC> & C)
{
  igl::colormap(igl::COLOR_MAP_TYPE_PARULA, Z, min_z, max_z, C);
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template void igl::parula<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> > const&, double, double, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
// generated by autoexplicit.sh
template void igl::parula<Eigen::Array<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Array<int, -1, 1, 0, -1, 1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::parula<double>(double, double*);
template void igl::parula<double>(double, double&, double&, double&);
template void igl::parula<Eigen::Matrix<double, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::parula<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, bool, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template void igl::parula<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, double, double, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
#endif
