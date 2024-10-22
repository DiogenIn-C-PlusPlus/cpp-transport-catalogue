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
           PrintOutBus(transport_catalogue.AllDataBus(request), output);
        }
    }
}

void detail::StatReader::PrintFindStops(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const
{
        if(transport_catalogue.FindStop(request) == nullptr)
        {
          PrintOutStop({},request, output, 0);
          return;
        }
        else if(transport_catalogue.GetBusesEnterInRoute(transport_catalogue.FindStop(request)).empty())
        {
          PrintOutStop({},request, output, 1);
          return;
        }
        std::set<std::string_view> sorted_numbers_buses = transport_catalogue.GetBusesEnterInRoute(transport_catalogue.FindStop(request));
        PrintOutStop(sorted_numbers_buses, request, output, 2);
}


void detail::StatReader::PrintOutBus(Catalogue::TransportCatalogue::OutPutBus data_bus, std::ostream& output) const
{
    using namespace std::literals;
    if(data_bus.count_stops_ == 0)
    {
        output <<"Bus " << data_bus.name_bus_ <<": "s << "not found"s << std::endl;
    }
    else
    {
         output << "Bus " << data_bus.name_bus_ <<": "s <<data_bus.count_stops_ << " stops on route, "s << data_bus.uniq_stops_ << " unique stops, "s << std::setprecision(6) << data_bus.lenght_ << " route length"s << std::endl;
    }
}
// Честно признаюсь так и не понял, как передать суда константный указатель просто const Stop*, а внутри уже вызывать метод?
void detail::StatReader::PrintOutStop(const std::set<std::string_view>& number_buses, std::string_view name_stop, std::ostream &output, size_t number_operation) const
{
    using namespace std::literals;
    switch (number_operation)
    {
    case 0:
     {
      output <<"Stop " << name_stop <<": "s << "not found"s << std::endl;
      break;
     }
    case 1:
     {
      output <<"Stop " << name_stop <<": "s << "no buses"s << std::endl;
      break;
     }
    case 2:
     {
      output <<"Stop " << name_stop <<": buses"s;
      for(const auto& number_bus: number_buses)
      {
          output << " " << number_bus;
      }
      output << std::endl;
      break;
     }
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
