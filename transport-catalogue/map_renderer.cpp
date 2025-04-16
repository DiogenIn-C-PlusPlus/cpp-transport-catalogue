#include "map_renderer.h"

// ======================= Установка параметров ==========================

void renderer::MapRenderer::SetHeight(double height)
{
    if(CheckRangeValue(height))
    {
        return;
    }
    general_picture_.height_ = height;
}

void renderer::MapRenderer::SetWidth(double width)
{
    if(CheckRangeValue(width))
    {
        return;
    }
    general_picture_.width_ = width;
}

void renderer::MapRenderer::SetPanding(double panding)   // ---------
{
    if(panding < 0)
    {
        return;
    }
    general_picture_.padding_ = panding;
}

void renderer::MapRenderer::SetUnderlayerWidth(double underlayer_width)
{
    if(CheckRangeValue(underlayer_width))
    {
        return;
    }
    general_picture_.underlayer_width_ = underlayer_width;
}

void renderer::MapRenderer::SetRadius(double radius)
{
    if(CheckRangeValue(radius))
    {
        return;
    }
    picture_stop_.radius_ = radius;
}

void renderer::MapRenderer::SetBusLineWidth(double line_width)
{
    if(CheckRangeValue(line_width))
    {
        return;
    }
    picture_bus_.line_width_ = line_width;
}

void renderer::MapRenderer::SetStopLineWidth(double line_width)
{
    if(CheckRangeValue(line_width))
    {
        return;
    }
    picture_stop_.line_width_ = line_width;
}

void renderer::MapRenderer::SetStopLabelFontSize(int32_t font_size)
{
    if(CheckRangeValue(font_size))
    {
        return;
    }
    picture_stop_.font_size_ = font_size;
}

void renderer::MapRenderer::SetBusLabelFontSize(int32_t font_size)
{
    if(CheckRangeValue(font_size))
    {
        return;
    }
    picture_bus_.font_size_ = font_size;
}

void renderer::MapRenderer::SetBusLabelOffset(const json::Array& numbers_offset)
{
    uint16_t x = 0;
    uint16_t y = 1;
    if(numbers_offset.size() < 2 && (numbers_offset[x].AsDouble() < range_value_.start_offset && numbers_offset[y].AsDouble() > range_value_.end_offset))
    {
        return;
    }
    picture_bus_.offset_x_ = numbers_offset[x].AsDouble();
    picture_bus_.offset_y_ = numbers_offset[y].AsDouble();
}

void renderer::MapRenderer::SetStopLabelOffset(const json::Array& numbers_offset)
{
    uint16_t x = 0;
    uint16_t y = 1;
    if(numbers_offset.size() < 2 && (numbers_offset[x].AsDouble() < range_value_.start_offset && numbers_offset[y].AsDouble() > range_value_.end_offset))
    {
        return;
    }
    picture_stop_.offset_x_ = numbers_offset[x].AsDouble();
    picture_stop_.offset_y_ = numbers_offset[y].AsDouble();
}

void renderer::MapRenderer::SetUnderlayerColor(const json::Node& colors_parametrs)
{
    uint16_t elem_rgba_r = 0;
    uint16_t elem_rgba_g = 1;
    uint16_t elem_rgba_b = 2;
    uint16_t elem_rgba_a = 3;
    if(colors_parametrs.IsArray() && colors_parametrs.AsArray().size() == 3)
    {
        general_picture_.underlayer_color_ = svg::Color{svg::Rgb{static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_b].AsInt())}};
    }
    else if(colors_parametrs.IsArray() && colors_parametrs.AsArray().size() == 4)
    {
        general_picture_.underlayer_color_ = svg::Color{svg::Rgba{static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_b].AsInt()), colors_parametrs.AsArray()[elem_rgba_a].AsDouble()}};
    }
    else if(colors_parametrs.IsString())
    {
        general_picture_.underlayer_color_ = svg::Color{colors_parametrs.AsString()};
    }
    else
    {
        general_picture_.underlayer_color_ = svg::Color{};
    }
}

void renderer::MapRenderer::SetColorPalette(const json::Node &colors_parametrs)
{
    uint16_t elem_rgba_r = 0;
    uint16_t elem_rgba_g = 1;
    uint16_t elem_rgba_b = 2;
    uint16_t elem_rgba_a = 3;
    if(colors_parametrs.IsArray()) // массив цветов, либо один
    {
        for(const json::Node& element : colors_parametrs.AsArray())
        {
            if(element.IsString())
            {
                general_picture_.color_palette_.push_back(svg::Color{element.AsString()});
            }
            else if(element.AsArray().size() == 3)
            {
                general_picture_.color_palette_.push_back(svg::Color{svg::Rgb{static_cast<uint8_t>(element.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_b].AsInt())}});
            }
            else if (element.AsArray().size() == 4)
            {
                 general_picture_.color_palette_.push_back(svg::Color{svg::Rgba{static_cast<uint8_t>(element.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_b].AsInt()), element.AsArray()[elem_rgba_a].AsDouble()}});
            }
            else
            {
                general_picture_.color_palette_.push_back(svg::Color{});
            }
        }
    }
    else if(colors_parametrs.IsString()) // Один цвет строка
    {
        general_picture_.color_palette_.push_back(svg::Color{colors_parametrs.AsString()});
    }
}

// ======================= Возвращение параметров ==========================

double renderer::MapRenderer::GetPanding() const
{
    return general_picture_.padding_;
}

double renderer::MapRenderer::GetHeight() const
{
    return general_picture_.height_;
}

double renderer::MapRenderer::GetWidth() const
{
    return general_picture_.width_;
}

double renderer::MapRenderer::GetStrokeWidthBuses() const
{
    return picture_bus_.line_width_;
}

double renderer::MapRenderer::GetStrokeWidthStops() const
{
    return picture_stop_.line_width_;
}

std::vector<svg::Color> renderer::MapRenderer::GetColorsPalette() const
{
    return general_picture_.color_palette_;
}

double renderer::MapRenderer::GetUnderlayerWidth() const
{
    return general_picture_.underlayer_width_;
}

svg::Color renderer::MapRenderer::GetUnderlayerColor() const
{
    return general_picture_.underlayer_color_;
}

const std::shared_ptr<std::set<std::string>> renderer::MapRenderer::GetNameBusesForDraw() const
{
    return std::make_shared<std::set<std::string>>(name_buses_);
}

uint32_t renderer::MapRenderer::GetFontSizeBus() const
{
   return picture_bus_.font_size_;
}

uint32_t renderer::MapRenderer::GetFontSizeStop() const
{
   return picture_stop_.font_size_;
}

std::pair<double, double> renderer::MapRenderer::GetOffsetStopXandY() const
{
    return {picture_stop_.offset_x_, picture_stop_.offset_y_};
}

std::pair<double, double> renderer::MapRenderer::GetOffsetBusXandY() const
{
    return {picture_bus_.offset_x_, picture_bus_.offset_y_};
}

double renderer::MapRenderer::GetRadius() const
{
    return picture_stop_.radius_;
}



// ======================= Ф-и построения картинки ==========================

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
        if(j >= GetColorsPalette().size())
        {
            j = 0;
        }
        result[i].SetStrokeColor(GetColorsPalette().at(j));
        result[i].SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor("none").SetStrokeWidth(GetStrokeWidthBuses());
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
        if(iterator_color >= GetColorsPalette().size())
        {
            iterator_color = 0;
        }
        PrinterNameBus temp;
        temp.underlayer_bus.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND); // Доп параметры
        temp.underlayer_bus.SetStrokeColor(GetUnderlayerColor()).SetFillColor(GetUnderlayerColor()).SetStrokeWidth(GetUnderlayerWidth());

        temp.underlayer_bus.SetFontFamily("Verdana").SetFontSize(GetFontSizeBus()).SetFontWeight("bold");
        temp.underlayer_bus.SetPosition(coords.front()).SetData(name_bus).SetOffset({GetOffsetBusXandY().first, GetOffsetBusXandY().second});
        temp.label_bus.SetFontFamily("Verdana").SetFontSize(GetFontSizeBus()).SetFontWeight("bold");
        temp.label_bus.SetPosition(coords.front()).SetData(name_bus).SetOffset({GetOffsetBusXandY().first, GetOffsetBusXandY().second});

        temp.label_bus.SetFillColor(GetColorsPalette().at(iterator_color)); // Доп параметр
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
        temp.SetCenter(coordinate).SetRadius(GetRadius()).SetFillColor("white");
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
        temp.underlayer_stop.SetPosition(coord).SetOffset(svg::Point{GetOffsetStopXandY().first, GetOffsetStopXandY().second}); // Общие св-ва
        temp.underlayer_stop.SetFontSize(GetFontSizeStop()).SetFontFamily("Verdana").SetData(static_cast<std::string>(name_stop));
        temp.label_stop.SetPosition(coord).SetOffset(svg::Point{GetOffsetStopXandY().first, GetOffsetStopXandY().second});
        temp.label_stop.SetFontSize(GetFontSizeStop()).SetFontFamily("Verdana").SetData(static_cast<std::string>(name_stop));

        temp.underlayer_stop.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        temp.underlayer_stop.SetStrokeColor(GetUnderlayerColor()).SetFillColor(GetUnderlayerColor()).SetStrokeWidth(GetUnderlayerWidth());

        temp.label_stop.SetFillColor("black");
        result.push_back(std::move(temp));
    }
    return result;
}

// =========================== Вспомогательные ф-и ==============================

bool renderer::MapRenderer::CheckRangeValue(double number) const
{
    if((number < range_value_.start_other_parametrs) && (range_value_.end_other_parametrs - number < 0))
    {
        return true;
    }
    return false;
}

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
