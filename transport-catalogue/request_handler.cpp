#include "request_handler.h"

RequestHandler::RequestHandler(const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer &map_render)
    : catalogue_(catalogue), renderer_(map_render)
{}

std::optional<BusStatistics> RequestHandler::GetBusStatistics(const std::string_view& request) const
{
    if(catalogue_.FindBus(request) == nullptr)
    {
        return std::nullopt;
    }
    size_t count_stops = catalogue_.FindBus(request)->stops_in_route_.size();
    size_t uniq_stops = catalogue_.ComputeUniqStops(request);
    double distance = catalogue_.GetDistanceFromRequests(request);
    double curvature = distance / catalogue_.DistanceInRouteGeo(request);
    return BusStatistics{request,count_stops, uniq_stops, distance, curvature};
}

const std::unique_ptr<std::set<std::string>> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
{
    if(catalogue_.FindStop(stop_name) == nullptr)
    {
        return nullptr;
    }
    else if(catalogue_.GetBusesEnterInRoute(catalogue_.FindStop(stop_name)) == nullptr)
    {
        std::set<std::string> empty_object{}; //  Насколько это нормально возвращаю указатель на временный объект
        return  std::make_unique<std::set<std::string>>(empty_object);
    }
    return catalogue_.GetBusesEnterInRoute(catalogue_.FindStop(stop_name));
}

svg::Document RequestHandler::RenderMap() const
{
    svg::Document doc;
    CoordinatesSphereProjectorAndAllNamesStops name_bus_project_sphere_coords_and_stop_plus_coord = SetCoordinatesForBusesSphereProjectorAndStopCoord();
    if(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector.empty() || name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate.empty())
    {
        return doc;
    }
    for(const auto& element : renderer_.GetSettingRenderRoute(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector))
    {
        doc.Add(element);
    }
    for(const auto& element : renderer_.GetSettingNamesBuses(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector, *catalogue_.GetRoudtripRoute()))
    {
        doc.Add(element.underlayer_bus);
        doc.Add(element.label_bus);
    }
    for(const auto& element : renderer_.GetSettingsSymbolStops(name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate))
    {
        doc.Add(element);
    }
    for(const auto& element : renderer_.GetSettingNamesStops(name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate))
    {
        doc.Add(element.underlayer_stop);
        doc.Add(element.label_stop);
    }
    return doc;
}

RequestHandler::CoordinatesSphereProjectorAndAllNamesStops RequestHandler::SetCoordinatesForBusesSphereProjectorAndStopCoord() const
{
    CoordinatesSphereProjectorAndAllNamesStops result;
    if(renderer_.GetNameBusesForDraw() == nullptr)
    {
        return {};
    }
    auto value = *renderer_.GetNameBusesForDraw();
    std::deque<Coordinates> geo_coords;
    std::vector<svg::Point> sphere_coords;
    std::vector<size_t> count_stops;
    for(const auto& name_bus : value)
    {
        const Bus* bus = catalogue_.FindBus(name_bus);
        count_stops.push_back(bus->stops_in_route_.size());
        for(const Stop* stop : bus->stops_in_route_)
        {
            geo_coords.push_back(stop->coordinates_);
        }
    }
    const renderer::SphereProjector proj(geo_coords.begin(), geo_coords.end(), renderer_.GetWidth(), renderer_.GetHeight(), renderer_.GetPanding()); // Задаем проекцию
    for(const auto& geo_coord : geo_coords)
    {
          const svg::Point screen_coord = proj(geo_coord);
          sphere_coords.push_back(screen_coord);
     }
        size_t iterator_count_stops = 0;
        size_t position = 0;
        for(const auto& name_bus : value)
        {
            std::vector<svg::Point> coords_for_add;
            const Bus* bus = catalogue_.FindBus(name_bus);
            for(size_t i = 0; i < count_stops[iterator_count_stops]; ++i, ++position)
            {
                result.name_stop_coordinate[bus->stops_in_route_.at(i)->name_stop_] = sphere_coords.at(position);
                coords_for_add.push_back(sphere_coords[position]);
            }
            result.coordinates_sphere_projector[name_bus] = coords_for_add;
            coords_for_add.clear();
            ++iterator_count_stops;
        }
    return result;
}
