%include "std_vector.i"

#define new_vector(a, b)	\
	%template(v##a) std::vector< b >;

/*
#ifndef __VECTOR_TEMPLATES__
#define __VECTOR_TEMPLATES__
namespace std {
#	%template(vector_d) vector<double>;
	%template(vector_s) vector<Spread>;
#	%template(vector_f) vector<float>;
#	%template(vector_i) vector<int>;
}
#endif // __VECTOR_TEMPLATES__
*/
