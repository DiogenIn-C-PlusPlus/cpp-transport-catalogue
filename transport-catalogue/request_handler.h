#pragma once
#include <optional>

#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler
{
public:
    explicit RequestHandler(const Catalogue::TransportCatalogue& db, const renderer::MapRenderer& map_render);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStatistics> GetBusStatistics(const std::string_view& request) const; // Что лучше или передать просто string_view

    // Возвращает маршруты, проходящие через маршруты
    const std::unique_ptr<std::set<std::string>> GetBusesByStop(const std::string_view& stop_name) const;

    svg::Document RenderMap() const; // Воссоздается принцип шаблонного метода (данная ф-я определяет порядок построения картинки)

private:
    struct CoordinatesSphereProjectorAndAllNamesStops
    {
        std::map<std::string, std::vector<svg::Point>> coordinates_sphere_projector;
        std::map<std::string, svg::Point> name_stop_coordinate;
    };

    CoordinatesSphereProjectorAndAllNamesStops SetCoordinatesForBusesSphereProjectorAndStopCoord() const; // Сделан map вместо unordered в качестве изначального отсортированного списка log(N) не критично при вставке
    const Catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
};
