#include "stat_reader.h"

void detail::StatReader::ParseAndAddRequest(std::string_view request)
{
    save_requests_.push_back(static_cast<std::string>(request));
}

void detail::StatReader::PrintResults(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream &output) const
{
    for(const auto& request: save_requests_)
    {
        std::string name_operation = request.substr(0, request.find(' '));
        std::string request_search = request.substr(request.find(' ') + 1);
        if(name_operation == "Stop")
        {
            PrintFindStops(transport_catalogue, output, request_search);
        }
        if(name_operation == "Bus")
        {
           PrintOutBus(transport_catalogue.GetBusStatistics(request_search), output);
        }
    }
}

void detail::StatReader::PrintFindStops(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const
{
        using namespace std::literals;
        if(transport_catalogue.FindStop(request) == nullptr)
        {
          output <<"Stop " << request <<": "s << "not found"s << std::endl;
          return;
        }
        else if(transport_catalogue.GetBusesEnterInRoute(transport_catalogue.FindStop(request)) == nullptr)
        {
          output <<"Stop " << request <<": "s << "no buses"s << std::endl;
          return;
        }
        std::set<std::string> sorted_numbers_buses = *transport_catalogue.GetBusesEnterInRoute(transport_catalogue.FindStop(request)); // Добавил для читабельности кода, чтобы было понятно, что они отсортированные
        output <<"Stop " << request <<": buses"s;
        for(const auto& number_bus: sorted_numbers_buses)
        {
            output << " " << number_bus;
        }
        output << std::endl;
}


void detail::StatReader::PrintOutBus(Catalogue::TransportCatalogue::BusStatistics data_bus, std::ostream& output) const
{
    using namespace std::literals;
    if(data_bus.count_stops_ == 0)
    {
        output <<"Bus " << data_bus.name_bus_ <<": "s << "not found"s << std::endl;
    }
    else
    {
         output << "Bus " << data_bus.name_bus_ <<": "s <<data_bus.count_stops_ << " stops on route, "s << data_bus.uniq_stops_ << " unique stops, "s << std::setprecision(6) << data_bus.length_ << " route length, "s << std::setprecision(6) << data_bus.curvature_ << " curvature"s << std::endl;
    }
}

void detail::StatReader::OutBaseRequest(std::istream &in, std::ostream &out, const Catalogue::TransportCatalogue& transport_catalogue)
{
    size_t stat_request_count;
    in >> stat_request_count >> std::ws;

    for (size_t i = 0; i < stat_request_count; ++i)
   {
     std::string line;
     std::getline(in, line);
     ParseAndAddRequest(line);
     }
     PrintResults(transport_catalogue, out);
}
