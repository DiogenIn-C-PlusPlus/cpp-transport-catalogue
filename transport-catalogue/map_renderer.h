#pragma once
#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <unordered_map>

#include "geo.h"
#include "svg.h"
#include "json.h"
#include "domain.h"

namespace renderer
{

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector
{
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
SphereProjector(PointInputIt points_begin, PointInputIt points_end,double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end)
        {
            return;
        }
        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(points_begin, points_end,[](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end,[](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_))
        {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }
        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat))
        {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }
        if (width_zoom && height_zoom)
        {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom)
        {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom)
        {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }
    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(Coordinates coords) const;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};
// Имеет ли смысл все эти структуры перенести в domain? Чтобы не засорять данный файл или это не целесообразно, если некоторые используются только здесь?
struct PictureBus
{
    double line_width_ = 0;
    uint32_t font_size_;
    double offset_x_;
    double offset_y_;
};

struct PictureStop
{
    double line_width_ = 0;
    double radius_;
    uint32_t font_size_;
    double offset_x_;
    double offset_y_;
};

struct GeneralPicture
{
    double width_ = 0;
    double height_ = 0;
    double padding_= 0;
    double underlayer_width_;
    svg::Color underlayer_color_;
    std::vector<svg::Color> color_palette_;
};

struct RangeValue
{
    RangeValue()
        : start_other_parametrs(0), end_other_parametrs(100'000), start_offset(-100'000), end_offset(100'000)
    {}
    double start_other_parametrs;
    double end_other_parametrs;
    double start_offset;
    double end_offset;
};

struct PrinterNameBus
{
    svg::Text underlayer_bus;
    svg::Text label_bus;
};

struct PrinterNameStop
{
    svg::Text underlayer_stop;
    svg::Text label_stop;
};

class MapRenderer
{
public:
    MapRenderer() = default;
    void SetHeight(double height);
    void SetWidth(double width);
    void SetPanding(double panding);
    void SetRadius(double radius);
    void SetBusLineWidth(double line_width);
    void SetStopLineWidth(double line_width);
    void SetUnderlayerWidth(double underlayer_width);

    void SetStopLabelFontSize(int32_t font_size);
    void SetBusLabelFontSize(int32_t font_size);

    void SetStopLabelOffset(const json::Array& numbers_offset);
    void SetBusLabelOffset(const json::Array& numbers_offset);

    void SetUnderlayerColor(const json::Node &colors_parametrs);
    void SetColorPalette(const json::Node& colors_parametrs);

    double GetPanding() const;
    double GetHeight() const;
    double GetWidth() const;
    double GetStrokeWidthBuses() const;
    double GetStrokeWidthStops() const;
    double GetUnderlayerWidth() const;
    double GetRadius() const;
    uint32_t GetFontSizeStop() const;
    uint32_t GetFontSizeBus() const;
    std::pair<double, double> GetOffsetStopXandY() const;
    std::pair<double, double> GetOffsetBusXandY() const;
    svg::Color GetUnderlayerColor() const;
    std::vector<svg::Color> GetColorsPalette() const;

    // Рисование карты
    void AddNameBusesForDraw(std::string name_bus);
    const std::shared_ptr<std::set<std::string>> GetNameBusesForDraw() const;
    std::vector<svg::Polyline> GetSettingRenderRoute(const std::map<std::string, std::vector<svg::Point>>& sphere_coords) const;
    std::vector<PrinterNameBus> GetSettingNamesBuses(const std::map<std::string, std::vector<svg::Point>>& sphere_coords,  const std::unordered_map<std::string, std::pair<bool, bool>>& route_type) const;
    std::vector<svg::Circle> GetSettingsSymbolStops(const std::map<std::string, svg::Point>& name_stop_coordinate) const;
    std::vector<PrinterNameStop> GetSettingNamesStops(const std::map<std::string, svg::Point>& name_stop_coordinate) const;

private:
    bool CheckRangeValue(double number) const;

    std::set<std::string> name_buses_;
    const RangeValue range_value_{};
    PictureStop picture_stop_;
    PictureBus picture_bus_;
    GeneralPicture general_picture_;
};

}
