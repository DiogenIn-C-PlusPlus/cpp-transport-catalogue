#include "json_reader.h"

// ==== Вход в обработку


// Перенес в виде константы статической, мне компилятор выдает предупреждение non-POD, я прочитал это больше относится к библиотекам
// мол, что это может привести к лишней трате ресурсов и при этом не использовано (Дают совет, просто изменить настройки компилятора)


  static const std::string key_base = "base_requests";
  static const std::string key_statats = "stat_requests";
  static const std::string key_picture = "render_settings";
  static const std::string key_router = "routing_settings";
  static const std::string stop = "Stop";
  static const std::string type = "type";
  static const std::string name = "name";
  static const std::string latitude = "latitude";
  static const std::string longitude = "longitude";
  static const std::string road_distances = "road_distances";
  static const std::string id = "id";
  static const std::string bus = "Bus";
  static const std::string stops = "stops";
  static const std::string is_roundtrip = "is_roundtrip";
  static const std::string width = "width";
  static const std::string height = "height";
  static const std::string padding = "padding";
  static const std::string stop_radius = "stop_radius";
  static const std::string line_width = "line_width";
  static const std::string bus_label_font_size = "bus_label_font_size";
  static const std::string bus_label_offset = "bus_label_offset";
  static const std::string stop_label_font_size = "stop_label_font_size";
  static const std::string stop_label_offset = "stop_label_offset";
  static const std::string underlayer_color = "underlayer_color";
  static const std::string underlayer_width = "underlayer_width";
  static const std::string color_palette  = "color_palette";
  static const std::string map = "Map";
  static const std::string route = "Route";
  static const std::string from = "from";
  static const std::string to = "to";
  static const std::string bus_wait_time = "bus_wait_time";
  static const std::string bus_velocity = "bus_velocity";

detail::JSONreader::JSONreader(json::Document doc)
    : requests_(doc.GetRoot())
    {}


void detail::JSONreader::SetInputRequests(Catalogue::TransportCatalogue& catalogue, const std::string& key_input)
{
    try
    {
        if(key_input != key_base)
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" == Address: JSONreader -> SetInputRequests" <<std::endl;
    }
    SetInputDataBase(requests_.GetRoot().AsMap().at(key_input).AsArray(), catalogue);
}

void detail::JSONreader::SetOutputRequests(const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter& route, const std::string& key_output, std::ostream &output)
{
    try
    {
        if(key_output != key_statats)
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" == Address: JSONreader -> SetOutPutRequests" <<std::endl;
    }
    SetOutPutDataBase(requests_.GetRoot().AsMap().at(key_output).AsArray(), catalogue , map_renderrer, route, output);
}

void detail::JSONreader::SetInputParametersPicture(renderer::MapRenderer& map_renderer, const std::string& key_input)
{
    try
    {
        if(key_input != key_picture)
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" or \"render_settings\" == Address: JSONreader -> SetInputParametrsPicture" << std::endl;
    }
    SetParametersPicture(requests_.GetRoot().AsMap().at(key_input).AsMap(), map_renderer);
}

void detail::JSONreader::SetInputParametrsRoute(Route::TransportRouter& router, const std::string& key_input)
{
    try
    {
        if(key_input != key_router)
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" or \"key_router\" == Address: JSONreader -> SetInputParametrsRoute" << std::endl;
    }
    SetPatametrsRoute(requests_.GetRoot().AsMap().at(key_input).AsMap(), router);
}

// =================  Ввод =========================

void detail::JSONreader::SetInputDataBase(const json::Array& array, Catalogue::TransportCatalogue &catalogue) // Можно попробовать через move (нужен один раз же)
{
    for(const json::Node& element : array)
    {
        try
        {
            json::Dict bus_or_stop = element.AsMap();
            if(bus_or_stop.find(type) != bus_or_stop.end() && bus_or_stop.at(type) == bus) // Нужно смотреть, что кидает at и делать try catch
            {
                TakeBusesInfo(bus_or_stop, catalogue); // Думаем насчет move
            }
            else if(bus_or_stop.find(type) != bus_or_stop.end() && bus_or_stop.at(type) == stop)
            {
                TakeStopsInfo(bus_or_stop, catalogue);  // Думаем насчет move
            }
        }
        catch (std::out_of_range& error)
        {
             std::cout << error.what() << "In input data have place where key type or key words Bus and Stop abcent";
             std::cout << "Look input data file. Adress: JSONreader -> SetInputDataBase" << std::endl;
        } catch (...)
        {
            std::cout << "Unknown error === Address: JSONreader -> SetInputDataBase";
        }
    }
    for(const auto& name_bus_and_route : names_and_routes_)
    {
        catalogue.AddBus(std::move(name_bus_and_route.name_bus_), name_bus_and_route.route_, SetDistanceForBus(name_bus_and_route)); // move? Возможно там правда, если удалиться раньше, то у другой ф-и будет пусто
    }
    // names_and_routes_.clear(); // используем для map_render
    dist_between_stops_.clear(); // Очищаем, чтобы не занимать память - больше не понадобиться
}

void detail::JSONreader::TakeStopsInfo(const json::Dict& information_about_stop, Catalogue::TransportCatalogue& catalogue)
{
    Stop stop;
    for(const auto& [key_word, Node] : information_about_stop)
    {
        if(type == key_word)
        {
            continue;
        }
        else if(name == key_word)
        {
            stop.name_stop_ = Node.AsString();
        }
        else if(latitude == key_word)
        {
            stop.coordinates_.lat = Node.AsDouble();
        }
        else if(longitude == key_word)
        {
            stop.coordinates_.lng = Node.AsDouble();
        }
        else if(road_distances == key_word)
        {
            for(const auto& [name_stop, dist] : Node.AsMap())
            {
                dist_between_stops_[std::make_pair(stop.name_stop_, name_stop)] = dist.AsInt();
            }
        }
        else
        {
            try
            {
                throw std::logic_error("Not right structure in parsing Stop");
            }  catch (std::logic_error& error)
            {
                std::cout << error.what() << "Look in data structure Stop. Address: JSONreader -> TakeStopInfo" << std::endl;
            }
            catch (...)
            {
                std::cout << "Unknown error === Address: JSONreader -> TakeStopInfo";
            }
        }
    }
    catalogue.AddStop(stop.name_stop_, stop.coordinates_); // std::move?
}

void detail::JSONreader::TakeBusesInfo(const json::Dict& information_about_bus, Catalogue::TransportCatalogue& catalogue)
{
   BusIncludeNameStops temp;
   bool last_stop_equal_first = false;
   for(const auto& [key_word, Node] : information_about_bus)
   {
       if(type == key_word)
       {
           continue;
       }
       else if(is_roundtrip == key_word)
       {
           continue;
       }
       else if(name == key_word)
       {
            temp.name_bus_ = Node.AsString();
       }
       else if(stops == key_word)
       {
           for(const auto& name_stops : Node.AsArray())
           {
               temp.route_.push_back(name_stops.AsString()); // move?
           }
           if(!information_about_bus.at(is_roundtrip).AsBool()) // По идее если не кольцевой то добавляем
           {
               last_stop_equal_first = temp.route_.back() == temp.route_.front();
                for(auto it = Node.AsArray().rbegin() + 1;  it != Node.AsArray().rend(); ++it)
                {
                    temp.route_.push_back(it->AsString());
                }
           }
       }
       else
       {
           try
           {
               throw std::logic_error("Not right structure in parsing Bus");
           }  catch (std::logic_error& error)
           {
               std::cout << error.what() << "Look in data structure Bus. Address: JSONreader -> TakeBusInfo" << std::endl;
           }
           catch (...)
           {
                std::cout << "Unknown error === Address: JSONreader -> TakeBusInfo";
           }
       }
   }
   catalogue.SetRoundtripRoute({temp.name_bus_, information_about_bus.at(is_roundtrip).AsBool()}, last_stop_equal_first);
   names_and_routes_.push_back(std::move(temp));
}

double detail::JSONreader::SetDistanceForBus(const BusIncludeNameStops& name_bus_and_route) const
{
    double result = 0;
    for(size_t i = 0; i + 1 < name_bus_and_route.route_.size(); ++i)
    {
         std::string start_stop = static_cast<std::string>(name_bus_and_route.route_[i]);
         std::string finish_stop = static_cast<std::string>(name_bus_and_route.route_[i + 1]);
         std::pair<std::string, std::string> check_first = std::make_pair(start_stop, finish_stop);
         std::pair<std::string, std::string> check_second = std::make_pair(finish_stop, start_stop);
         auto key_first = dist_between_stops_.find(check_first);
         auto key_second = dist_between_stops_.find(check_second);
         if(key_first != dist_between_stops_.end())
         {
             result += dist_between_stops_.at(check_first);
             continue;
         }
         if(key_second != dist_between_stops_.end())
         {
             result += dist_between_stops_.at(check_second);
             continue;
         }
    }
    return result;
}

void detail::JSONreader::SetParametersPicture(const json::Dict& information_about_picture, renderer::MapRenderer& map_renderer)
{
    renderer::RenderOptions renderrer_settings;
    for(const auto& [key_word, value] : information_about_picture)
    {
            if(key_word == width)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                    renderrer_settings.general_picture.width_ = value.AsDouble();
                }
            }
            else if(key_word == height)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                    renderrer_settings.general_picture.height_ = value.AsDouble();
                }
            }
            else if (key_word == padding)
            {
                if(value.AsDouble() > 0)
                {
                   renderrer_settings.general_picture.padding_ = value.AsDouble();
                }
            }
            else if(key_word == line_width)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                    renderrer_settings.picture_bus.line_width_ = value.AsDouble();
                    renderrer_settings.picture_stop.line_width_ = value.AsDouble();
                }
            }
            else if(key_word == stop_radius)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                    renderrer_settings.picture_stop.radius_ = value.AsDouble();
                }
            }
            else if(key_word == underlayer_width)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                   renderrer_settings.general_picture.underlayer_width_ = value.AsDouble();
                }
            }
            else if(key_word == bus_label_font_size)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                   renderrer_settings.picture_bus.font_size_ = value.AsInt();
                }
            }
            else if(key_word == stop_label_font_size)
            {
                if(!CheckRangeValue(value.AsDouble()))
                {
                   renderrer_settings.picture_stop.font_size_ = value.AsInt();
                }
            }
            else if(key_word == bus_label_offset)
            {
                if(SetBusLabelOffset(value.AsArray()).has_value())
                {
                    renderrer_settings.picture_bus.offset_x_ = SetBusLabelOffset(value.AsArray()).value().first;
                    renderrer_settings.picture_bus.offset_y_ = SetBusLabelOffset(value.AsArray()).value().second;
                }
            }
            else if(key_word == stop_label_offset)
            {
                if(SetStopLabelOffset(value.AsArray()).has_value())
                {
                    renderrer_settings.picture_stop.offset_x_ = SetStopLabelOffset(value.AsArray()).value().first;
                    renderrer_settings.picture_stop.offset_y_ = SetStopLabelOffset(value.AsArray()).value().second;
                }
            }
            else if(key_word == underlayer_color)
            {
                renderrer_settings.general_picture.underlayer_color_ = SetUnderlayerColor(value);
            }
            else if(key_word == color_palette)
            {
                 renderrer_settings.general_picture.color_palette_ = SetColorPalette(value);
            }
    }
    if(renderrer_settings.general_picture.padding_ > std::min(renderrer_settings.general_picture.height_, renderrer_settings.general_picture.width_) / 2) // Обработка Painding связано с тем, что инициализация происходит не по порядку
    {
         renderrer_settings.general_picture.padding_ = std::min(renderrer_settings.general_picture.height_, renderrer_settings.general_picture.width_) / 2;
    }
    for(auto element : names_and_routes_) // Как лучше перебирать в цикле если делаем move по ссылке или нет
    {
        map_renderer.AddNameBusesForDraw(std::move(element.name_bus_));

    }
    map_renderer.SetSettingsMap(std::move(renderrer_settings));
    names_and_routes_.clear(); // Освобождаем за ненадобностью
}

void detail::JSONreader::SetPatametrsRoute(const json::Dict& information_about_picture, Route::TransportRouter& router)
{
    Route::RouteOptions options;
    for(const auto& [key_word, value] : information_about_picture)
    {
        if(key_word == bus_wait_time)
        {
            options.bus_time_wait = value.AsInt();
        }
        else if(key_word == bus_velocity)
        {
            options.bus_velocity = value.AsDouble();
        }
    }
    router.SetParametrsBusWaitTimeAndVelocity(options.bus_time_wait, options.bus_velocity);
    router.BuildGraph();
}

std::optional<std::pair<double, double>> detail::JSONreader::SetBusLabelOffset(const json::Array& numbers_offset)
{
    std::pair<double, double> result;
    uint16_t x = 0;
    uint16_t y = 1;
    if(numbers_offset.size() < 2 && (numbers_offset[x].AsDouble() < range_value_.start_offset && numbers_offset[y].AsDouble() > range_value_.end_offset))
    {
        return std::nullopt;
    }
    result.first = numbers_offset[x].AsDouble();
    result.second = numbers_offset[y].AsDouble();
    return result;
}

std::optional<std::pair<double, double>> detail::JSONreader::SetStopLabelOffset(const json::Array& numbers_offset)
{
    std::pair<double, double> result;
    uint16_t x = 0;
    uint16_t y = 1;
    if(numbers_offset.size() < 2 && (numbers_offset[x].AsDouble() < range_value_.start_offset && numbers_offset[y].AsDouble() > range_value_.end_offset))
    {
        return std::nullopt;
    }
    result.first = numbers_offset[x].AsDouble();
    result.second = numbers_offset[y].AsDouble();
    return result;
}

svg::Color detail::JSONreader::SetUnderlayerColor(const json::Node& colors_parametrs)
{
    uint16_t elem_rgba_r = 0;
    uint16_t elem_rgba_g = 1;
    uint16_t elem_rgba_b = 2;
    uint16_t elem_rgba_a = 3;
    if(colors_parametrs.IsArray() && colors_parametrs.AsArray().size() == 3)
    {
        return svg::Color{svg::Rgb{static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_b].AsInt())}};
    }
    else if(colors_parametrs.IsArray() && colors_parametrs.AsArray().size() == 4)
    {
        return svg::Color{svg::Rgba{static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(colors_parametrs.AsArray()[elem_rgba_b].AsInt()), colors_parametrs.AsArray()[elem_rgba_a].AsDouble()}};
    }
    else if(colors_parametrs.IsString())
    {
        return svg::Color{colors_parametrs.AsString()};
    }
    else
    {
        return svg::Color{};
    }
}

std::vector<svg::Color> detail::JSONreader::SetColorPalette(const json::Node &colors_parametrs)
{
    std::vector<svg::Color> result;
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
                result.push_back(svg::Color{element.AsString()});
            }
            else if(element.AsArray().size() == 3)
            {
                result.push_back(svg::Color{svg::Rgb{static_cast<uint8_t>(element.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_b].AsInt())}});
            }
            else if (element.AsArray().size() == 4)
            {
                 result.push_back(svg::Color{svg::Rgba{static_cast<uint8_t>(element.AsArray()[elem_rgba_r].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_g].AsInt()), static_cast<uint8_t>(element.AsArray()[elem_rgba_b].AsInt()), element.AsArray()[elem_rgba_a].AsDouble()}});
            }
            else
            {
                result.push_back(svg::Color{});
            }
        }
    }
    else if(colors_parametrs.IsString()) // Один цвет строка
    {
        result.push_back(svg::Color{colors_parametrs.AsString()});
    }
    return result;
}

bool detail::JSONreader::CheckRangeValue(double number) const
{
    if((number < range_value_.start_other_parametrs) && (range_value_.end_other_parametrs - number < 0))
    {
        return true;
    }
    return false;
}


// ====================== Вывод ==========================

void detail::JSONreader::SetOutPutDataBase(const json::Array& array, const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter& transport_route, std::ostream &output)
{
    for(const json::Node& element : array)
    {
        try
        {          
            json::Dict bus_or_stop_or_map_or_route = element.AsMap(); // Один эл-т с Map для построения карты
            if(bus_or_stop_or_map_or_route.find(type) != bus_or_stop_or_map_or_route.end() && bus_or_stop_or_map_or_route.at(type) == bus) // Нужно смотреть, что кидает at и делать try catch
            {
                TakeBusInfoOut(bus_or_stop_or_map_or_route); // Думаем насчет move
            }
            else if(bus_or_stop_or_map_or_route.find(type) != bus_or_stop_or_map_or_route.end() && bus_or_stop_or_map_or_route.at(type) == stop)
            {
                TakeStopInfoOut(bus_or_stop_or_map_or_route); // Думаем насчет move
            }
            else if(bus_or_stop_or_map_or_route.find(type) != bus_or_stop_or_map_or_route.end() && bus_or_stop_or_map_or_route.at(type) == map)
            {
                TakePictureInfoOut(bus_or_stop_or_map_or_route); // Это одноразово
            }
            else if(bus_or_stop_or_map_or_route.find(type) != bus_or_stop_or_map_or_route.end() && bus_or_stop_or_map_or_route.at(type) == route) // Новое добавлено
            {
                TakeRouteInfoOut(bus_or_stop_or_map_or_route); // Думаем насчет move
            }
        }
        catch (std::out_of_range& error)
        {
             std::cout << error.what() << "In output data have place where key type or key words Bus and Stop abcent";
             std::cout << "Look input data file. Adress: JSONreader -> SetOutPutDataBase" << std::endl;
        }
        catch (...)
        {
              std::cout << "Unknown error === Address: JSONreader -> SetOutPutDataBase";
        }
    }
    PrintResults(catalogue, map_renderrer, transport_route, output);
}

void detail::JSONreader::TakeBusInfoOut(const json::Dict& information_about_bus) // Можно сделать через for - временный объект и добавление (этот вариант короче), но менее наглядно
{
    id_type_bus_or_stop_.push_back(detail::JSONreader::OutInfo{information_about_bus.at(id).AsInt(), information_about_bus.at(type).AsString(),information_about_bus.at(name).AsString()});
}
// Можно вообще убрать эти две функции, но решил вынести в отдельность, если в будущем будет меняться структура запроса было проще менять
void detail::JSONreader::TakeStopInfoOut(const json::Dict& information_about_stop) // Можно сделать через for - временный объект и добавление (этот вариант короче), но менее наглядно
{
    id_type_bus_or_stop_.push_back(detail::JSONreader::OutInfo{information_about_stop.at(id).AsInt(), information_about_stop.at(type).AsString(),information_about_stop.at(name).AsString()});
}

void detail::JSONreader::TakePictureInfoOut(const json::Dict &information_about_picture)
{
    id_type_bus_or_stop_.push_back(detail::JSONreader::OutInfo{information_about_picture.at(id).AsInt(), information_about_picture.at(type).AsString(), ""});
}

void detail::JSONreader::TakeRouteInfoOut(const json::Dict &information_about_route) // Новое добавлено
{
    id_type_bus_or_stop_.push_back(detail::JSONreader::OutInfo{information_about_route.at(id).AsInt(), information_about_route.at(type).AsString(), information_about_route.at(from).AsString(), information_about_route.at(to).AsString()});
}

void detail::JSONreader::PrintResults(const Catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& map_renderrer, const Route::TransportRouter& router,std::ostream& out) const
{
    using namespace std::literals;
    bool first = true;
    std::string sign = "";
    out  << "["s; // << std::endl был перед скобкой видимо просто для отделения в консоли для большей наглядности
    for(const auto& element : id_type_bus_or_stop_)
    {
        out << sign;
        if(element.type == bus)
        {
            PrintOutBus(catalogue, element.name, element.id, out);
        }
        else if(element.type == stop)
        {
            PrintOutStop(catalogue, element.name, element.id, out);
        }
        else if(element.type == map)
        {
            PrintOutPicture(map_renderrer, catalogue, element.id, out);
        }
        else if(element.type == route)
        {
            PrintOutRouter(router, element.from, element.to, element.id, out);
        }
        else
        {
            try
            {
                throw std::logic_error("Incorrect save data for output data"s);
            }  catch (std::logic_error& error)
            {
                std::cout << error.what() << "Check id_type_bus_or_stop what about data which it save"s << std::endl;
            }
            catch (...)
            {
                std::cout << "Unknown error === Address: JSONreader -> SetOutPutDataBase"s;
            }
        }
        if(first)
        {
            sign +=",";
            first = false;
        }
    }
        out << "\n]"s << std::endl;
}

void detail::JSONreader::PrintOutStop(const Catalogue::TransportCatalogue& catalogue, const std::string& name_stop, int32_t id, std::ostream& out) const
{
    const std::unique_ptr<std::set<std::string>> all_stops_in_request = catalogue.GetBusesByStop(name_stop);
    const std::string request_id = "request_id";
    const std::string buses = "buses";
    if(all_stops_in_request != nullptr)
    {
        json::Dict stops_parametrs;
        json::Array all_stops;
        for(const auto& element : *all_stops_in_request)
        {
            all_stops.push_back(element);
        }
        stops_parametrs[buses] = all_stops;
        stops_parametrs[request_id] = id;
        json::Print(json::Document{stops_parametrs}, out);
    }
    else
    {
        PrintEmptyRequest(id,out);
    }
}

void detail::JSONreader::PrintOutBus(const Catalogue::TransportCatalogue& catalogue, const std::string& name_bus, int32_t id, std::ostream& out) const
{
    const std::string curvature = "curvature"; // words which need for print results, they will see in concole
    const std::string request_id = "request_id";
    const std::string route_length = "route_length";
    const std::string stop_count = "stop_count";
    const std::string unique_stop_count = "unique_stop_count";
    if(catalogue.GetBusStatistics(name_bus).has_value())
    {
        BusStatistics temp = catalogue.GetBusStatistics(name_bus).value();
        json::Dict bus_parametrs;
        bus_parametrs[curvature] = temp.curvature_;
        bus_parametrs[request_id] = id;
        bus_parametrs[route_length] = temp.length_;
        bus_parametrs[stop_count] = static_cast<double>(temp.count_stops_);
        bus_parametrs[unique_stop_count] = static_cast<double>(temp.uniq_stops_);
        json::Print(json::Document{bus_parametrs}, out);
    }
    else
    {
        PrintEmptyRequest(id, out);
    }
}

void detail::JSONreader::PrintEmptyRequest(int32_t id, std::ostream& out) const
{
    const std::string req_id = "request_id";
    const std::string error_massage = "error_message";
    const std::string miss = "not found";
    json::Dict empty_request;
    empty_request[req_id] = id;
    empty_request[error_massage] = miss;
    json::Print(json::Document{empty_request}, out);
}

void detail::JSONreader::PrintOutPicture(const renderer::MapRenderer& map_renderrer, const Catalogue::TransportCatalogue& catalogue, int32_t id, std::ostream& out) const
{
    const std::string request_id = "request_id";
    const std::string map = "map";
    json::Dict picture_parametrs;
    std::ostringstream svg_str;
    map_renderrer.RenderMap(catalogue).Render(svg_str);
    picture_parametrs[map] = svg_str.str();
    picture_parametrs[request_id] = id;
    json::Print(json::Document{picture_parametrs}, out);
}

void detail::JSONreader::PrintOutRouter(const Route::TransportRouter& route, const std::string &from_stop, const std::string &to_stop, int32_t id, std::ostream &out) const
{
    auto vec = route.GetMinimumRouteInfo(from_stop, to_stop);
    if(vec.has_value())
    {
        const std::string req_id = "request_id";
        const std::string total_time = "total_time";
        const std::string items_key = "items";
        json::Dict route_parametrs;
        route_parametrs[req_id] = id;
        route_parametrs[total_time] = vec->front().total_time;
        json::Array items;
        for(const auto& elem : vec.value())
        {
          if(!elem.stop_name.empty())
          {
              items.emplace_back(PrintItemsWait(elem));
          }
          else if(!elem.bus_name.empty())
          {
             items.emplace_back(PrintItemsBus(elem));
          }
        }
        route_parametrs[items_key] = items;
        json::Print(json::Document{route_parametrs}, out);
    }
    else
    {
        PrintEmptyRequest(id, out);
    }
}

json::Dict detail::JSONreader::PrintItemsWait(const Route::RouteOutputData& data) const
{
    const std::string type = "type";
    const std::string stop_name = "stop_name";
    const std::string time = "time";
    const std::string wait = "Wait";
    json::Dict result;
    result[stop_name] = static_cast<std::string>(data.stop_name);
    result[time] = data.edge_time;
    result[type] = wait;
    return result;
}

json::Dict detail::JSONreader::PrintItemsBus(const Route::RouteOutputData& data) const
{
    const std::string type = "type";
    const std::string bus = "bus";
    const std::string time = "time";
    const std::string span_count = "span_count";
    json::Dict result;
    result[time] = data.edge_time;
    result[bus] = static_cast<std::string>(data.bus_name);
    result[type] = "Bus";
    result[span_count] = data.span_count;
    return result;
}
