#pragma once

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <string_view>

#include "geo.h"
#include "transport_catalogue.h"

namespace detail
{
class StatReader
{
public:
    void OutBaseRequest(std::istream& in, std::ostream& out,const Catalogue::TransportCatalogue& transport_catalogue);
private:
    void ParseAndAddRequest(std::string_view request);
    void PrintFindStops(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const;
    void PrintResults(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output) const;
    void PrintOutBus(Catalogue::TransportCatalogue::OutPutBus data_bus, std::ostream& output) const;
    void PrintOutStop(const std::set<std::string_view>& number_buses, std::string_view name_stop, std::ostream& output, size_t number_operation) const;
    std::deque<std::string> save_requests_;
};
}
