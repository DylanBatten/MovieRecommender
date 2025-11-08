#include "graph.h"
#include <iostream>

int Graph::addMovie(const Movie& movie) {
    const int id = movie.tmdbId;

    if (id <= 0) {
        std::cerr << "addMovie ERROR: invalid tmdbId for movie '"
                  << movie.name << "'\n";
        return -1;
    }

    if (const auto it = idToIndex.find(id); it != idToIndex.end()) {
        return it->second;
    }

    const int idx = static_cast<int>(movies.size());
    movies.push_back(movie);
    adj.emplace_back();
    idToIndex[id] = idx;

    return idx;
}

void Graph::addEdge(const int from, const int to, const int weight) {
    adj[from].push_back(Edge{to, weight});
    adj[to].push_back(Edge{from, weight});
}

int Graph::indexOf(const int &tmdbId) {
    const auto it = idToIndex.find(tmdbId);
    return it == idToIndex.end() ? -1 : it->second;
}

std::vector<Movie> Graph::getMovies() const{
    return movies;
}

std::vector<std::vector<Edge>> Graph::getAdj() const {
    return adj;
}
