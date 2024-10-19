#include "transport_catalogue.h"

void Catalogue::TransportCatalogue::AddStop(const std::string& name_stop, Coordinates coordinates)
{
    stops_.push_back(Stop{name_stop, coordinates});
    check_stop_[stops_.back().name_stop_] = &stops_.back();
}

void Catalogue::TransportCatalogue::AddBus(const std::vector<std::pair<std::string, std::vector<std::string_view>>> &names_routes)
{
    for(const auto& element: names_routes)
    {
     Bus bus;
     bus.name_bus_ = element.first;
    for(std::string_view stop: element.second)
      {
        bus.stops_in_route_.push_back(check_stop_[stop]);
      }
    buses_.push_back(std::move(bus));
    AddStopIncludeOtherRoutes(buses_.back().stops_in_route_, buses_.back().name_bus_);
    check_bus_[buses_.back().name_bus_] = &buses_.back();
    }
}

const Catalogue::TransportCatalogue::Stop* Catalogue::TransportCatalogue::FindStop(std::string_view name_stop) const
{
    if(check_stop_.count(name_stop) == 0)
    {
        return nullptr;
    }
    return check_stop_.at(name_stop);
}

const Catalogue::TransportCatalogue::Bus* Catalogue::TransportCatalogue::FindBus(std::string_view name_bus) const
{
    if(check_bus_.count(name_bus) == 0)
    {
        return nullptr;
    }
    return check_bus_.at(name_bus);
}

void Catalogue::TransportCatalogue::AddStopIncludeOtherRoutes(const std::vector<Stop*>stops, std::string_view name_bus)
{
    for(Stop* stop: stops)
    {
        stop_enter_in_routes_[stop].insert(name_bus);
    }
}

std::set<std::string_view> Catalogue::TransportCatalogue::GetBusesEnterInRoute(const Stop *stop) const
{
    if(stop_enter_in_routes_.count(stop) == 0)
    {
        return {};
    }
    return stop_enter_in_routes_.at(stop);
}
