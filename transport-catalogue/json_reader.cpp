#include "json_reader.h"

// ==== Вход в обработку

detail::Json_reader::Json_reader(json::Document doc)
    : requests_(doc.GetRoot())
    {}


void detail::Json_reader::SetInputRequests(Catalogue::TransportCatalogue& catalogue, std::string key_input)
{
    try
    {
        if(key_input != "base_requests")
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" == Address: json_reader -> SetInputRequests" <<std::endl;
    }
    SetInputDataBase(requests_.GetRoot().AsMap().at(key_input).AsArray(), catalogue);
}

void detail::Json_reader::SetOutPutRequests(RequestHandler facade, std::string key_output, std::ostream &output)
{
    try
    {
        if(key_output != "stat_requests")
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" == Address: json_reader -> SetOutPutRequests" <<std::endl;
    }
    SetOutPutDataBase(requests_.GetRoot().AsMap().at(key_output).AsArray(), facade , output);
}

void detail::Json_reader::SetInputParametrsPicture(renderer::MapRenderer& map_renderer, const std::string& key_input)
{
    try
    {
        if(key_input != "render_settings")
        {
            throw std::logic_error("Not right keys for load in data base");
        }
    }  catch (std::logic_error& error)
    {
        std::cout << error.what() << "\t Look correct you \"key_input\" and \"key_output\" or \"render_settings\" == Address: json_reader -> SetInputParametrsPicture" << std::endl;
    }
    LoadParametrsPicture(requests_.GetRoot().AsMap().at(key_input).AsMap(), map_renderer);
}

// =================  Ввод =========================

void detail::Json_reader::SetInputDataBase(const json::Array& array, Catalogue::TransportCatalogue &catalogue) // Можно попробовать через move (нужен один раз же)
{
    for(const json::Node& element : array)
    {
        try
        {
            json::Dict bus_or_stop = element.AsMap();
            if(bus_or_stop.find(key_word_bus_.type) != bus_or_stop.end() && bus_or_stop.at(key_word_bus_.type) == key_word_bus_.bus) // Нужно смотреть, что кидает at и делать try catch
            {
                TakeBusesInfo(bus_or_stop, catalogue); // Думаем насчет move
            }
            else if(bus_or_stop.find(key_word_stop_.type) != bus_or_stop.end() && bus_or_stop.at(key_word_stop_.type) == key_word_stop_.stop)
            {
                TakeStopsInfo(bus_or_stop, catalogue);  // Думаем насчет move
            }
        }
        catch (std::out_of_range& error)
        {
             std::cout << error.what() << "In input data have place where key type or key words Bus and Stop abcent";
             std::cout << "Look input data file. Adress: Json_reader -> SetInputDataBase" << std::endl;
        } catch (...)
        {
            std::cout << "Unknown error === Address: json_reader -> SetInputDataBase";
        }
    }
    for(const auto& name_bus_and_route : names_and_routes_)
    {
        catalogue.AddBus(std::move(name_bus_and_route.name_bus_), name_bus_and_route.route_, SetDistanceForBus(name_bus_and_route)); // move? Возможно там правда, если удалиться раньше, то у другой ф-и будет пусто
    }
    // names_and_routes_.clear(); // используем для map_render
    dist_between_stops_.clear(); // Очищаем, чтобы не занимать память - больше не понадобиться
}

void detail::Json_reader::TakeStopsInfo(const json::Dict& information_about_stop, Catalogue::TransportCatalogue& catalogue)
{
    Stop stop;
    for(const auto& [key_word, Node] : information_about_stop)
    {
        if(key_word_stop_.type == key_word)
        {
            continue;
        }
        else if(key_word_stop_.name == key_word)
        {
            stop.name_stop_ = Node.AsString();
        }
        else if(key_word_stop_.latitude == key_word)
        {
            stop.coordinates_.lat = Node.AsDouble();
        }
        else if(key_word_stop_.longitude == key_word)
        {
            stop.coordinates_.lng = Node.AsDouble();
        }
        else if(key_word_stop_.road_distances == key_word)
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
                std::cout << error.what() << "Look in data structure Stop. Address: json_reader -> TakeStopInfo" << std::endl;
            }
            catch (...)
            {
                std::cout << "Unknown error === Address: json_reader -> TakeStopInfo";
            }
        }
    }
    catalogue.AddStop(stop.name_stop_, stop.coordinates_);
}

void detail::Json_reader::TakeBusesInfo(const json::Dict& information_about_bus, Catalogue::TransportCatalogue& catalogue)
{
   BusIncludeNameStops temp;
   bool last_stop_equal_first = false;
   for(const auto& [key_word, Node] : information_about_bus)
   {
       if(key_word_bus_.type == key_word)
       {
           continue;
       }
       else if(key_word_bus_.is_roundtrip == key_word)
       {
           continue;
       }
       else if(key_word_bus_.name == key_word)
       {
            temp.name_bus_ = Node.AsString();
       }
       else if(key_word_bus_.stops == key_word)
       {
           for(const auto& name_stops : Node.AsArray())
           {
               temp.route_.push_back(name_stops.AsString()); // move?
           }
           if(!information_about_bus.at(key_word_bus_.is_roundtrip).AsBool()) // По идее если не кольцевой то добавляем
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
               std::cout << error.what() << "Look in data structure Bus. Address: json_reader -> TakeBusInfo" << std::endl;
           }
           catch (...)
           {
                std::cout << "Unknown error === Address: json_reader -> TakeBusInfo";
           }
       }
   }
   catalogue.SetRoudtripRoute({temp.name_bus_, information_about_bus.at(key_word_bus_.is_roundtrip).AsBool()}, last_stop_equal_first);
   names_and_routes_.push_back(std::move(temp));
}

double detail::Json_reader::SetDistanceForBus(const BusIncludeNameStops& name_bus_and_route) const
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

void detail::Json_reader::LoadParametrsPicture(const json::Dict& information_about_picture, renderer::MapRenderer& map_renderer)
{
    for(const auto& [key_word, value] : information_about_picture)
    {
            if(key_word == key_word_picture_.width)
            {
                map_renderer.SetWidth(value.AsDouble());
            }
            else if(key_word == key_word_picture_.height)
            {
                map_renderer.SetHeight(value.AsDouble());
            }
            else if (key_word == key_word_picture_.padding)
            {
                map_renderer.SetPanding(value.AsDouble());
            }
            else if(key_word == key_word_picture_.line_width)
            {
                map_renderer.SetBusLineWidth(value.AsDouble());
                map_renderer.SetStopLineWidth(value.AsDouble());
            }
            else if(key_word == key_word_picture_.stop_radius)
            {
                map_renderer.SetRadius(value.AsDouble());
            }
            else if(key_word == key_word_picture_.underlayer_width)
            {
                map_renderer.SetUnderlayerWidth(value.AsDouble());
            }
            else if(key_word == key_word_picture_.bus_label_font_size)
            {
                map_renderer.SetBusLabelFontSize(value.AsInt());
            }
            else if(key_word == key_word_picture_.stop_label_font_size)
            {
                map_renderer.SetStopLabelFontSize(value.AsInt());
            }
            else if(key_word == key_word_picture_.bus_label_offset)
            {
                map_renderer.SetBusLabelOffset(value.AsArray());
            }
            else if(key_word == key_word_picture_.stop_label_offset)
            {
                map_renderer.SetStopLabelOffset(value.AsArray());
            }
            else if(key_word == key_word_picture_.underlayer_color)
            {
                map_renderer.SetUnderlayerColor(value);
            }
            else if(key_word == key_word_picture_.color_palette)
            {
                 map_renderer.SetColorPalette(value);
            }
    }
    if(map_renderer.GetPanding() > std::min(map_renderer.GetHeight(), map_renderer.GetWidth()) / 2) // Обработка Painding связано с тем, что инициализация происходит не по порядку
    {
         map_renderer.SetPanding(std::min(map_renderer.GetHeight(), map_renderer.GetWidth()) / 2);
    }
    for(auto element : names_and_routes_) // Как лучше перебирать в цикле если делаем move по ссылке или нет
    {
        map_renderer.AddNameBusesForDraw(std::move(element.name_bus_));

    }
    names_and_routes_.clear(); // Освобождаем за ненадобностью
}

// ====================== Вывод ==========================

void detail::Json_reader::SetOutPutDataBase(const json::Array& array, const RequestHandler& fasad, std::ostream &output)
{
    for(const json::Node& element : array)
    {
        try
        {
            json::Dict bus_or_stop = element.AsMap(); // Один эл-т с Map для построения карты
            if(bus_or_stop.find(key_word_bus_.type) != bus_or_stop.end() && bus_or_stop.at(key_word_bus_.type) == key_word_bus_.bus) // Нужно смотреть, что кидает at и делать try catch
            {
                TakeBusInfoOut(bus_or_stop); // Думаем насчет move
            }
            else if(bus_or_stop.find(key_word_stop_.type) != bus_or_stop.end() && bus_or_stop.at(key_word_stop_.type) == key_word_stop_.stop)
            {
                TakeStopInfoOut(bus_or_stop); // Думаем насчет move
            }
            else if(bus_or_stop.find(key_word_picture_.type) != bus_or_stop.end() && bus_or_stop.at(key_word_picture_.type) == key_word_picture_.map)
            {
                TakePictureInfoOut(bus_or_stop); // Это одноразово, поэтому имя переменной не менял
            }
        }
        catch (std::out_of_range& error)
        {
             std::cout << error.what() << "In output data have place where key type or key words Bus and Stop abcent";
             std::cout << "Look input data file. Adress: Json_reader -> SetOutPutDataBase" << std::endl;
        }
        catch (...)
        {
              std::cout << "Unknown error === Address: json_reader -> SetOutPutDataBase";
        }
    }
    PrintResults(fasad, output);
}

void detail::Json_reader::TakeBusInfoOut(const json::Dict& information_about_bus) // Можно сделать через for - временный объект и добавление (этот вариант короче), но менее наглядно
{
    id_type_bus_or_stop_.push_back(detail::Json_reader::OutInfo{information_about_bus.at(key_word_bus_.id).AsInt(), information_about_bus.at(key_word_bus_.type).AsString(),information_about_bus.at(key_word_bus_.name).AsString()});
}
// Можно вообще убрать эти две функции, но решил вынести в отдельность, если в будущем будет меняться структура запроса было проще менять
void detail::Json_reader::TakeStopInfoOut(const json::Dict& information_about_stop) // Можно сделать через for - временный объект и добавление (этот вариант короче), но менее наглядно
{
    id_type_bus_or_stop_.push_back(detail::Json_reader::OutInfo{information_about_stop.at(key_word_stop_.id).AsInt(), information_about_stop.at(key_word_stop_.type).AsString(),information_about_stop.at(key_word_stop_.name).AsString()});
}

void detail::Json_reader::TakePictureInfoOut(const json::Dict &information_about_picture)
{
    id_type_bus_or_stop_.push_back(detail::Json_reader::OutInfo{information_about_picture.at(key_word_picture_.id).AsInt(), information_about_picture.at(key_word_picture_.type).AsString(), ""});
}

void detail::Json_reader::PrintResults(const RequestHandler& facade, std::ostream& out) const
{
    using namespace std::literals;
    bool first = true;
    std::string sign = "";
    out  << "["s; // << std::endl был перед скобкой видимо просто для отделения в консоли для большей наглядности
    for(const auto& element : id_type_bus_or_stop_)
    {
        out << sign;
        if(element.type == key_word_bus_.bus)
        {
            PrintOutBus(facade, element.name, element.id, out);
        }
        else if(element.type == key_word_stop_.stop)
        {
            PrintOutStop(facade, element.name, element.id, out);
        }
        else if(element.type == key_word_picture_.map)
        {
            PrintOutPicture(facade, element.id, out);
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
                std::cout << "Unknown error === Address: json_reader -> SetOutPutDataBase"s;
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

void detail::Json_reader::PrintOutStop(const RequestHandler& facade, const std::string& name_stop, int32_t id, std::ostream& out) const
{
    const std::unique_ptr<std::set<std::string>> all_stops_in_request = facade.GetBusesByStop(name_stop);
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

void detail::Json_reader::PrintOutBus(const RequestHandler& facade, const std::string& name_bus, int32_t id, std::ostream& out) const
{
    const std::string curvature = "curvature"; // words which need for print results, they will see in concole
    const std::string request_id = "request_id";
    const std::string route_length = "route_length";
    const std::string stop_count = "stop_count";
    const std::string unique_stop_count = "unique_stop_count";
    if(facade.GetBusStatistics(name_bus).has_value())
    {
        BusStatistics temp = facade.GetBusStatistics(name_bus).value();
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

void detail::Json_reader::PrintEmptyRequest(int32_t id, std::ostream& out) const
{
    const std::string req_id = "request_id";
    const std::string error_massage = "error_message";
    const std::string miss = "not found";
    json::Dict empty_request;
    empty_request[req_id] = id;
    empty_request[error_massage] = miss;
    json::Print(json::Document{empty_request}, out);
}

void detail::Json_reader::PrintOutPicture(const RequestHandler& facade, int32_t id, std::ostream& out) const
{
    const std::string request_id = "request_id";
    const std::string map = "map";
    json::Dict picture_parametrs;
    std::ostringstream svg_str;
    facade.RenderMap().Render(svg_str);
    picture_parametrs[map] = svg_str.str();
    picture_parametrs[request_id] = id;
    json::Print(json::Document{picture_parametrs}, out);
}
