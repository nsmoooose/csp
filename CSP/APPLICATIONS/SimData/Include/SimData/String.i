%module String
%{

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};

%}

%include "std_string.i"

class swig_string_bug_workaround {
public:
	swig_string_bug_workaround(std::string s) {
		get_as_swig_wrapped_pointer = s;
	}
	std::string get_as_swig_wrapped_pointer;
};


