#include <iostream>
#include <string>
#include <fstream>

#include "json.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace std;


// Если я не использую хэши так как каждый запрос уникален, это нормально или нет?

int main()
{
    const std::string key_for_array_describe_buses_and_stops = "base_requests";
    const std::string key_for_array_requests_by_base = "stat_requests";
    const std::string key_for_map_render_settings = "render_settings";
    std::ifstream input("c:\\Qt\\My_projects\\Diogen\\build-Transport__catalogue-Desktop_Qt_5_12_12_MinGW_64_bit-Debug\\s10_final_opentest_3.json");
    if(!input.is_open())
    {
        throw std::invalid_argument("Not right path to file");
    }
    std::ofstream file("text.txt");
    Catalogue::TransportCatalogue catalogue;
    renderer::MapRenderer map_renderer;
    json::Document requests = json::Load(input);
    detail::Json_reader data_base{requests};
    data_base.SetInputRequests(catalogue, key_for_array_describe_buses_and_stops); // Решил разбить на две ф-и, можно в одной не знаю, как лучше, просто на // на две разбил не так перегружена и может более наглядно, как две ветки
    data_base.SetInputParametrsPicture(map_renderer, key_for_map_render_settings);
    RequestHandler facade{catalogue, map_renderer};
    data_base.SetOutPutRequests(facade, key_for_array_requests_by_base, file);
}
