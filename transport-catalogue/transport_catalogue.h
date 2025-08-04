#pragma once
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "domain.h"
#include "geo.h"

namespace Catalogue
{

class TransportCatalogue
{
private:

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string, std::pair<bool, bool>> check_roudtrip_bus_and_equal_first_last_stop; // Можно перенести в map_renderer, можно ли сказать, что это входит в бизнес-логику или нет, не знаю
    std::unordered_map<std::string_view, Stop*> check_stop_;
    std::unordered_map<std::string_view, Bus*> check_bus_;
    std::unordered_map<std::string_view, double> lenght_in_bus_;
    std::unordered_map<const Stop*,std::set<std::string>> stop_enter_in_routes_; // можно сделать string прибавим в скорости, единственно мы в памяти проигрываем
    std::unordered_map<std::string_view, double> bus_name_and_dist_;
    void AddStopIncludeOtherRoutes(const Stop* stop, const std::string& name_bus); // Добавил передачу по ссылке
    double DistanceInRouteGeo(std::string_view request) const;
    size_t ComputeUniqStops(std::string_view request) const;
    double GetDistanceFromRequests(std::string_view request) const;
    const std::unique_ptr<std::set<std::string>> GetBusesEnterInRoute(const Stop* stop) const;
public:
    const std::deque<Stop> &GetAllStops() const;
    const std::deque<Bus>& GetAllBuses() const;
    const std::unique_ptr<std::set<std::string>> GetBusesByStop(const std::string_view& stop_name) const;
    std::optional<BusStatistics> GetBusStatistics(const std::string_view& request) const;
    const std::unique_ptr<std::unordered_map<std::string, std::pair<bool, bool>>> GetRoudtripRoute() const;
    void SetRoundtripRoute(std::pair<std::string, bool> name_answer, bool first_last_stop);
    void AddStop(const std::string& name_stop, Coordinates coordinates);
    const Stop* FindStop(std::string_view name_stop) const;
    void AddBus(std::string name_bus, const std::vector<std::string> &route, double distance);
    const Bus* FindBus(std::string_view name_bus) const;
};

}
