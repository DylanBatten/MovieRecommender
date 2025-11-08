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

    // ----- Load movies -----
    if (!j.contains("movies") || !j["movies"].is_array()) {
        throw std::runtime_error("Graph file missing 'movies' array");
    }

    for (const auto& jm : j["movies"]) {
        Movie m;

        // required
        m.tmdbId = jm.at("tmdbId").get<int>();
        m.name  = jm.at("title").get<std::string>();

        // optional
        if (jm.contains("genres") && jm["genres"].is_array()) {
            m.genres = jm["genres"].get<std::vector<std::string>>();
        }
        m.rating = jm.value("rating", 0.0);
        m.year   = jm.value("year", 0);

        g.addMovie(m); // builds movies, adj (empty lists), and idToIndex
    }

    const std::size_t n = g.getMovies().size();

    // ----- Load adjacency -----
    if (!j.contains("adj") || !j["adj"].is_array()) {
        throw std::runtime_error("Graph file missing 'adj' array");
    }

    const auto& adjJson = j["adj"];
    if (adjJson.size() != n) {
        throw std::runtime_error("Graph 'adj' size does not match 'movies' size");
    }

    // we need writable access to adj; add adjRef() in MovieGraph:
    // std::vector<std::vector<Edge>>& adjRef() { return adj; }
    std::vector<std::vector<Edge>> adj = g.getAdj();
    adj.clear();
    adj.resize(n);

    for (std::size_t i = 0; i < n; ++i) {
        const auto& row = adjJson[i];
        if (!row.is_array()) {
            throw std::runtime_error("Graph 'adj' row is not an array at index " + std::to_string(i));
        }

        for (const auto& je : row) {
            Edge e;
            e.to = je.at("to").get<int>();
            e.weight  = je.at("w").get<double>();

            if (e.to < 0 || static_cast<std::size_t>(e.to) >= n) {
                throw std::runtime_error("Invalid 'to' index in adj at row " + std::to_string(i));
            }

            adj[i].push_back(e);
        }
    }

    return g;
}


#endif //MOVIERECOMMENDER_LOADGRAPH_H