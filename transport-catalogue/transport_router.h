#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <unordered_map>
#include <optional>
#include <memory>

namespace Route
{

struct RouteOptions
{
    double bus_velocity = 0.0;
    int32_t bus_time_wait = 0;
};

struct EdgeItem
{
     graph::VertexId vertex_id = 0;
     std::string_view stop_name_from;
     std::string_view stop_name_to;
     double time = 0.0;
     int32_t span_count = 0;
     graph::EdgeId edge_id = 0;
     std::string_view bus_name;
};

struct RouteOutputData
{
     std::string_view stop_name;
     std::string_view bus_name;
     int32_t span_count = 0;
     double edge_time = 0.0;
     double total_time = 0.0;
};

class TransportRouter
{
public:
    explicit TransportRouter(Catalogue::TransportCatalogue& catalogue_db);
    void BuildGraph();
    const std::optional<std::vector<RouteOutputData>> GetMinimumRouteInfo(const std::string_view stop_from, const std::string_view stop_to) const;
    void SetParametrsBusWaitTimeAndVelocity(int32_t bus_time_wait, double bus_velocity);
private:
    RouteOptions router_settings_;
    Catalogue::TransportCatalogue& catalogue_db_;
    graph::DirectedWeightedGraph<double> base_graph_;
    std::unique_ptr<graph::Router<double>> router_ = nullptr;
    std::unordered_map<graph::EdgeId, EdgeItem> edge_id_items_{};
    std::unordered_map<const Stop*, graph::VertexId> stop_vertex_ids_{};
};

}
