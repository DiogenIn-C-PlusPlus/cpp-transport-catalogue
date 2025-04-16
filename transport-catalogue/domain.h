#pragma once

#include "geo.h"

#include <string>
#include <vector>


struct Stop
{
    Stop() = default;
    explicit Stop(const std::string& name_stop, const Coordinates& coordinates);
    explicit Stop(const Stop& other);
    explicit Stop(Stop&& other) noexcept;
    ~Stop() = default;

    std::string name_stop_;
    Coordinates coordinates_;
};

struct Bus
{
    Bus() = delete;
    explicit Bus(Bus&& other) noexcept;
    explicit Bus(const std::string& name_bus, const std::vector<Stop*>& stops_in_route);
    ~Bus() = default;

    std::string name_bus_;
    std::vector<Stop*> stops_in_route_;
};

struct BusIncludeNameStops
{
    BusIncludeNameStops() = default;
    explicit BusIncludeNameStops(BusIncludeNameStops&& other) noexcept;
    explicit BusIncludeNameStops(std::string name_bus, std::vector<std::string> route); // Возможно лучше переделать const std::string& на string_view, объект все равно новый создасться (как в busstatistics)
    BusIncludeNameStops (const BusIncludeNameStops& other);

    std::string name_bus_;
    std::vector<std::string> route_;
};

struct BusStatistics
{
        explicit BusStatistics();
        explicit BusStatistics(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double length, double curvature);

        std::string name_bus_;
        size_t count_stops_;
        size_t uniq_stops_;
        double length_;
        double curvature_;
};
