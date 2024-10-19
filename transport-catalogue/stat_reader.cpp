#include "stat_reader.h"

void detail::StatReader::ParseAndAddRequest(std::string_view request)
{
    save_requests_.push_back(static_cast<std::string>(request));
}

void detail::StatReader::PrintResults(const Catalogue::TransportCatalogue &transport_catalogue, std::ostream &output) const
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
            PrintFindBuses(transport_catalogue, output, request_search);
        }
    }
}

void detail::StatReader::PrintFindBuses(const Catalogue::TransportCatalogue& transport_catalogue, std::ostream& output, std::string_view request) const
{
        if(transport_catalogue.FindBus(request) == nullptr)
        {
          PrintOutBus(request, size_t{},size_t{},double{}, output, 0);
          return;
        }
        size_t count_stops = transport_catalogue.FindBus(request)->stops_in_route_.size();
        size_t uniq_stops = ComputeUniqStops(transport_catalogue, request);
        double distance = DistanceInRoute(transport_catalogue, request);
        PrintOutBus(request, count_stops,uniq_stops,distance, output, 1);
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

double detail::StatReader::DistanceInRoute(const Catalogue::TransportCatalogue& transport_catalogue, std::string_view request) const
{
    double dist_in_route = 0;
    for(size_t i = 0; i +1 <  transport_catalogue.FindBus(request)->stops_in_route_.size(); ++i)
    {
     dist_in_route += ComputeDistance(transport_catalogue.FindBus(request)->stops_in_route_[i]->coordinates_, transport_catalogue.FindBus(request)->stops_in_route_[i+1]->coordinates_);
    }
    return dist_in_route;
}

size_t detail::StatReader::ComputeUniqStops(const Catalogue::TransportCatalogue& transport_catalogue, std::string_view request) const
{
    std::unordered_set<std::string> result;
    for(const auto& stop: transport_catalogue.FindBus(request)->stops_in_route_)
    {
        result.insert(stop->name_stop_);
    }
    return result.size();
}

void detail::StatReader::PrintOutBus(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double distance, std::ostream& output, size_t number_out) const
{
    using namespace std::literals;
    switch (number_out)
    {
      case 0:
       {
        output <<"Bus " << name_bus <<": "s << "not found"s << std::endl;
        break;
       }
     case 1:
      {
        output << "Bus " << name_bus <<": "s <<count_stops << " stops on route, "s << uniq_stops << " unique stops, "s << std::setprecision(6) <<distance << " route length"s << std::endl;
        break;
      }
    }
}

void detail::StatReader::PrintOutStop(const std::set<std::string_view> number_buses, std::string_view name_stop, std::ostream &output, size_t number_operation) const
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
