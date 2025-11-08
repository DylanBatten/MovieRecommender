#ifndef MOVIERECOMMENDER_BUILDKNNGRAPH_H
#define MOVIERECOMMENDER_BUILDKNNGRAPH_H
#include "./Graph/graph.h"
#include "./MoviesUtil/similarityScore.h"

inline void buildKNNGraph(Graph& g, const int K) {
    const auto& movies = g.getMovies();
    int n = static_cast<int>(movies.size());
    if (n == 0) return;

    std::vector<std::pair<double,int>> sims;
    sims.reserve(n);

    for (int i = 0; i < n; ++i) {
        sims.clear();

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            double sim = similarityScore(movies[i], movies[j]);
            if (sim > 0.0)
                sims.push_back({sim, j});
        }

        if (sims.empty()) continue;

        if (static_cast<int>(sims.size()) > K) {
            std::ranges::nth_element(sims, sims.begin() + K,
            [](auto& a, auto& b){ return a.first > b.first; }
            );
            sims.resize(K);
        } else {
            std::ranges::sort(sims,
            [](auto& a, auto& b){ return a.first > b.first; }
            );
        }

        for (auto& [sim, j] : sims) {
            double w = weightFromSimilarity(sim);
            g.addEdge(i, j, w);
        }
    }
}


#endif //MOVIERECOMMENDER_BUILDKNNGRAPH_H