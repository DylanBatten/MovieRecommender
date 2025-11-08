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

    // ----- movies -----
    j["movies"] = nlohmann::json::array();

    for (const auto& m : movies) {
        nlohmann::json jm;
        jm["tmdbId"] = m.tmdbId;       // or m.getTmdbId()
        jm["title"]  = m.name;        // or m.getTitle()
        jm["genres"] = m.genres;
        jm["rating"] = m.rating;
        jm["year"]   = m.year;
        j["movies"].push_back(jm);
    }

    // sanity check
    if (adj.size() != movies.size()) {
        throw std::runtime_error("saveGraphToDisk: adj size does not match movies size");
    }

    // ----- adjacency -----
    j["adj"] = nlohmann::json::array();

    for (const auto& nbrs : adj) {
        nlohmann::json row = nlohmann::json::array();

        for (const auto& e : nbrs) {
            nlohmann::json je;
            je["to"] = e.to;
            je["w"]  = e.weight;   // double or scaled int, consistent with your Edge
            row.push_back(je);
        }

        j["adj"].push_back(row);
    }

    // ----- write to disk -----
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("saveGraphToDisk: failed to open file: " + path);
    }

    out << j.dump(2) << '\n';
}


#endif //MOVIERECOMMENDER_SAVEGRAPH_H