#include "domain.h"


Stop::Stop(const std::string& name_stop, const Coordinates& coordinates)
    :name_stop_(name_stop), coordinates_(coordinates)
{}

Stop::Stop(const Stop& other)
    : name_stop_(other.name_stop_), coordinates_(other.coordinates_)
{}

Stop::Stop(Stop&& other) noexcept
    : name_stop_(std::move(other.name_stop_)), coordinates_(std::move(other.coordinates_))
{}



Bus::Bus(Bus&& other) noexcept
    : name_bus_(std::move(other.name_bus_)), stops_in_route_(std::move(other.stops_in_route_))
{
    other.stops_in_route_.clear();
}

Bus::Bus(const std::string& name_bus, const std::vector<Stop*>& stops_in_route)
    : name_bus_(name_bus), stops_in_route_(stops_in_route)
{}



BusIncludeNameStops::BusIncludeNameStops(BusIncludeNameStops&& other) noexcept
: name_bus_(std::move(other.name_bus_)), route_(std::move(other.route_))
{}

BusIncludeNameStops::BusIncludeNameStops(std::string name_bus, std::vector<std::string> route) // Возможно лучше переделать const std::string& на string_view, объект все равно новый создасться (как в busstatistics
 : name_bus_(std::move(name_bus)), route_(std::move(route))
{}

BusIncludeNameStops::BusIncludeNameStops (const BusIncludeNameStops& other)
: name_bus_(other.name_bus_), route_(other.route_)
{}



BusStatistics::BusStatistics()
: name_bus_(""), count_stops_(0), uniq_stops_(0), length_(0), curvature_(0)
{}
BusStatistics::BusStatistics(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double length, double curvature)
: name_bus_(static_cast<std::string>(name_bus)), count_stops_(count_stops), uniq_stops_(uniq_stops), length_(length), curvature_(curvature)
{}
