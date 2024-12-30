#pragma once
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>


#include "geo.h"

namespace Catalogue
{

// Благодарю за ответы! :)

// Вроде подкорректировал, если что-то нужно, ещё подправлю

class TransportCatalogue
{
private:

    struct Stop
    {
        Stop() = delete;
        explicit Stop(const std::string& name_stop, const Coordinates& coordinates)
            :name_stop_(name_stop), coordinates_(coordinates)
        {}
        explicit Stop(const Stop& other)
            : name_stop_(other.name_stop_), coordinates_(other.coordinates_)
        {}
        explicit Stop(Stop&& other) noexcept
            : name_stop_(std::move(other.name_stop_)), coordinates_(std::move(other.coordinates_))
        {}
        std::string name_stop_;
        Coordinates coordinates_;
    };

    struct Bus
    {
        Bus() = delete;

        explicit Bus(Bus&& other) noexcept
            : name_bus_(std::move(other.name_bus_)), stops_in_route_(std::move(other.stops_in_route_))
        {
            other.stops_in_route_.clear();
        }
        explicit Bus(const std::string& name_bus, const std::vector<Stop*>& stops_in_route)
            : name_bus_(name_bus), stops_in_route_(stops_in_route)
        {}

        ~Bus() = default;

        std::string name_bus_;
        std::vector<Stop*> stops_in_route_;
    };

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> check_stop_;
    std::unordered_map<std::string_view, Bus*> check_bus_;
    std::unordered_map<std::string_view, double> lenght_in_bus_;
    std::unordered_map<const Stop*,std::set<std::string>> stop_enter_in_routes_; // можно сделать string прибавим в скорости, единственно мы в памяти проигрываем
    std::unordered_map<std::string_view, double> bus_name_and_dist_;
    void AddStopIncludeOtherRoutes(const Stop* stop, const std::string& name_bus); // Добавил передачу по ссылке
    double DistanceInRouteGeo(std::string_view request) const;
    size_t ComputeUniqStops(std::string_view request) const;

public:

    struct BusIncludeNameStops
    {
        explicit BusIncludeNameStops(BusIncludeNameStops&& other) noexcept
       : name_bus_(std::move(other.name_bus_)), route_(std::move(other.route_))
        {}

        explicit BusIncludeNameStops(std::string name_bus, std::vector<std::string_view> route) // Возможно лучше переделать const std::string& на string_view, объект все равно новый создасться (как в busstatistics
       : name_bus_(std::move(name_bus)), route_(std::move(route))
        {}

        BusIncludeNameStops (const BusIncludeNameStops& other)
       : name_bus_(other.name_bus_), route_(other.route_)
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
    void AddBus(std::string name_bus, const std::vector<std::string_view>& route, double distance);
    const Bus* FindBus(std::string_view name_bus) const;
    const std::unique_ptr<std::set<std::string>> GetBusesEnterInRoute(const Stop* stop) const; // Хотел через optional ссылку, потом подумал более громоздко решил через умный указатель - это хуже или лучше?
};

}
