#pragma once

#include <algorithm>
#include <sstream>

#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace detail
{

class Json_reader
{
public:
    Json_reader() = delete; // В тестой версии можно убрать delete
    Json_reader(json::Document requests);

    void SetInputRequests(Catalogue::TransportCatalogue &catalogue, std::string key_input);
    void SetOutPutRequests(RequestHandler facade, std::string key_output ,std::ostream& output);
    void SetInputParametrsPicture(renderer::MapRenderer& map_renderer,  const std::string& key_input);
private:

    struct Key_words_stop
    {
      const std::string stop = "Stop";
      const std::string type = "type";
      const std::string name = "name";
      const std::string latitude = "latitude";
      const std::string longitude = "longitude";
      const std::string road_distances = "road_distances";
      const std::string id = "id";
    };

    struct Key_words_bus
    {
        const std::string bus = "Bus";
        const std::string type = "type";
        const std::string name = "name";
        const std::string stops = "stops";
        const std::string is_roundtrip = "is_roundtrip";
        const std::string id = "id";
    };

    struct Key_words_picture
    {
        const std::string width = "width";
        const std::string height = "height";
        const std::string padding = "padding";
        const std::string stop_radius = "stop_radius";
        const std::string line_width = "line_width";
        const std::string bus_label_font_size = "bus_label_font_size";
        const std::string bus_label_offset = "bus_label_offset";
        const std::string stop_label_font_size = "stop_label_font_size";
        const std::string stop_label_offset = "stop_label_offset";
        const std::string underlayer_color = "underlayer_color";
        const std::string underlayer_width = "underlayer_width";
        const std::string color_palette  = "color_palette";
        const std::string id = "id";
        const std::string map = "Map";
        const std::string type = "type";
    };

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
        int32_t id;
        std::string type;
        std::string name;
    };

//  -- Ввод
//protected: // Активно только в тестовом режиме программы
   std::unordered_map<std::pair<std::string, std::string>, double, HasherDistStop> dist_between_stops_; // string_view нельзя же, объекты инвалидируются
   std::vector<BusIncludeNameStops> names_and_routes_;

   void SetInputDataBase(const json::Array& array, Catalogue::TransportCatalogue& catalogue);
   void TakeStopsInfo(const json::Dict& information_about_stop, Catalogue::TransportCatalogue& catalogue);
   void TakeBusesInfo(const json::Dict& information_about_bus, Catalogue::TransportCatalogue& catalogue);
   double SetDistanceForBus(const BusIncludeNameStops& name_bus_and_route) const;
   void LoadParametrsPicture(const json::Dict& information_about_picture, renderer::MapRenderer &map_renderer);
// --

// -- Вывод
   std::deque<OutInfo> id_type_bus_or_stop_; // Есть один элемент для построения карты (ключ Map)

   void SetOutPutDataBase(const json::Array &array, const RequestHandler& fasad, std::ostream& output);
   void TakeStopInfoOut(const json::Dict& information_about_stop);
   void TakeBusInfoOut(const json::Dict& information_about_bus);
   void TakePictureInfoOut(const json::Dict& information_about_picture);
   void PrintResults(const RequestHandler& facade, std::ostream& out) const;
   void PrintOutBus(const RequestHandler& facade, const std::string& name_bus, int32_t id, std::ostream& out) const;
   void PrintOutStop(const RequestHandler& facade, const std::string& name_stop, int32_t id, std::ostream& out) const;
   void PrintOutPicture(const RequestHandler& facade, int32_t id, std::ostream& out) const;
   void PrintEmptyRequest(int32_t id, std::ostream& out) const;

   json::Document requests_;
   const Key_words_bus key_word_bus_;
   const Key_words_stop key_word_stop_;
   const Key_words_picture key_word_picture_;
};

} // detail
