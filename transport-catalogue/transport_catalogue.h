#pragma once
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#include "geo.h"

namespace Catalogue
{
//struct ConnectStopDist; // Для приватной области, чтобы можно было использовать словарь


class TransportCatalogue
{
private:

    struct Stop
    {
        Stop() = default;
        explicit Stop(const std::string& name_stop, const Coordinates& coordinates)
            :name_stop_(name_stop), coordinates_(coordinates)
        {}
        Stop(Stop&& other)
            : name_stop_(other.name_stop_), coordinates_(other.coordinates_)
        {}
        std::string name_stop_;
        Coordinates coordinates_;
    };

    struct Bus
    {
        std::string name_bus_;
        std::vector<Stop*> stops_in_route_;
    };

    struct HasherDistStop
    {
       size_t operator()(const std::pair<std::string_view,std::string_view>& start_finish) const
       {
                size_t start = std::hash<std::string_view>{}(start_finish.first);
                size_t finish = std::hash<std::string_view>{}(start_finish.second);
                return start + 37 * finish;
        }
    };

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> check_stop_;
    std::unordered_map<std::string_view, Bus*> check_bus_;
    std::unordered_map<std::string_view, double> lenght_in_bus_;
    std::unordered_map<const Stop*,std::set<std::string_view>> stop_enter_in_routes_; // можно сделать string прибавим в скорости (наверно)
    std::unordered_map<const std::pair<std::string_view,std::string_view>, size_t, HasherDistStop> start_stop_to_finish_stop_and_dist_between_;
    void AddStopIncludeOtherRoutes(const std::vector<Stop *>& stops, std::string_view name_bus); // Добавил передачу по ссылке
    double DistanceInRouteGeo(std::string_view request) const;
    size_t DistanceInRouteFromRequests(std::string_view request) const;
    size_t GetDistanceBetweenStops(std::string_view start, std::string_view finish) const;
    size_t ComputeUniqStops(std::string_view request) const;

public:

    struct FinishStopDist // Для парсинга расстояния из запроса
    {
        explicit FinishStopDist() = default;
        explicit FinishStopDist(const std::string_view name_stop , const double distance_to_next) // string -> string_view
            : name_stop_(name_stop) ,distance_to_next_(distance_to_next)
        {}
       std::string_view name_stop_;
       double distance_to_next_;
    };

    struct DistanceWithStops
    {
          explicit DistanceWithStops(DistanceWithStops&& other)
            : start_name_stop_(other.start_name_stop_), finish_stop_and_dist_(other.finish_stop_and_dist_)
        {}
          explicit DistanceWithStops(const std::string& name_stop, const std::vector<FinishStopDist>& stop_dist) // string -> string_view
              : start_name_stop_(static_cast<std::string>(name_stop)), finish_stop_and_dist_(std::move(stop_dist))
          {}
          std::string start_name_stop_;
          std::vector<FinishStopDist> finish_stop_and_dist_;
    };

    struct BusIncludeNameStops
    {
        explicit BusIncludeNameStops(BusIncludeNameStops&& other)
       : name_bus_(other.name_bus_), route_(other.route_)
        {}
        explicit BusIncludeNameStops(const std::string& name_bus,const std::vector<std::string_view>& route) // Возможно лучше переделать const std::string& на string_view, объект все равно новый создасться (как в busstatistics
            : name_bus_(name_bus), route_(std::move(route))
        {}  
        std::string name_bus_;
        std::vector<std::string_view> route_;
    };

    struct BusStatistics
    {
        explicit BusStatistics()
            : name_bus_(""), count_stops_(0), uniq_stops_(0), length_(0), curvature_(0)
        {}
        explicit BusStatistics(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double length, double curvature)
            : name_bus_(static_cast<std::string>(name_bus)), count_stops_(count_stops), uniq_stops_(uniq_stops), length_(length), curvature_(curvature)
        {}
        std::string name_bus_;
        size_t count_stops_;
        size_t uniq_stops_;
        double length_;
        double curvature_;
    };

    BusStatistics GetBusStatistics(std::string_view request) const;
    void AddStop(const std::string& name_stop, Coordinates coordinates);
    const Stop* FindStop(std::string_view name_stop) const;
    void AddBuses(std::vector<BusIncludeNameStops> names_and_routes, std::vector<DistanceWithStops> name_stop_and_dist_next_stop);
    const Bus* FindBus(std::string_view name_bus) const;
    std::set<std::string_view> GetBusesEnterInRoute(const Stop* stop) const;

private: // Как лучше сделать? Проблема если объявить выше в private будет конфликт, что нет объявления, а если объявить выше будет конфликт, что части находятся в private и public одновременно
       void AddDistanceForBus(std::vector<DistanceWithStops> name_stop_and_dist_next_stop);
};

}
