// Combat Simulator Project
// Copyright (C) 2003, 2004 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Vector.h
 *
 **/

/** @namespace numeric
 *  @brief Numerical integration and related utilities
 */

#ifndef __CSPSIM_NUMERIC_VECTOR_H__
#define __CSPSIM_NUMERIC_VECTOR_H__

#if !defined(NOT_USE_VALARRAY) && defined(_MSC_VER) && !defined(_STLP_WIN32)
#define NOT_USE_VALARRAY
#endif

#include <csp/csplib/util/Namespace.h>

#include <cmath>
#include <functional>

#ifdef NOT_USE_VALARRAY

#include <algorithm>
#include <numeric>
#include <vector>

CSP_NAMESPACE

namespace numeric {

template<typename T,class A> class Expr {
	A m_Iter;
	typedef typename std::vector<T>::size_type size_type;
	size_type m_Size;
public:
	Expr(const A& a):
	  m_Iter(a) {
	 }
	  Expr(const A& a, size_type s):
	  m_Iter(a),
	  m_Size(s) {
	 }
	 T const operator*() const {
		 return m_Iter();
	 }
	 Expr& operator++() {
		 ++m_Iter;
		 return *this;
	 }
	  const A& begin() const {
		 return m_Iter;
	 }
	  const size_type& size() const {
		 return m_Size;
	 }
};

template<typename T, class A, class B, class Op> class BinExprOp {
	A m_A;
	B m_B;
public:
	BinExprOp(const A& a,const B& b):
	  m_A(a),
	  m_B(b) {
	}
	T const operator()() const {
		return Op()(*m_A,*m_B);
	}
	BinExprOp& operator++() {
		++m_A;
		++m_B;
		return *this;
	}
};


template<typename T,class A, class Op> class UnaExprOp {
	T m_T;
	A m_A;
public:
	UnaExprOp(const T& t,const A& a):
	  m_T(t),
	  m_A(a) {
	}
	T const operator()() const {
		return Op()(m_T, *m_A);
	}
	UnaExprOp& operator++() {
		++m_A;
		return *this;
	}
};

/*
template<typename T,class A, class Op> class UnaExprOp {
	A m_A;
public:
	UnaExprOp(const A& a):
	  m_A(a) {
	}
	T const operator()() const {
		return Op()(*m_A);
	}
	UnaExprOp& operator++() {
		++m_A;
		return *this;
	}
};
*/
/**
* @warning This class is a first attempt to use a vector arithmetic; in no way, it 
* pretends to be optimized, nor fully complete.
* @warning Don t inheritate from this class.
* To be safe, this class should aggregate a std::vector<T> (or better a ref_count<T*>) 
* member and owns usefull std::vector<> methods.
*/
template<typename T> struct Vector: public std::vector<T> {
	typedef typename std::vector<T>::iterator vi;
	typedef typename std::vector<T>::const_iterator cvi;
	typedef typename std::vector<T>::size_type size_type;
	Vector() {
	}
	Vector(size_type n):std::vector<T>(n){
	}
	Vector(T value,size_type n):std::vector<T>(n,value){
	}
	template<class A> Vector(const Expr<T,A>& rhs):std::vector<T>(rhs.size()){
		*this = rhs;
	}
	template <class A> Vector& operator=(Expr<T,A> rhs) {
		cvi iEnd = end();
		for (vi i = begin();i != iEnd;++i,++rhs)
			*i = *rhs;
		return *this;
	}
	Expr<T,BinExprOp<T,cvi,cvi,std::plus<T> > > operator+(const Vector& rhs) const {
		typedef BinExprOp<T,cvi,cvi, std::plus<T> > ExprT;
		return Expr<T,ExprT>(ExprT(begin(),rhs.begin()),size());
	}
		
	template<class A> Expr<T,BinExprOp<T,cvi,Expr<T,A>,std::plus<T> > > operator+(const Expr<T,A>& rhs) const {
		typedef BinExprOp<T,cvi,Expr<T,A>,std::plus<T> > ExprT;
		return Expr<T,ExprT>(ExprT(begin(),rhs.begin()),size());
	}
	Vector operator-() const {
		Vector opp(size()); 
		std::transform(begin(),end(),opp.begin(),std::negate<T>());
		return opp;
	}
	Expr<T,BinExprOp<T,cvi,cvi,std::minus<T> > > operator-(const Vector& rhs) const {
		typedef BinExprOp<T,cvi,cvi,std::minus<T> > ExprT;
		return Expr<T,ExprT>(ExprT(begin(),rhs.begin()),size());
	}
	template<class A> Expr<T,BinExprOp<T,cvi,A,std::minus<T> > > operator-(const A& rhs) const {
		typedef BinExprOp<T,cvi,A,std::minus<T> > ExprT;
		return Expr<T,ExprT>(ExprT(begin(), rhs.begin()),size());
	}
	T operator*(const Vector& rhs) const {
		return std::inner_product(begin(),end(),rhs.begin(),0.0);
	}
};

template<typename T,class A> inline Expr<T,BinExprOp<T,A,typename Vector<T>::cvi,std::plus<T> > > 
operator+(const A& lhs, const Vector<T>& rhs) {
	typedef BinExprOp<T,A,typename Vector<T>::cvi,std::plus<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs.begin(),rhs.begin()),lhs.size());
}

template<typename T,class A,class B> inline Expr<T,BinExprOp<T,Expr<T,A>,Expr<T,B>,std::plus<T> > > 
operator+(const Expr<T,A>& lhs,const Expr<T,B>& rhs) {
	typedef BinExprOp<T,Expr<T,A>,Expr<T,B>,std::plus<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs.begin(),rhs.begin()),lhs.size());
}

template<typename T,class A> inline Expr<T,BinExprOp<T,A,typename Vector<T>::cvi,std::minus<T> > > 
operator-(const A& lhs,const Vector<T>& rhs) {
	typedef BinExprOp<T,A,typename Vector<T>::cvi,std::minus<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs.begin(),rhs.begin()),lhs.size());
}

template<typename T,class A, class B> inline Expr<T,BinExprOp<T,Expr<T,A>,Expr<T,B>,std::minus<T> > > 
operator-(const Expr<T,A>& lhs, const Expr<T,B>& rhs) {
	typedef BinExprOp<T,Expr<T,A>,Expr<T,B>,std::minus<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs.begin(),rhs.begin()),lhs.size());
}

template<typename T> inline Expr<T,UnaExprOp<T,typename Vector<T>::cvi,std::multiplies<T> > > operator*(T lhs,const Vector<T>& rhs) {
	typedef UnaExprOp<T,typename Vector<T>::cvi,std::multiplies<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs,rhs.begin()),rhs.size());
}
/*
template<typename T> inline Expr<T,UnaExprOp<T,typename Vector<T>::cvi,std::binder1st<std::multiplies<T> > > > operator*(T lhs,const Vector<T>& rhs) {
	typedef UnaExprOp<T,typename Vector<T>::cvi,std::bind1st<std::multiplies<T>,T>(std::multiplies<T>,lhs)> ExprT;
	return Expr<T,ExprT>(ExprT(lhs,rhs.begin()),rhs.size());
}
*/
template<typename T,class A> inline Expr<T,UnaExprOp<T,A,std::multiplies<T> > > operator*(T lhs, const A& rhs) {
	typedef UnaExprOp<T,A,std::multiplies<T> > ExprT;
	return Expr<T,ExprT>(ExprT(lhs,rhs.begin()),rhs.size());
}

template<typename T> class PrintElement {
	std::ostream& m_Ostream;
public:
	PrintElement(std::ostream& lhs):m_Ostream(lhs) {}
	void operator ()(T lhs){
		m_Ostream << "," << lhs;
	}
};

template<typename T> struct Abs {
	inline T operator()(T lhs,T rhs) const {
		return lhs + fabs(rhs);
	}
};

template<typename T> inline T norm_1(const Vector<T>& lhs) {
	return std::accumulate(lhs.begin(),lhs.end(),0.0,Abs<T>());
}

template<typename T, class A> inline T norm_1(const Expr<T,A>& lhs) {
	return norm_1(Vector<T>(lhs));
}

template<typename T, class A> inline T norm_2(const Expr<T,A>& lhs) {
	return norm_2(Vector<T>(lhs));
}

template<typename T> inline T norm_2(const Vector<T>& lhs) {
	return sqrt(lhs*lhs);
}

typedef Vector<double> Vectord;

} // namespace numeric

CSP_NAMESPACE_END

template<typename T> std::ostream &operator<<(std::ostream& lhs, const CSP(numeric::Vector<T>)& rhs) {
	if (!rhs.empty()) {
		typename CSP(numeric::Vector<T>)::cvi iBegin = rhs.begin();
		lhs << "(" << *iBegin;
		std::for_each(iBegin+1,rhs.end(),PrintElement<T>(lhs));
		lhs << ") ";
	}
	return lhs;
}

#else // NOT_USE_VALARRAY ifdef

#include <valarray>

CSP_NAMESPACE

namespace numeric {

template<typename T> T norm_2(const std::valarray<T>& lhs) {
	return sqrt((lhs*lhs).sum());
}

template<typename T> T norm_1(const std::valarray<T>& lhs) {
	return abs(lhs).sum();
}

typedef std::valarray<double> Vectord;

} // namespace numeric

CSP_NAMESPACE_END

template<typename T> std::ostream &operator<<(std::ostream& lhs, const std::valarray<T>& rhs) {
	size_t n = rhs.size();
	if (n>0) {
		lhs << "(" << rhs[0];
		for(size_t i = 1;i<n;++i) lhs << "," << rhs[i];
		lhs << ")";
	}
	return lhs;
}

#endif  // NOT_USE_VALARRAY else

#endif // __CSPSIM_NUMERIC_VECTOR_H__

