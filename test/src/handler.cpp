
#include <iostream>
#include "handler/archivehandler.cpp"
#include "handler/defaulthandler.cpp"

// todo "default point to file"

void aaa()
{
	Handler* handler = new DefaultHandler(".");

	while ( handler->GetParent() != NULL )
	{
		handler->GetParent()->Traverse();
		std::cout << handler->GetNext() << std::endl;
		std::cout << handler->GetName() << '\n';
		handler = handler->GetParent();
	}
	std::cout << handler->GetName() << '\n';
	wxFileName dirname(".");
	std::cout << dirname.GetVolume() << '\n';
}

void testHandler( Handler* handler )
{
	std::cout << "Parent Name:\t" << handler->GetParent()->GetName() << '\n';
	handler->GetParent()->Traverse();
	std::cout << "Next : \t\t" << handler->GetNext() << '\n';
	std::cout << "Prev : \t\t" << handler->GetPrev() << '\n';
}
void dfHandler()
{
	testHandler( new DefaultHandler(".") );
}

void arcHandler()
{
	testHandler( new ArchiveHandler("./handler.zip") );
}

int main()
{
	dfHandler();
	arcHandler();
}
