#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

class C
{
public:
	template <class T>
	static string f(std::string x);
};

template <>
string C::f<int>(std::string x)
{
	cout << x << endl;
	return x;
}

template <>
string C::f<double>(std::string x)
{
	cout << stod(x) + 5 << endl;
	return x;
}

int main()
{
	C::f<int>("8");
	C::f<int>("7");
	C::f<double>("10");
	return 0;
}