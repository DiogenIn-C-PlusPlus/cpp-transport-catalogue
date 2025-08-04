#include "transport_router.h"


Route::TransportRouter::TransportRouter(Catalogue::TransportCatalogue& catalogue_db)
    : catalogue_db_(catalogue_db)
    , base_graph_(catalogue_db.GetAllStops().size() * 2)
{}

void Route::TransportRouter::BuildGraph()
{
    const double koef = 60.0 / (1000 * router_settings_.bus_velocity);
    std::deque<Bus> buses = catalogue_db_.GetAllBuses();
    graph::VertexId id = 0;
    for (const Bus& bus : buses)
    {
        std::vector<Stop*> bus_stops = catalogue_db_.FindBus(bus.name_bus_)->stops_in_route_;
        for (int32_t i = 0; i < static_cast<int32_t>(bus_stops.size()); ++i)
        {
            if (stop_vertex_ids_.find(bus_stops[i]) != stop_vertex_ids_.end())
            {
                continue;
            }
            stop_vertex_ids_[bus_stops[i]] = id;
            ++id;
        }
    }
    for (const Bus& bus : buses)
    {
        std::vector<Stop*> bus_stops = catalogue_db_.FindBus(bus.name_bus_)->stops_in_route_;
        for (int32_t i = 0; i < static_cast<int32_t>(bus_stops.size()); ++i)
        {
            double total_distance_time = router_settings_.bus_time_wait;
            Stop* stop_from = bus_stops[i];
            graph::VertexId start_point_id = stop_vertex_ids_[stop_from];
            if(catalogue_db_.GetBusStatistics(bus.name_bus_).has_value())
            {
               total_distance_time += catalogue_db_.GetBusStatistics(bus.name_bus_).value().length_ * koef;
            }
            for (int32_t j = i; j + 1 < static_cast<int32_t>(bus_stops.size()); ++j)
            {
                if (!catalogue_db_.GetRoudtripRoute()->at(bus.name_bus_).first && (i < static_cast<int32_t>(bus_stops.size() / 2)) && j == static_cast<int32_t>(bus_stops.size()) / 2 && stop_from->name_stop_ != bus_stops[bus_stops.size() / 2]->name_stop_)
                {
                    break;
                }
                Stop* stop_to = bus_stops[j + 1];
                graph::VertexId end_point_id = stop_vertex_ids_.at(stop_to);
                graph::EdgeId edge_id = base_graph_.AddEdge({ start_point_id, end_point_id, total_distance_time });
                edge_id_items_[edge_id].edge_id = edge_id;
                edge_id_items_[edge_id].time = total_distance_time;
                edge_id_items_[edge_id].span_count = j - i + 1;
                edge_id_items_[edge_id].stop_name_from = stop_from->name_stop_;
                edge_id_items_[edge_id].stop_name_to = stop_to->name_stop_;
                edge_id_items_[edge_id].bus_name = catalogue_db_.FindBus(bus.name_bus_)->name_bus_;
            }
        }
    }
        router_ = std::make_unique<graph::Router<double>>(base_graph_);
}

const std::optional<std::vector<Route::RouteOutputData>> Route::TransportRouter::GetMinimumRouteInfo(const std::string_view stop_from, const std::string_view stop_to) const
{
    std::set<std::string> buses_on_start_stop = *catalogue_db_.GetBusesByStop(stop_from);
    std::set<std::string> buses_on_end_stop = *catalogue_db_.GetBusesByStop(stop_to);
    if (buses_on_start_stop.empty() || buses_on_end_stop.empty())
    {
        return std::nullopt;
    }
    if (edge_id_items_.empty())
    {
        return std::nullopt;
    }
    std::vector<RouteOutputData> min_route_data;
    graph::VertexId from = stop_vertex_ids_.at(catalogue_db_.FindStop(stop_from));
    graph::VertexId to = stop_vertex_ids_.at(catalogue_db_.FindStop(stop_to));
    const std::optional<graph::Router<double>::RouteInfo>& router_output = router_->BuildRoute(from, to);
    if (router_output.has_value())
    {
        std::unordered_map<graph::EdgeId, EdgeItem> optimum_id_edge_items;
        for (const graph::EdgeId& optimum_edge_id : router_output.value().edges)
        {
            if (edge_id_items_.count(optimum_edge_id))
            {
                optimum_id_edge_items[optimum_edge_id] = edge_id_items_.at(optimum_edge_id);
            }
        }
        for (const auto& [edge_id, edge_item] : optimum_id_edge_items)
        {
            min_route_data.push_back({ edge_item.stop_name_from, edge_item.bus_name, edge_item.span_count, edge_item.time - router_settings_.bus_time_wait, router_output.value().weight });
        }
        return min_route_data;
    }
    return std::nullopt;
}

void Route::TransportRouter::SetParametrsBusWaitTimeAndVelocity(int32_t bus_time_wait, double bus_velocity)
{
    router_settings_.bus_time_wait = bus_time_wait;
    router_settings_.bus_velocity = bus_velocity;
}
