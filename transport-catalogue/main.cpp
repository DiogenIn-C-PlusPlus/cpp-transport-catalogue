#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

#include "tests.h"

// Если я не использую хэши так как каждый запрос уникален, это нормально или нет?

int main()
{
    Catalogue::TransportCatalogue catalogue;
    detail::InputReader inputreader;
    inputreader.SetBaseRequest(std::cin, catalogue);
    detail::StatReader outputreader;
    outputreader.OutBaseRequest(std::cin, std::cout, catalogue);
}
