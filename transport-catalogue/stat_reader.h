#pragma once

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <string_view>
#include <unordered_set>

#include "geo.h"
#include "transport_catalogue.h"

namespace detail
{
class StatReader
{
public:
    void ParseAndAddRequest(std::string_view request);
    void PrintResults(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output) const;
private:
    void PrintFindStops(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const;
    void PrintFindBuses(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const;
    double DistanceInRoute(const Catalogue::TransportCatalogue& transport_catalogue, std::string_view request) const;
    size_t ComputeUniqStops(const Catalogue::TransportCatalogue& transport_catalogue, std::string_view request) const;
    void PrintOutBus(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double distance, std::ostream& output, size_t number_out) const;
    void PrintOutStop(const std::set<std::string_view> number_buses, std::string_view name_stop, std::ostream& output, size_t number_operation) const;
    std::deque<std::string> save_requests_;
};
}
