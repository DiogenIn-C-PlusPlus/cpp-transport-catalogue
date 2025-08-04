#include "transport_catalogue.h"
#include <iostream>

void Catalogue::TransportCatalogue::AddStop(const std::string& name_stop, Coordinates coordinates)
{
    stops_.push_back(Stop{name_stop, coordinates});
    check_stop_[stops_.back().name_stop_] = &stops_.back();
}

void Catalogue::TransportCatalogue::AddBus(std::string name_bus, const std::vector<std::string>& route, double distance)
{
     std::vector<Stop*> stops;
     for(std::string_view name_stop: route)
     {
         Stop* temp_stop = check_stop_[name_stop];
         stops.push_back(temp_stop);
         AddStopIncludeOtherRoutes(temp_stop, name_bus);
     }
     buses_.push_back(Bus{name_bus, stops});
     check_bus_[buses_.back().name_bus_] = &buses_.back();
     bus_name_and_dist_[buses_.back().name_bus_] = distance;
}

const Stop* Catalogue::TransportCatalogue::FindStop(std::string_view name_stop) const
{
    auto temp = check_stop_.find(name_stop);
    if(temp == check_stop_.end())
    {
        return nullptr;
    }
    return temp->second;
}

const Bus* Catalogue::TransportCatalogue::FindBus(std::string_view name_bus) const
{
    auto temp =check_bus_.find(name_bus);
    if(temp == check_bus_.end())
    {
        return nullptr;
    }
    return temp->second;
}

void Catalogue::TransportCatalogue::AddStopIncludeOtherRoutes(const Stop* stop, const std::string& name_bus)
{
        stop_enter_in_routes_[stop].insert(name_bus);
}

const std::unique_ptr<std::set<std::string>> Catalogue::TransportCatalogue::GetBusesEnterInRoute(const Stop* stop) const
{
    auto temp = stop_enter_in_routes_.find(stop);
    if(temp == stop_enter_in_routes_.end())
    {
        return nullptr;
    }
    return std::make_unique<std::set<std::string>>(temp->second);
}

const std::deque<Stop> &Catalogue::TransportCatalogue::GetAllStops() const
{
    return stops_;
}

double Catalogue::TransportCatalogue::DistanceInRouteGeo(std::string_view request) const
{
    double dist_in_route = 0;
    for(size_t i = 0; i +1 <  FindBus(request)->stops_in_route_.size(); ++i)
    {
         dist_in_route += ComputeDistance(FindBus(request)->stops_in_route_[i]->coordinates_, FindBus(request)->stops_in_route_[i+1]->coordinates_);
    }
    return dist_in_route;
}

size_t Catalogue::TransportCatalogue::ComputeUniqStops(std::string_view request) const
{
    std::unordered_set<std::string> result;
    for(const auto& stop: FindBus(request)->stops_in_route_)
    {
        result.insert(stop->name_stop_);
    }
    return result.size();
}

double Catalogue::TransportCatalogue::GetDistanceFromRequests(std::string_view request) const
{
    if(bus_name_and_dist_.find(request) == bus_name_and_dist_.end())
    {
        return -1;
    }
    return bus_name_and_dist_.at(request);
}

void Catalogue::TransportCatalogue::SetRoundtripRoute(std::pair<std::string, bool> name_answer, bool first_last_stop)
{
    check_roudtrip_bus_and_equal_first_last_stop[name_answer.first] = std::pair<bool, bool>(name_answer.second, first_last_stop);
}

const std::unique_ptr<std::unordered_map<std::string, std::pair<bool, bool>>> Catalogue::TransportCatalogue::GetRoudtripRoute() const
{
    if(check_roudtrip_bus_and_equal_first_last_stop.empty())
    {
        return nullptr;
    }
    return std::make_unique<std::unordered_map<std::string, std::pair<bool, bool>>>(check_roudtrip_bus_and_equal_first_last_stop);
}

std::optional<BusStatistics> Catalogue::TransportCatalogue::GetBusStatistics(const std::string_view& request) const
{
    if(FindBus(request) == nullptr)
    {
        return std::nullopt;
    }
    size_t count_stops = FindBus(request)->stops_in_route_.size();
    size_t uniq_stops = ComputeUniqStops(request);
    double distance = GetDistanceFromRequests(request);
    double curvature = distance / DistanceInRouteGeo(request);
    return BusStatistics{request,count_stops, uniq_stops, distance, curvature};
}

const std::unique_ptr<std::set<std::string>> Catalogue::TransportCatalogue::GetBusesByStop(const std::string_view& stop_name) const
{
    if(FindStop(stop_name) == nullptr)
    {
        return nullptr;
    }
    else if(GetBusesEnterInRoute(FindStop(stop_name)) == nullptr)
    {
        std::set<std::string> empty_object{}; //  Насколько это нормально возвращаю указатель на временный объект
        return  std::make_unique<std::set<std::string>>(empty_object);
    }
    return GetBusesEnterInRoute(FindStop(stop_name));
}

const std::deque<Bus>& Catalogue::TransportCatalogue::GetAllBuses() const
{
    return buses_;
}
