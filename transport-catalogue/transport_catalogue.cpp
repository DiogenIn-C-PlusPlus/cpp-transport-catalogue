#include "transport_catalogue.h"

void Catalogue::TransportCatalogue::AddStop(const std::string& name_stop, Coordinates coordinates)
{
    stops_.push_back(Stop{name_stop, coordinates});
    check_stop_[stops_.back().name_stop_] = &stops_.back();
}

void Catalogue::TransportCatalogue::AddBuses(std::vector<BusIncludeNameStops> names_and_routes)
{
    for(const auto& element: names_and_routes)
    {
     Bus bus;
     bus.name_bus_ = element.name_bus_;
    for(std::string_view stop: element.route_)
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
    auto temp = check_stop_.find(name_stop);
    if(temp == check_stop_.end())
    {
        return nullptr;
    }
    return temp->second;
}

const Catalogue::TransportCatalogue::Bus* Catalogue::TransportCatalogue::FindBus(std::string_view name_bus) const
{
    auto temp =check_bus_.find(name_bus);
    if(temp == check_bus_.end())
    {
        return nullptr;
    }
    return temp->second;
}

void Catalogue::TransportCatalogue::AddStopIncludeOtherRoutes(const std::vector<Stop*>stops, std::string_view name_bus)
{
    for(Stop* stop: stops)
    {
        stop_enter_in_routes_[stop].insert(name_bus);
    }
}
// Что значит вернуть константный указатель, просто сделать проверку на наличие stop, если нету nullptr, а если есть const Stop* вернуть? Просто смысл делать просто проверку на наличие в отдельном методе или не так понял?)
std::set<std::string_view> Catalogue::TransportCatalogue::GetBusesEnterInRoute(const Stop *stop) const
{
    auto temp = stop_enter_in_routes_.find(stop);
    if(temp == stop_enter_in_routes_.end())
    {
        return {};
    }
    return temp->second;
}

double Catalogue::TransportCatalogue::DistanceInRoute(std::string_view request) const
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


Catalogue::TransportCatalogue::OutPutBus Catalogue::TransportCatalogue::AllDataBus(std::string_view request) const
{
        if(FindBus(request) == nullptr)
        {
            return OutPutBus{};
        }
        size_t count_stops = FindBus(request)->stops_in_route_.size();
        size_t uniq_stops = ComputeUniqStops(request);
        double distance = DistanceInRoute(request);
        return OutPutBus{request,count_stops, uniq_stops, distance};
}
