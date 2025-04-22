#include "map_renderer.h"

// ======================= Установка параметров ==========================

void renderer::MapRenderer::SetSettingsMap(RenderOption settings)
{
    settings_renderrer_ = settings;
}

// ======================= Ф-и построения картинки ==========================

svg::Document renderer::MapRenderer::RenderMap(const Catalogue::TransportCatalogue& catalogue) const
{
    svg::Document doc;
    CoordinatesSphereProjectorAndAllNamesStops name_bus_project_sphere_coords_and_stop_plus_coord = SetCoordinatesForBusesSphereProjectorAndStopCoord(catalogue);
    if(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector.empty() || name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate.empty())
    {
        return doc;
    }
    for(const auto& element : GetSettingRenderRoute(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector))
    {
        doc.Add(element);
    }
    for(const auto& element : GetSettingNamesBuses(name_bus_project_sphere_coords_and_stop_plus_coord.coordinates_sphere_projector, *catalogue.GetRoudtripRoute()))
    {
        doc.Add(element.underlayer_bus);
        doc.Add(element.label_bus);
    }
    for(const auto& element : GetSettingsSymbolStops(name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate))
    {
        doc.Add(element);
    }
    for(const auto& element : GetSettingNamesStops(name_bus_project_sphere_coords_and_stop_plus_coord.name_stop_coordinate))
    {
        doc.Add(element.underlayer_stop);
        doc.Add(element.label_stop);
    }
    return doc;
}


std::vector<svg::Polyline> renderer::MapRenderer::GetSettingRenderRoute(const std::map<std::string, std::vector<svg::Point>>& sphere_coords) const
{
    std::vector<svg::Polyline> result;
    svg::Polyline temp;
    for(const auto& [name_bus, coords] : sphere_coords)
    {
        for(const svg::Point point : coords)
        {
            temp.AddPoint(point);
        }
        result.push_back(std::move(temp));
    }
    for(size_t i = 0, j = 0; i < result.size(); ++i, ++j) // Устанавливаем цвет
    {
        if(j >= settings_renderrer_.general_picture.color_palette_.size())
        {
            j = 0;
        }
        result[i].SetStrokeColor(settings_renderrer_.general_picture.color_palette_.at(j));
        result[i].SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor("none").SetStrokeWidth(settings_renderrer_.picture_bus.line_width_);
    }
    return result;
}

std::vector<renderer::PrinterNameBus> renderer::MapRenderer::GetSettingNamesBuses(const std::map<std::string, std::vector<svg::Point>>& sphere_coords, const std::unordered_map<std::string, std::pair<bool, bool>>& route_type) const
{
    std::vector<PrinterNameBus> result;
    size_t iterator_color = 0;
    for(const auto& [name_bus, coords] : sphere_coords)
    {
        if(coords.empty())
        {
            continue;
        }
        if(iterator_color >= settings_renderrer_.general_picture.color_palette_.size())
        {
            iterator_color = 0;
        }
        PrinterNameBus temp;
        temp.underlayer_bus.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND); // Доп параметры
        temp.underlayer_bus.SetStrokeColor(settings_renderrer_.general_picture.underlayer_color_).SetFillColor(settings_renderrer_.general_picture.underlayer_color_).SetStrokeWidth(settings_renderrer_.general_picture.underlayer_width_);

        temp.underlayer_bus.SetFontFamily("Verdana").SetFontSize(settings_renderrer_.picture_bus.font_size_).SetFontWeight("bold");
        temp.underlayer_bus.SetPosition(coords.front()).SetData(name_bus).SetOffset({settings_renderrer_.picture_bus.offset_x_, settings_renderrer_.picture_bus.offset_y_});
        temp.label_bus.SetFontFamily("Verdana").SetFontSize(settings_renderrer_.picture_bus.font_size_).SetFontWeight("bold");
        temp.label_bus.SetPosition(coords.front()).SetData(name_bus).SetOffset({settings_renderrer_.picture_bus.offset_x_, settings_renderrer_.picture_bus.offset_y_});

        temp.label_bus.SetFillColor(settings_renderrer_.general_picture.color_palette_.at(iterator_color)); // Доп параметр
        result.push_back(temp);
        if(route_type.find(name_bus) != route_type.end() && !(route_type.at(name_bus).first)) // Для некольцевого маршрута
        {
            if(route_type.at(name_bus).second)
            {
                ++iterator_color;
                continue;
            }
            temp.underlayer_bus.SetPosition(coords[coords.size() / 2]);
            temp.label_bus.SetPosition(coords[coords.size() / 2]);
            result.push_back(std::move(temp));
        }
        ++iterator_color;
    }
    return result;
}

std::vector<svg::Circle> renderer::MapRenderer::GetSettingsSymbolStops(const std::map<std::string, svg::Point>& name_stop_coordinate) const
{
    std::vector<svg::Circle> result;
    for(const auto& [name_stop, coordinate] : name_stop_coordinate)
    {
        svg::Circle temp;
        temp.SetCenter(coordinate).SetRadius(settings_renderrer_.picture_stop.radius_).SetFillColor("white");
        result.push_back(std::move(temp));
    }
    return result;
}

std::vector<renderer::PrinterNameStop> renderer::MapRenderer::GetSettingNamesStops(const std::map<std::string, svg::Point>& name_stop_coordinate) const
{
    std::vector<renderer::PrinterNameStop> result;
    PrinterNameStop temp;
    for(const auto& [name_stop, coord] : name_stop_coordinate)
    {
        temp.underlayer_stop.SetPosition(coord).SetOffset(svg::Point{settings_renderrer_.picture_stop.offset_x_, settings_renderrer_.picture_stop.offset_y_}); // Общие св-ва
        temp.underlayer_stop.SetFontSize(settings_renderrer_.picture_stop.font_size_).SetFontFamily("Verdana").SetData(name_stop);
        temp.label_stop.SetPosition(coord).SetOffset(svg::Point{settings_renderrer_.picture_stop.offset_x_, settings_renderrer_.picture_stop.offset_y_});
        temp.label_stop.SetFontSize(settings_renderrer_.picture_stop.font_size_).SetFontFamily("Verdana").SetData(name_stop);

        temp.underlayer_stop.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        temp.underlayer_stop.SetStrokeColor(settings_renderrer_.general_picture.underlayer_color_).SetFillColor(settings_renderrer_.general_picture.underlayer_color_).SetStrokeWidth(settings_renderrer_.general_picture.underlayer_width_);

        temp.label_stop.SetFillColor("black");
        result.push_back(std::move(temp));
    }
    return result;
}

// =========================== Вспомогательные ф-и ==============================

void renderer::MapRenderer::AddNameBusesForDraw(std::string name_bus)
{
    name_buses_.insert(std::move(name_bus));
}

bool renderer::IsZero(double value)
{
   return std::abs(value) < EPSILON;
}

svg::Point renderer::SphereProjector::operator()(Coordinates coords) const
{
    return {(coords.lng - min_lon_) * zoom_coeff_ + padding_,(max_lat_ - coords.lat) * zoom_coeff_ + padding_};
}

renderer::MapRenderer::CoordinatesSphereProjectorAndAllNamesStops renderer::MapRenderer::SetCoordinatesForBusesSphereProjectorAndStopCoord(const Catalogue::TransportCatalogue& catalogue) const
{
    CoordinatesSphereProjectorAndAllNamesStops result;
    if(name_buses_.empty())
    {
        return {};
    }
    std::deque<Coordinates> geo_coords;
    std::vector<svg::Point> sphere_coords;
    std::vector<size_t> count_stops;
    for(const auto& name_bus : name_buses_)
    {
        const Bus* bus = catalogue.FindBus(name_bus);
        count_stops.push_back(bus->stops_in_route_.size());
        for(const Stop* stop : bus->stops_in_route_)
        {
            geo_coords.push_back(stop->coordinates_);
        }
    }
    const renderer::SphereProjector proj(geo_coords.begin(), geo_coords.end(), settings_renderrer_.general_picture.width_, settings_renderrer_.general_picture.height_, settings_renderrer_.general_picture.padding_); // Задаем проекцию
    for(const auto& geo_coord : geo_coords)
    {
          const svg::Point screen_coord = proj(geo_coord);
          sphere_coords.push_back(screen_coord);
     }
        size_t iterator_count_stops = 0;
        size_t position = 0;
        for(const auto& name_bus : name_buses_)
        {
            std::vector<svg::Point> coords_for_add;
            const Bus* bus = catalogue.FindBus(name_bus);
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
