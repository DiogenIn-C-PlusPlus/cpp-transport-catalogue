#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "geo.h"
#include "transport_catalogue.h"

namespace detail
{

struct CommandDescription 
{
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const
    {
        return !command.empty();
    }

    bool operator!() const
    {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
    std::string distance_to_stop; // Для дистанции до остановки
};

class InputReader
{
public:
    void SetBaseRequest(std::istream& input, Catalogue::TransportCatalogue& catalogue);
private:

    struct HasherDistStop
    {
       size_t operator()(const std::pair<std::string_view,std::string_view>& start_finish) const
       {
                size_t start = std::hash<std::string_view>{}(start_finish.first);
                size_t finish = std::hash<std::string_view>{}(start_finish.second);
                return start + 37 * finish;
        }
    };

    void ParseLine(std::string_view line); // Парсит строку в структуру CommandDescription и сохраняет результат в commands_
    void ApplyCommands(Catalogue::TransportCatalogue& catalogue) const; // Связь между парсингом и справочником //  // Наполняет данными транспортный справочник, используя команды из commands_
    double SetDistanceForBus(const Catalogue::TransportCatalogue::BusIncludeNameStops& name_bus_and_route, const std::unordered_map<std::pair<std::string, std::string>, double, HasherDistStop> &names_stops_and_dist_beetwen) const;
    std::vector<CommandDescription> commands_;
};

}
