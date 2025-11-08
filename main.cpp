#include <iostream>
#include "./ImdbAPI/ImdbAPI.h"
#include "./MoviesUtil/Movie.h"
#include "./Graph/graph.h"
#include "./D_alg/dAlg.h"
#include "./D_alg/topKRecommendations.h"
#include "./Graph/loadgraph.h"
#include "./Graph/buildKNNGraph.h"

int main() {
    try {
        TmdbAPI api("c9a60d0459daa5ba1f1de1f284b07980");

        constexpr int NUM_PAGES = 100;
        std::cout << "Fetching popular movies from TMDB (" << NUM_PAGES << " pages)...\n";
        std::vector<Movie> moviesFromApi = api.fetchPopularMovies(NUM_PAGES);


        if (moviesFromApi.empty()) {
            throw std::runtime_error("No movies fetched from TMDB. Check your token or network.");
        }

        std::cout << "Fetched " << moviesFromApi.size() << " movies.\n";

        Graph g;
        for (const auto& m : moviesFromApi) {
            g.addMovie(m);
        }
        std::cout << "Graph nodes: " << g.getMovies().size() << "\n";

        constexpr int K_NEIGHBORS = 30;
        std::cout << "Building similarity graph (K=" << K_NEIGHBORS << ")...\n";
        buildKNNGraph(g, K_NEIGHBORS);
        std::cout << "Graph built.\n";

        std::cout << "\nEnter a TMDB movie ID to get recommendations: ";
        int seedId;
        std::cin >> seedId;

        int src = g.indexOf(seedId);

        if (src == -1) {
            std::cout << "Seed not in graph. Fetching from TMDB and inserting...\n";
            Movie seed = api.fetchMovieById(seedId);
            src = g.addMovie(seed);

            std::vector<Movie> all = g.getMovies();
            int n = static_cast<int>(all.size());

            std::vector<std::pair<double,int>> sims;
            sims.reserve(n);
            for (int i = 0; i < n - 1; ++i) {
                if (double sim = similarityScore(seed, all[i]); sim > 0.0)
                    sims.emplace_back(sim, i);
            }

            constexpr int K = K_NEIGHBORS;
            if (!sims.empty()) {
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

                for (auto& [sim, i] : sims) {
                    double w = weightFromSimilarity(sim);
                    g.addEdge(src, i, w);
                }
            }

            std::cout << "Seed movie '" << seed.name << "' added at index " << src << ".\n";
        }

        const auto& movies = g.getMovies();
        std::cout << "\nSource movie: " << movies[src].name
                  << " (tmdbId=" << movies[src].tmdbId << ")\n";

        auto res = dijkstra(src, g.getAdj());

        constexpr int TOP_K = 10;
        auto top = topKRecommendations(src, res, TOP_K);

        std::cout << "\nTop " << TOP_K << " recommended movies:\n";
        for (int idx : top) {
            std::cout << "  " << movies[idx].name
                      << "  (tmdbId=" << movies[idx].tmdbId
                      << ", distance=" << res.distance[idx] << ")\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}