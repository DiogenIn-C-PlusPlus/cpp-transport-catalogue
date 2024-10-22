#pragma once

#include <string>
#include <string_view>
#include <vector>

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
};

class InputReader
{
public:
    void SetBaseRequest(std::istream& input, Catalogue::TransportCatalogue& catalogue);
private:
    void ParseLine(std::string_view line); // Парсит строку в структуру CommandDescription и сохраняет результат в commands_
    void ApplyCommands(Catalogue::TransportCatalogue& catalogue) const; // Связь между парсингом и справочником //  // Наполняет данными транспортный справочник, используя команды из commands_
    std::vector<CommandDescription> commands_;
};

}
