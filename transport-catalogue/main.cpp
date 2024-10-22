#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;



int main()
{
    Catalogue::TransportCatalogue catalogue;
    detail::InputReader inputreader;
    inputreader.SetBaseRequest(std::cin, catalogue);
    detail::StatReader outputreader;
    outputreader.OutBaseRequest(std::cin, std::cout, catalogue);
}
