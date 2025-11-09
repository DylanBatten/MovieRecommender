#ifndef MOVIERECOMMENDER_LOADGRAPH_H
#define MOVIERECOMMENDER_LOADGRAPH_H
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "./graph.h"
#include "../MoviesUtil/Movie.h"

inline Graph loadGraphFromDisk(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open graph file: " + path);
    }

    nlohmann::json j;
    in >> j;

    Graph g;

    if (!j.contains("movies") || !j["movies"].is_array()) {
        throw std::runtime_error("Graph file missing 'movies' array");
    }

    for (const auto& jm : j["movies"]) {
        Movie m;

        m.tmdbId = jm.at("tmdbId").get<int>();
        m.name  = jm.at("title").get<std::string>();

        if (jm.contains("genres") && jm["genres"].is_array()) {
            m.genres = jm["genres"].get<std::vector<std::string>>();
        }
        m.rating = jm.value("rating", 0.0);
        m.year   = jm.value("year", 0);

        g.addMovie(m);
    }

    const std::size_t n = g.getMovies().size();

    if (!j.contains("adj") || !j["adj"].is_array()) {
        throw std::runtime_error("Graph file missing 'adj' array");
    }

    const auto& adjJson = j["adj"];
    if (adjJson.size() != n) {
        throw std::runtime_error("Graph 'adj' size does not match 'movies' size");
    }

    // Create edges using the graph's addEdge method instead of modifying a local copy
    for (std::size_t i = 0; i < n; ++i) {
        const auto& row = adjJson[i];
        if (!row.is_array()) {
            throw std::runtime_error("Graph 'adj' row is not an array at index " + std::to_string(i));
        }

        for (const auto& je : row) {
            int to = je.at("to").get<int>();
            double weight = je.at("w").get<double>();

            if (to < 0 || static_cast<std::size_t>(to) >= n) {
                throw std::runtime_error("Invalid 'to' index in adj at row " + std::to_string(i));
            }

            // Use the graph's addEdge method to actually add the edge to the graph
            g.addEdge(static_cast<int>(i), to, weight);
        }
    }

    return g;
}

#endif //MOVIERECOMMENDER_LOADGRAPH_H