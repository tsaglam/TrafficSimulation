#include <iostream>
#include <exception>
#include "json.hpp"

using json = nlohmann::json;

struct Car {};

int main()
{
    json input;
    try
    {
        std::cin >> input;
    }
    catch(std::exception e)
    {
        std::cerr << "Failed to parse JSON.\n";
        return 1;
    }

    input["time_steps"];
    for(const auto& junction : input["junctions"])
    {
        junction["id"];
        junction["x"];
        junction["y"];
        for(const auto& signal : junction["signals"])
        {
            signal["dir"]; // 0->North, 1->East, 2->South, 3->West
            signal["time"]; // in s
        }
    }
    for(const auto& road : input["roads"])
    {
        road["junction1"];
        road["junction2"]; // junction IDs
        road["lanes"]; // 1, 2 or 3
        road["limit"]; // in km/h
    }
    for(const auto& car : input["cars"])
    {
        car["id"];
        car["target_velocity"]; // in km/h
        car["max_acceleration"]; // in m/s²
        car["target_deceleration"]; // in m/s²
        car["min_distance"]; // in m
        car["target_headway"]; // in s
        car["politeness"]; // [0.0, 1.0]
        car["start"]["from"];
        car["start"]["to"]; // starting road via junction IDs
        car["start"]["lane"];
        car["start"]["distance"]; // distance from 'from' junction in m
        for(const auto& route : car["route"]) route;  // turn order: 0->uturn, 1->right, 2->straight, 3->left
    }

    std::vector<Car> my_cars;
    /* my_cars = ... */

    json output;
    for(const auto& car : my_cars)
    {
        json out_car;
        out_car["id"];
        out_car["from"];
        out_car["to"]; // junction IDs
        out_car["lane"];
        out_car["position"]; // in m
        output["cars"].push_back(out_car);
    }
    std::cout << output.dump() << "\n";

    return 0;
}
