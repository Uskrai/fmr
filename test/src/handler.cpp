
#include "handler/defaulthandler.h"
#include <iostream>

int aaa()
{
	Handler* handler = new DefaultHandler(".");

	while ( handler->GetParent() != NULL )
	{
		handler->GetParent()->Traverse();
		std::cout << handler->GetNext() << std::endl;
		std::cout << handler->GetCurrentName() << '\n';
		handler = handler->GetParent();
	}
	std::cout << handler->GetCurrentName() << '\n';
	wxFileName dirname(".");
	std::cout << dirname.GetVolume() << '\n';
}

int bbb()
{
	DefaultHandler* handler = new DefaultHandler(".");
	std::cout << handler->GetParentName() << '\n';
	handler->GetParent()->Traverse();
	std::cout << handler->GetNext() << '\n';
	std::cout << handler->GetPrev() << '\n';

	for ( auto& it : handler->GetParent()->GetAllName() )
	{
	//	std::cout << it << std::endl;
	}
}

int main()
{
	//DefaultHandler* handler = new DefaultHandler("./a.out");
	bbb();
}
