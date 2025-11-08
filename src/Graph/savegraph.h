#ifndef MOVIERECOMMENDER_SAVEGRAPH_H
#define MOVIERECOMMENDER_SAVEGRAPH_H
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "Graph.h"
#include "../MoviesUtil/Movie.h"

inline void saveGraphToDisk(const Graph& g, const std::string& path) {
    nlohmann::json j;

    const auto& movies = g.getMovies();
    const auto& adj    = g.getAdj();

    j["movies"] = nlohmann::json::array();

    for (const auto&[tmdbId, name, genres, rating, year] : movies) {
        nlohmann::json jm;
        jm["tmdbId"] = tmdbId;
        jm["title"]  = name;
        jm["genres"] = genres;
        jm["rating"] = rating;
        jm["year"]   = year;
        j["movies"].push_back(jm);
    }

    if (adj.size() != movies.size()) {
        throw std::runtime_error("saveGraphToDisk: adj size does not match movies size");
    }

    j["adj"] = nlohmann::json::array();

    for (const auto& nbrs : adj) {
        nlohmann::json row = nlohmann::json::array();

        for (const auto&[to, weight] : nbrs) {
            nlohmann::json je;
            je["to"] = to;
            je["w"]  = weight;
            row.push_back(je);
        }

        j["adj"].push_back(row);
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("saveGraphToDisk: failed to open file: " + path);
    }

    out << j.dump(2) << '\n';
}


#endif //MOVIERECOMMENDER_SAVEGRAPH_H