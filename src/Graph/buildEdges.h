#ifndef MOVIERECOMMENDER_BUILDEDGES_H
#define MOVIERECOMMENDER_BUILDEDGES_H
#include "./graph.h"
#include "../MoviesUtil/similarityScore.h"


inline void buildEdges(Graph& graph) {
    const auto& movies = graph.getMovies();
    const int n = static_cast<int>(movies.size());

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            const double similarity = similarityScore(movies[i], movies[j]);
            if (similarity <= 0.0) {
                continue;
            }
            const double weight = weightFromSimilarity(similarity);
            graph.addEdge(i, j, weight);
        }
    }
    std::cout << "Edges:\n";
    const auto& adj = graph.getAdj();
    for (int u = 0; u < static_cast<int>(adj.size()); ++u) {
        for (auto& e : adj[u]) {
            std::cout << "  " << u << " -> " << e.to
                      << "  w=" << e.weight << "\n";
        }
    }
}


#endif //MOVIERECOMMENDER_BUILDEDGES_H