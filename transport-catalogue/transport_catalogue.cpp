#include "transport_catalogue.h"

void Catalogue::TransportCatalogue::AddStop(const std::string& name_stop, Coordinates coordinates)
{
    stops_.push_back(Stop{name_stop, coordinates});
    check_stop_[stops_.back().name_stop_] = &stops_.back();
}

void Catalogue::TransportCatalogue::AddDistanceForBus(std::vector<DistanceWithStops> name_stop_and_dist_next_stop)
{
        for(const DistanceWithStops& temp: name_stop_and_dist_next_stop)
        {
            for(auto name_stop_and_dist: temp.finish_stop_and_dist_)
            {
                std::string_view start_stop = check_stop_[temp.start_name_stop_]->name_stop_;
                std::string_view finish_stop = check_stop_[name_stop_and_dist.name_stop_]->name_stop_;
                std::pair<std::string_view,std::string_view> key = std::make_pair(start_stop, finish_stop);
                start_stop_to_finish_stop_and_dist_between_[key] = name_stop_and_dist.distance_to_next_;
            }
        }
}

void Catalogue::TransportCatalogue::AddBuses(std::vector<BusIncludeNameStops> names_and_routes, std::vector<Catalogue::TransportCatalogue::DistanceWithStops> name_stop_and_dist_next_stop)
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
    AddDistanceForBus(std::move(name_stop_and_dist_next_stop));
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

void Catalogue::TransportCatalogue::AddStopIncludeOtherRoutes(const std::vector<Stop*>& stops, std::string_view name_bus)
{
    for(Stop* stop: stops)
    {
        stop_enter_in_routes_[stop].insert(name_bus);
    }
}

std::set<std::string_view> Catalogue::TransportCatalogue::GetBusesEnterInRoute(const Stop* stop) const
{
    auto temp = stop_enter_in_routes_.find(stop);
    if(temp == stop_enter_in_routes_.end())
    {
        return {};
    }
    return temp->second;
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

size_t Catalogue::TransportCatalogue::GetDistanceBetweenStops(std::string_view start, std::string_view finish) const // Если не было указано расстояний, то x == y и y == x, поэтому проводим зеркальный поиск
{
    std::pair<std::string_view, std::string_view> first_check = std::make_pair(start,finish);
    std::pair<std::string_view, std::string_view> second_check = std::make_pair(finish, start);
    auto key_one = start_stop_to_finish_stop_and_dist_between_.find(first_check);
    auto key_second = start_stop_to_finish_stop_and_dist_between_.find(second_check);

    if(key_one != start_stop_to_finish_stop_and_dist_between_.end())
    {
        return start_stop_to_finish_stop_and_dist_between_.at(first_check);
    }
    else if(key_second != start_stop_to_finish_stop_and_dist_between_.end())
    {
        return start_stop_to_finish_stop_and_dist_between_.at(second_check);
    }
    return 0;
}

size_t Catalogue::TransportCatalogue::DistanceInRouteFromRequests(std::string_view request) const
{
    size_t result_dist = 0;
    for(size_t i = 0; i + 1 <  FindBus(request)->stops_in_route_.size(); ++i)
    {
        result_dist += GetDistanceBetweenStops(FindBus(request)->stops_in_route_[i]->name_stop_, FindBus(request)->stops_in_route_[i + 1]->name_stop_);
    }
    return result_dist;
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


Catalogue::TransportCatalogue::BusStatistics Catalogue::TransportCatalogue::GetBusStatistics(std::string_view request) const
{  
        if(FindBus(request) == nullptr)
        {
            return BusStatistics{request,0,0,0,0};
        }
        size_t count_stops = FindBus(request)->stops_in_route_.size();
        size_t uniq_stops = ComputeUniqStops(request);
        double distance = DistanceInRouteFromRequests(request);
        double curvature = distance / DistanceInRouteGeo(request);
        return BusStatistics{request,count_stops, uniq_stops, distance, curvature};
}

