#pragma once
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

#include "geo.h"

namespace Catalogue
{
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

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> check_stop_;
    std::unordered_map<std::string_view, Bus*> check_bus_;
    std::unordered_map<const Stop*,std::set<std::string_view>> stop_enter_in_routes_; // можно сделать string прибавим в скорости (наверно)
    void AddStopIncludeOtherRoutes(const std::vector<Stop *> stops, std::string_view name_bus);
public:
    void AddStop(const std::string& name_stop, Coordinates coordinates);
    const Stop* FindStop(std::string_view name_stop) const;
    void AddBus(const std::vector<std::pair<std::string, std::vector<std::string_view>>>& names_routes);
    const Bus* FindBus(std::string_view name_bus) const;
    std::set<std::string_view> GetBusesEnterInRoute(const Stop* stop) const;
};
}
