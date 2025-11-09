#ifndef MOVIERECOMMENDER_GRAPH_H
#define MOVIERECOMMENDER_GRAPH_H
#include <unordered_map>
#include <vector>
#include "../MoviesUtil/Movie.h"

struct Edge {
    int to;
    double weight;
};

class Graph {

public:
    int addMovie(const Movie &movie);
    void addEdge(int from, int to, double weight);
    int indexOf(const int &imdbId);
    [[nodiscard]] std::vector<Movie> getMovies() const;
    [[nodiscard]] std::vector<std::vector<Edge>> getAdj() const;

private:
    std::vector<Movie> movies;
    std::vector<std::vector<Edge>> adj;
    std::unordered_map<int, int> idToIndex;
};

#endif //MOVIERECOMMENDER_GRAPH_H