#pragma once
#include <deque>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>


#include "geo.h"

namespace Catalogue
{
                                                     // Подскажите пожалуйста, что ещё можно улучшить в проекте? //

// Если у Вас есть время можете пожалуйста ответить на пару вопросов
//   1) Я реализовал деструктор для типа Bus 93 row in this file, but при окончании проекта он вызывается и видимо удаляться нечему и программа падает, 
// как отследить, что остается в памяти как мусор на текущий момент и понять почему в самом конце вызывает деструктор, хотя по областям видимости он должен
// был удалиться раньше?

//  2) Меня беспокоит, что я не использовал string_view в input_reader.cpp 149 row, хряняться string хоть и временно, но неприятно согласитесь, происходит инвалидация string_view в одном моменте
//  159 row file input_reader.cpp в образование пары у элемента element.id, хотя сдесь указатель ссылается на вектор commands_, который уже получается не меняется, по идее релокации нет и он находиться в классе
// все должно быть хорошо, но он падает...?


// Благодарю за уделенное время! :)

// Если не получиться снова встретится до этого года, то с Наступающим!)
class TransportCatalogue
{
private:

    struct Stop
    {
        Stop() = delete;
        explicit Stop(const std::string& name_stop, const Coordinates& coordinates)
            :name_stop_(name_stop), coordinates_(coordinates)
        {}
        explicit Stop(const Stop& other)
            : name_stop_(other.name_stop_), coordinates_(other.coordinates_)
        {}
        explicit Stop(Stop&& other) noexcept
            : name_stop_(std::move(other.name_stop_)), coordinates_(std::move(other.coordinates_))
        {}
        std::string name_stop_;
        Coordinates coordinates_;
    };

    struct Bus
    {
        Bus() = delete;

        explicit Bus(Bus&& other) noexcept
            : name_bus_(std::move(other.name_bus_)), stops_in_route_(std::move(other.stops_in_route_))
        {
            other.stops_in_route_.clear();
        }
        explicit Bus(const std::string& name_bus, const std::vector<Stop*>& stops_in_route)
            : name_bus_(name_bus), stops_in_route_(stops_in_route)
        {}

        explicit Bus(const Bus& other)
            : name_bus_(other.name_bus_)
        {
            stops_in_route_.reserve(other.stops_in_route_.size());
            for(const auto* stop : other.stops_in_route_)
            {
                stops_in_route_.push_back(new Stop(*stop));
            }
        }

        Bus& operator=(Bus&& other) noexcept
        {
            if(this != &other)
            {
                name_bus_ = std::move(other.name_bus_);
                stops_in_route_ = std::move(other.stops_in_route_);
                other.stops_in_route_.clear();
            }
            return *this;
        }

        Bus& operator=(const Bus& other)
        {
           if(this != &other)
           {
               name_bus_ = other.name_bus_;
               for(Stop* stop: stops_in_route_)
               {
                   delete stop;
               }
               stops_in_route_.clear();

               stops_in_route_.reserve(other.stops_in_route_.size());
               for(Stop* stop: other.stops_in_route_)
               {
                   stops_in_route_.push_back(new Stop(*stop));
               }
           }
           return *this;
        }

        ~Bus() // Возникает ошибка
        {
//            for(Stop* stop: stops_in_route_)
//            {
//                delete  stop;
//            }
        }

        std::string name_bus_;
        std::vector<Stop*> stops_in_route_;
    };

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> check_stop_;
    std::unordered_map<std::string_view, Bus*> check_bus_;
    std::unordered_map<std::string_view, double> lenght_in_bus_;
    std::unordered_map<const Stop*,std::set<std::string_view>> stop_enter_in_routes_; // можно сделать string прибавим в скорости (наверно)
    std::unordered_map<std::string_view, double> bus_name_and_dist_;
    void AddStopIncludeOtherRoutes(const std::vector<Stop *>& stops, std::string_view name_bus); // Добавил передачу по ссылке
    double DistanceInRouteGeo(std::string_view request) const;
    size_t ComputeUniqStops(std::string_view request) const;

public:

    struct FinishStopDist // Для парсинга расстояния из запроса
    {
        explicit FinishStopDist() = default;
        explicit FinishStopDist(const std::string_view name_stop , const double distance_to_next) // string -> string_view
            : name_stop_(name_stop) ,distance_to_next_(distance_to_next)
        {}

        bool operator==(const FinishStopDist& other) const
        {
            return name_stop_ == other.name_stop_ && distance_to_next_ == other.distance_to_next_;
        }

        bool operator!=(const FinishStopDist& other) const
        {
            return !(*this == other);
        }

       std::string_view name_stop_;
       double distance_to_next_;
    };

    struct DistanceWithStops
    {
          explicit DistanceWithStops(DistanceWithStops&& other) noexcept
            : start_name_stop_(std::move(other.start_name_stop_)), finish_stop_and_dist_(std::move(other.finish_stop_and_dist_))
          {}
          explicit DistanceWithStops(const std::string& name_stop, const std::vector<FinishStopDist>& stop_dist) // string -> string_view
              : start_name_stop_(static_cast<std::string>(name_stop)), finish_stop_and_dist_(std::move(stop_dist))
          {}

            DistanceWithStops(const DistanceWithStops& other)
                : start_name_stop_(other.start_name_stop_), finish_stop_and_dist_(other.finish_stop_and_dist_)
            {}

           bool operator==(const DistanceWithStops& other) const
           {
               return start_name_stop_ == other.start_name_stop_ && finish_stop_and_dist_ == other.finish_stop_and_dist_;
           }

           bool operator!=(const DistanceWithStops& other) const
           {
               return !(*this == other);
           }

          std::string start_name_stop_;
          std::vector<FinishStopDist> finish_stop_and_dist_;
    };

    struct BusIncludeNameStops
    {
        explicit BusIncludeNameStops(BusIncludeNameStops&& other) noexcept
       : name_bus_(std::move(other.name_bus_)), route_(std::move(other.route_))
        {}

        explicit BusIncludeNameStops(std::string name_bus, std::vector<std::string_view> route) // Возможно лучше переделать const std::string& на string_view, объект все равно новый создасться (как в busstatistics
       : name_bus_(std::move(name_bus)), route_(std::move(route))
        {}

        BusIncludeNameStops (const BusIncludeNameStops& other)
       : name_bus_(other.name_bus_), route_(other.route_)
        {}

        bool operator==(const BusIncludeNameStops& other) const
        {
            return name_bus_ == other.name_bus_ && route_ == other.route_;
        }

        bool operator!=(const BusIncludeNameStops& other) const
        {
            return !(*this == other);
        }

        std::string name_bus_;
        std::vector<std::string_view> route_;
    };

    struct BusStatistics
    {
        explicit BusStatistics()
            : name_bus_(""), count_stops_(0), uniq_stops_(0), length_(0), curvature_(0)
        {}
        explicit BusStatistics(std::string_view name_bus, size_t count_stops, size_t uniq_stops, double length, double curvature)
            : name_bus_(static_cast<std::string>(name_bus)), count_stops_(count_stops), uniq_stops_(uniq_stops), length_(length), curvature_(curvature)
        {}
        std::string name_bus_;
        size_t count_stops_;
        size_t uniq_stops_;
        double length_;
        double curvature_;
    };

    BusStatistics GetBusStatistics(std::string_view request) const;
    void AddStop(const std::string& name_stop, Coordinates coordinates);
    const Stop* FindStop(std::string_view name_stop) const;
    void AddBus(std::string name_bus, const std::vector<std::string_view>& route, double distance);
    const Bus* FindBus(std::string_view name_bus) const;
    const std::unique_ptr<std::set<std::string_view>> GetBusesEnterInRoute(const Stop* stop) const; // Хотел через optional ссылку, потом подумал более громоздко решил через умный указатель - это хуже или лучше?
};

}
