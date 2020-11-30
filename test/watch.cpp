#include <wx/stopwatch.h>
#include <iostream>

int main ()
{
	wxStopWatch sw;
	std::cout << sw.Time() << std::endl;
	return 0;
}
