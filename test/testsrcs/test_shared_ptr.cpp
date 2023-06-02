#include "utils/shared_ptr.hpp"
#include <cstdlib>

void checkleaks(void)
{
	system("leaks test_shared_ptr");
}

void testSimple(void)
{
	ft::shared_ptr<char> buf(new char[100]);
}

void testCopy(void)
{
	ft::shared_ptr<char> buf1(new char[100]);
	ft::shared_ptr<char> buf2(new char[100]);
	{
		{
			ft::shared_ptr<char> buf3(buf1);
			ft::shared_ptr<char> buf4(buf2);
		}
		ft::shared_ptr<char> buf5(buf2);
	}
	ft::shared_ptr<char> buf6(buf2);
	ft::shared_ptr<char> buf7(buf6);
}

void testAssignment(void)
{
	ft::shared_ptr<char> buf1(new char[100]);
	ft::shared_ptr<char> buf2(new char[100]);
	ft::shared_ptr<char> buf3(buf1);
	ft::shared_ptr<char> buf4(buf2);
	buf2 = buf1;
	buf3 = buf1;
	buf4 = buf2;
}

int main(void)
{
	atexit(checkleaks);

	testSimple();
	testCopy();
    testAssignment();

	return (0);
}
