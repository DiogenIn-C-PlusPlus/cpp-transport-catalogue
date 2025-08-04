#pragma once

#include <algorithm>
#include <optional>
#include <sstream>

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

namespace detail
{

class JSONreader
{
public:
    JSONreader() = delete; // В тестой версии можно убрать delete
    JSONreader(json::Document requests);

    void SetInputRequests(Catalogue::TransportCatalogue &catalogue, const std::string& key_input);
    void SetOutputRequests(const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter& route,const std::string& key_output ,std::ostream& output);
    void SetInputParametersPicture(renderer::MapRenderer& map_renderer,  const std::string& key_input);
    void SetInputParametrsRoute(Route::TransportRouter &router, const std::string& key_input);
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

    struct OutInfo
    {
        explicit OutInfo() = default;
        explicit OutInfo(int32_t id_, std::string type_, std::string name_)
            : id(id_), type(type_), name(name_)
        {}
        explicit OutInfo(int32_t id_, std::string type_, std::string from_, std::string to_)
                : id(id_),
                  type(type_),
                  from(from_),
                  to(to_)
        {}
        int32_t id = 0;
        std::string type ="";
        std::string name = "";
        std::string from = "";
        std::string to = "";
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

//  -- Ввод
//protected: // Активно только в тестовом режиме программы
   std::unordered_map<std::pair<std::string, std::string>, double, HasherDistStop> dist_between_stops_; // string_view нельзя же, объекты инвалидируются
   std::vector<BusIncludeNameStops> names_and_routes_;
   const RangeValue range_value_{};

   void SetInputDataBase(const json::Array& array, Catalogue::TransportCatalogue& catalogue);
   void TakeStopsInfo(const json::Dict& information_about_stop, Catalogue::TransportCatalogue& catalogue);
   void TakeBusesInfo(const json::Dict& information_about_bus, Catalogue::TransportCatalogue& catalogue);
   double SetDistanceForBus(const BusIncludeNameStops& name_bus_and_route) const;
   void SetParametersPicture(const json::Dict& information_about_picture, renderer::MapRenderer &map_renderer);
   void SetPatametrsRoute(const json::Dict& information_about_picture, Route::TransportRouter& router);
   bool CheckRangeValue(double number) const;
   std::optional<std::pair<double, double>> SetStopLabelOffset(const json::Array& numbers_offset);
   std::optional<std::pair<double, double>> SetBusLabelOffset(const json::Array& numbers_offset);
   svg::Color SetUnderlayerColor(const json::Node &colors_parametrs);
   std::vector<svg::Color> SetColorPalette(const json::Node& colors_parametrs);
   // --

// -- Вывод  (Для хранения запросов на вывод существует альтернативный подход в хранении 2 векторов двух типов данных и сортировки по выводу id) вместо одного дека (один дек просто хранит лишние поля в структуре, вопрос выйгрыша зав. от запроса)
   std::deque<OutInfo> id_type_bus_or_stop_; // Есть один элемент для построения карты (ключ Map) (Новое добаленное - изменили добавили в струкруту эл-ты запроса route)
//   std::deque<OutInfoRoute> from_id_to_type_; // Новое добавленное (подлежит удалению)

   void SetOutPutDataBase(const json::Array &array, const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter& transport_route, std::ostream& output);
   void TakeStopInfoOut(const json::Dict& information_about_stop);
   void TakeBusInfoOut(const json::Dict& information_about_bus);
   void TakePictureInfoOut(const json::Dict& information_about_picture);
   void TakeRouteInfoOut(const json::Dict& information_about_route); // Новое добавлено
   void PrintResults(const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter &router, std::ostream& out) const;
   void PrintOutBus(const Catalogue::TransportCatalogue& catalogue, const std::string& name_bus, int32_t id, std::ostream& out) const;
   void PrintOutStop(const Catalogue::TransportCatalogue& catalogue, const std::string& name_stop, int32_t id, std::ostream& out) const;
   void PrintOutPicture(const renderer::MapRenderer& map_renderrer, const Catalogue::TransportCatalogue& catalogue, int32_t id, std::ostream& out) const;
   void PrintEmptyRequest(int32_t id, std::ostream& out) const;
   void PrintOutRouter(const Route::TransportRouter& route, const std::string& from_stop, const std::string& to_stop, int32_t id, std::ostream& out) const;
   json::Dict PrintItemsWait(const Route::RouteOutputData &data) const;
   json::Dict PrintItemsBus(const Route::RouteOutputData &data) const;

   json::Document requests_;
};

} // detail
