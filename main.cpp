#include <iostream>
#include "./ImdbAPI/ImdbAPI.h"
#include "./MoviesUtil/Movie.h"
#include "./Graph/graph.h"
#include "./D_alg/dAlg.h"
#include "./D_alg/topKRecommendations.h"
#include "./Graph/loadgraph.h"
#include "./Graph/buildKNNGraph.h"

using std::cout;
using std::cin;
using std::endl;

int main() {
    try {
        // 1. Init TMDB API (v4 read access token recommended)
        TmdbAPI api("c9a60d0459daa5ba1f1de1f284b07980");

        // 2. Pull a large list of movies from TMDB
        //    Tune numPages based on rate limits & performance: 5 => ~100 movies, 25 => ~500, etc.
        constexpr int NUM_PAGES = 100;
        std::cout << "Fetching popular movies from TMDB (" << NUM_PAGES << " pages)...\n";
        std::vector<Movie> moviesFromApi = api.fetchPopularMovies(NUM_PAGES);


        if (moviesFromApi.empty()) {
            throw std::runtime_error("No movies fetched from TMDB. Check your token or network.");
        }

        std::cout << "Fetched " << moviesFromApi.size() << " movies.\n";

        // 3. Build graph storage
        Graph g;
        for (const auto& m : moviesFromApi) {
            g.addMovie(m);
        }
        std::cout << "Graph nodes: " << g.getMovies().size() << "\n";

        // 4. Build KNN similarity graph (edges)
        constexpr int K_NEIGHBORS = 30;  // per node; tune as desired
        std::cout << "Building similarity graph (K=" << K_NEIGHBORS << ")...\n";
        buildKNNGraph(g, K_NEIGHBORS);
        std::cout << "Graph built.\n";

        // 5. Ask user for a seed movie by TMDB ID
        std::cout << "\nEnter a TMDB movie ID to get recommendations: ";
        int seedId;
        std::cin >> seedId;

        int src = g.indexOf(seedId);

        if (src == -1) {
            // Not already in graph → fetch details, add, connect to graph
            std::cout << "Seed not in graph. Fetching from TMDB and inserting...\n";
            Movie seed = api.fetchMovieById(seedId);
            src = g.addMovie(seed);

            // Connect new seed node to existing graph using similarity
            std::vector<Movie> all = g.getMovies();
            int n = static_cast<int>(all.size());

            std::vector<std::pair<double,int>> sims;
            sims.reserve(n);
            for (int i = 0; i < n - 1; ++i) { // existing nodes
                if (double sim = similarityScore(seed, all[i]); sim > 0.0)
                    sims.push_back({sim, i});
            }

            constexpr int K = K_NEIGHBORS;
            if (!sims.empty()) {
                if (static_cast<int>(sims.size()) > K) {
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

        // 6. Run Dijkstra over your graph storage
        auto res = dijkstra(src, g.getAdj());

        // 7. Extract top-K recommendations based on shortest path distance
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