src = ../../src/

handler: handler.cpp handler.zip $(addprefix $(src), handler/defaulthandler.cpp handler/archivehandler.cpp)
	g++ `wx-config --cxxflags --libs` --std=c++17 -I ../../src handler.cpp -o $@

handler.zip: handler.cpp
	zip $@ $^
