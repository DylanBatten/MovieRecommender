#ifndef MOVIERECOMMENDER_BUILDGLOBALGRAPH_H
#define MOVIERECOMMENDER_BUILDGLOBALGRAPH_H
#include "Graph.h"
#include "../MoviesUtil/similarityScore.h"

inline void buildGlobalGraph(Graph& g, const std::vector<Movie>& all) {
    for (const auto& m : all)
        g.addMovie(m);

    const auto& movies = g.getMovies();
    const int n = static_cast<int>(movies.size());

    std::vector<std::pair<double,int>> sims;
    sims.reserve(n);

    for (int i = 0; i < n; ++i) {
        sims.clear();
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            if (double sim = similarityScore(movies[i], movies[j]); sim > 0.0) sims.push_back({sim, j});
        }

        if (sims.empty()) continue;

        if (int K = 40; static_cast<int>(sims.size()) > K) {
            std::ranges::nth_element(sims, sims.begin() + K
                                     ,
                                     [](auto& a, auto& b){ return a.first > b.first; }
            );
            sims.resize(K);
        } else {
            std::ranges::sort(sims
                              ,
                              [](auto& a, auto& b){ return a.first > b.first; }
            );
        }

        for (auto& [sim, j] : sims) {
            const double w = weightFromSimilarity(sim);
            g.addEdge(i, j, w);
        }
    }
}


#endif //MOVIERECOMMENDER_BUILDGLOBALGRAPH_H