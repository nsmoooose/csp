%module LUT
%{
#include "SimData/LUT.h"
%}

%include "std_vector.i"
%include "SimData/ns-simdata.h"

%include "std_vector.i"

#ifndef VFLOAT
#define VFLOAT
namespace std {
	%template(vector_t) vector<float>;
}
#endif

/* // already defined by List.i
#ifndef VINT
#define VINT
namespace std {
	%template(vector_i) vector<int>;
}
#endif
*/

%typemap(in) std::vector<std::vector<float> > const & (std::vector<std::vector<float> > temp) {
        if (PyTuple_Check($input) || PyList_Check($input)) {
            unsigned int size = (PyTuple_Check($input) ?  PyTuple_Size($input) : PyList_Size($input));
            temp.resize(size);
            $1 = &temp;
            for (unsigned int i=0; i<size; i++) {
                PyObject* o = PySequence_GetItem($input,i);
        	if (PyTuple_Check(o) || PyList_Check(o)) {
            		unsigned int size = (PyTuple_Check(o) ?  PyTuple_Size(o) : PyList_Size(o));
			temp[i].resize(size);
			for (unsigned int j=0; j<size; j++) {
                		PyObject* v = PySequence_GetItem(o,j);
                		if (SwigNumber_Check(v)) {
				    temp[i][j] = (float)(SwigNumber_AsDouble(v));
				    Py_DECREF(v);
				}else {
				    Py_DECREF(v);
				    PyErr_SetString(PyExc_TypeError, "vector<vector<float>> expected");
				    SWIG_fail;
				}
			}
			Py_DECREF(o);
		} else {
			Py_DECREF(o);
			PyErr_SetString(PyExc_TypeError, "vector<vector<float>> expected");
			SWIG_fail;
		}
		
            }
        } else {
            PyErr_SetString(PyExc_TypeError,"vector<vector<float>> expected");
            SWIG_fail;
        }
}

NAMESPACE_SIMDATA


class Interpolation: public BaseType {
	Interpolation();
public:
	typedef enum { LINEAR, SPLINE } Modes;
	bool isInterpolated() const;
};


template <typename X>
class InterpolationType: public Interpolation {
};

#define TRY	try { $action }
#define CATCH(a, b) \
	catch (a& e) {\
		std::cout << "EXCEPT\n"; \
		SWIG_exception(b, const_cast<char*>(e.getError().c_str()));\
	}
	
%include "exception.i"

%exception  {
	TRY
	CATCH(InterpolationIndex, SWIG_IndexError)
}

#undef TRY
#undef CATCH


template <int N, class X=float>
class LUT: public InterpolationType<X> {
	InterpolationType();
public:
	typedef VEC<N, int> Dim;
	typedef VEC<N, X> Vec;
	typedef VEC<N, std::vector<X> > Breaks;

	LUT();
	~LUT();

	%rename(__getitem__) getValue(std::vector<float> const &) const;
	X getValue(std::vector<float> const &) const;
	void load(std::vector<X> const &values, std::vector< std::vector<X> > const &breaks);
	void interpolate(std::vector<int> const &dim, Interpolation::Modes mode);
	virtual void pack(Packer& p) const;
	virtual void unpack(UnPacker& p);
	%rename(__repr__) asString() const;
	virtual std::string asString() const;
};



%template(InterpolationTypeFloat) InterpolationType<float>;
%template(Table1) LUT<1,float>;
%template(Table2) LUT<2,float>;
%template(Table3) LUT<3,float>;
%template(Table4) LUT<4,float>;


/*
%rename(__getitem__) LUT<1,float>::getValue(std::vector<float> const &) const;
%rename(__getitem__) Table1::getValue(std::vector<float> const &) const;
%rename(__getitem__) Table2::getValue(std::vector<float> const &) const;
%rename(__getitem__) Table3::getValue(std::vector<float> const &) const;
%rename(__getitem__) Table4::getValue(std::vector<float> const &) const;
%rename(__repr__) LUT<1,float>::asString() const;
%rename(__repr__) Table1::asString() const;
%rename(__repr__) Table2::asString() const;
%rename(__repr__) Table3::asString() const;
%rename(__repr__) Table4::asString() const;
*/

%exception;


NAMESPACE_END // simdata

