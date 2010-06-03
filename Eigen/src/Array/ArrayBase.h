// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2009 Gael Guennebaud <g.gael@free.fr>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_ARRAYBASE_H
#define EIGEN_ARRAYBASE_H

template<typename ExpressionType> class MatrixWrapper;

/** \ingroup Array_Module
  *
  * \class ArrayBase
  *
  * \brief Base class for all 1D and 2D array, and related expressions
  *
  * An array is similar to a dense vector or matrix. While matrices are mathematical
  * objects with well defined linear algebra operators, an array is just a collection
  * of scalar values arranged in a one or two dimensionnal fashion. As the main consequence,
  * all operations applied to an array are performed coefficient wise. Furthermore,
  * arrays support scalar math functions of the c++ standard library (e.g., std::sin(x)), and convenient
  * constructors allowing to easily write generic code working for both scalar values
  * and arrays.
  *
  * This class is the base that is inherited by all array expression types.
  *
  * \param Derived is the derived type, e.g., an array or an expression type.
  *
  * \sa class MatrixBase
  */
template<typename Derived> class ArrayBase
  : public DenseBase<Derived>
{
  public:
#ifndef EIGEN_PARSED_BY_DOXYGEN
    /** The base class for a given storage type. */
    typedef ArrayBase StorageBaseType;
    
    typedef ArrayBase Eigen_BaseClassForSpecializationOfGlobalMathFuncImpl;

    using ei_special_scalar_op_base<Derived,typename ei_traits<Derived>::Scalar,
                typename NumTraits<typename ei_traits<Derived>::Scalar>::Real>::operator*;

    typedef typename ei_traits<Derived>::StorageKind StorageKind;
    typedef typename ei_traits<Derived>::Index Index;
    typedef typename ei_traits<Derived>::Scalar Scalar;
    typedef typename ei_packet_traits<Scalar>::type PacketScalar;
    typedef typename NumTraits<Scalar>::Real RealScalar;

    typedef DenseBase<Derived> Base;
    using Base::RowsAtCompileTime;
    using Base::ColsAtCompileTime;
    using Base::SizeAtCompileTime;
    using Base::MaxRowsAtCompileTime;
    using Base::MaxColsAtCompileTime;
    using Base::MaxSizeAtCompileTime;
    using Base::IsVectorAtCompileTime;
    using Base::Flags;
    using Base::CoeffReadCost;

    using Base::derived;
    using Base::const_cast_derived;
    using Base::rows;
    using Base::cols;
    using Base::size;
    using Base::coeff;
    using Base::coeffRef;
    using Base::lazyAssign;
    using Base::operator=;
    using Base::operator+=;
    using Base::operator-=;
    using Base::operator*=;
    using Base::operator/=;

    typedef typename Base::CoeffReturnType CoeffReturnType;
#endif // not EIGEN_PARSED_BY_DOXYGEN

#ifndef EIGEN_PARSED_BY_DOXYGEN
    /** \internal the plain matrix type corresponding to this expression. Note that is not necessarily
      * exactly the return type of eval(): in the case of plain matrices, the return type of eval() is a const
      * reference to a matrix, not a matrix! It is however guaranteed that the return type of eval() is either
      * PlainObject or const PlainObject&.
      */
    typedef Array<typename ei_traits<Derived>::Scalar,
                ei_traits<Derived>::RowsAtCompileTime,
                ei_traits<Derived>::ColsAtCompileTime,
                AutoAlign | (ei_traits<Derived>::Flags&RowMajorBit ? RowMajor : ColMajor),
                ei_traits<Derived>::MaxRowsAtCompileTime,
                ei_traits<Derived>::MaxColsAtCompileTime
          > PlainObject;


    /** \internal Represents a matrix with all coefficients equal to one another*/
    typedef CwiseNullaryOp<ei_scalar_constant_op<Scalar>,Derived> ConstantReturnType;
#endif // not EIGEN_PARSED_BY_DOXYGEN

#define EIGEN_CURRENT_STORAGE_BASE_CLASS Eigen::ArrayBase
#   include "../plugins/CommonCwiseUnaryOps.h"
#   include "../plugins/MatrixCwiseUnaryOps.h"
#   include "../plugins/ArrayCwiseUnaryOps.h"
#   include "../plugins/CommonCwiseBinaryOps.h"
#   include "../plugins/MatrixCwiseBinaryOps.h"
#   include "../plugins/ArrayCwiseBinaryOps.h"
#   ifdef EIGEN_ARRAYBASE_PLUGIN
#     include EIGEN_ARRAYBASE_PLUGIN
#   endif
#undef EIGEN_CURRENT_STORAGE_BASE_CLASS

    /** Special case of the template operator=, in order to prevent the compiler
      * from generating a default operator= (issue hit with g++ 4.1)
      */
    Derived& operator=(const ArrayBase& other)
    {
      return ei_assign_selector<Derived,Derived>::run(derived(), other.derived());
    }

    Derived& operator+=(const Scalar& scalar)
    { return *this = derived() + scalar; }
    Derived& operator-=(const Scalar& scalar)
    { return *this = derived() - scalar; }

    template<typename OtherDerived>
    Derived& operator+=(const ArrayBase<OtherDerived>& other);
    template<typename OtherDerived>
    Derived& operator-=(const ArrayBase<OtherDerived>& other);

    template<typename OtherDerived>
    Derived& operator*=(const ArrayBase<OtherDerived>& other);

    template<typename OtherDerived>
    Derived& operator/=(const ArrayBase<OtherDerived>& other);

  public:
    ArrayBase<Derived>& array() { return *this; }
    const ArrayBase<Derived>& array() const { return *this; }

    MatrixWrapper<Derived> matrix() { return derived(); }
    const MatrixWrapper<Derived> matrix() const { return derived(); }

//     template<typename Dest>
//     inline void evalTo(Dest& dst) const { dst = matrix(); }

  protected:
    ArrayBase() : Base() {}

  private:
    explicit ArrayBase(Index);
    ArrayBase(Index,Index);
    template<typename OtherDerived> explicit ArrayBase(const ArrayBase<OtherDerived>&);
};

/** replaces \c *this by \c *this - \a other.
  *
  * \returns a reference to \c *this
  */
template<typename Derived>
template<typename OtherDerived>
EIGEN_STRONG_INLINE Derived &
ArrayBase<Derived>::operator-=(const ArrayBase<OtherDerived> &other)
{
  SelfCwiseBinaryOp<ei_scalar_difference_op<Scalar>, Derived> tmp(derived());
  tmp = other;
  return derived();
}

/** replaces \c *this by \c *this + \a other.
  *
  * \returns a reference to \c *this
  */
template<typename Derived>
template<typename OtherDerived>
EIGEN_STRONG_INLINE Derived &
ArrayBase<Derived>::operator+=(const ArrayBase<OtherDerived>& other)
{
  SelfCwiseBinaryOp<ei_scalar_sum_op<Scalar>, Derived> tmp(derived());
  tmp = other.derived();
  return derived();
}

/** replaces \c *this by \c *this * \a other coefficient wise.
  *
  * \returns a reference to \c *this
  */
template<typename Derived>
template<typename OtherDerived>
EIGEN_STRONG_INLINE Derived &
ArrayBase<Derived>::operator*=(const ArrayBase<OtherDerived>& other)
{
  SelfCwiseBinaryOp<ei_scalar_product_op<Scalar>, Derived> tmp(derived());
  tmp = other.derived();
  return derived();
}

/** replaces \c *this by \c *this / \a other coefficient wise.
  *
  * \returns a reference to \c *this
  */
template<typename Derived>
template<typename OtherDerived>
EIGEN_STRONG_INLINE Derived &
ArrayBase<Derived>::operator/=(const ArrayBase<OtherDerived>& other)
{
  SelfCwiseBinaryOp<ei_scalar_quotient_op<Scalar>, Derived> tmp(derived());
  tmp = other.derived();
  return derived();
}

#endif // EIGEN_ARRAYBASE_H
