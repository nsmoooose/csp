#include <SimData/Types.h>
#include <iostream>

using std::cout;
using std::endl;

using namespace simdata;

int main() {
	Vector3 x, y;
	Matrix3 a, b;
	a.FromAxisAngle(Vector3::ZAXIS, 0.3);
	cout << a.asString() << endl;
	b.FromAxisAngle(a * Vector3::YAXIS, 0.3);
	cout << b.asString() << endl;
	cout << (a*b).asString() << endl;
	x = Vector3::XAXIS;
	cout << x << endl;
	y = a  * x;
	cout << y << endl;
	return 0;
};

	
