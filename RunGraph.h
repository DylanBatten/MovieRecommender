#ifndef MOVIERECOMMENDER_RUNGRAPH_H
#define MOVIERECOMMENDER_RUNGRAPH_H
#include <iostream>
#include <cctype>
#include "./ImdbAPI/ImdbAPI.h"
#include "./MoviesUtil/Movie.h"
#include "./Graph/graph.h"
#include "./D_alg/dAlg.h"
#include "./D_alg/topKRecommendations.h"
#include "./Graph/loadgraph.h"
#include "./Graph/buildKNNGraph.h"


inline int runGraph() {
    try {
        TmdbAPI api("c9a60d0459daa5ba1f1de1f284b07980");

        int poolSize = 0;
        std::cout << "Enter movie pool size (number of popular movies to load): ";
        std::cin >> poolSize;


        std::cout << "Fetching popular movies from TMDB...\n";
        std::vector<Movie> moviesFromApi = api.fetchPopularMovies(poolSize);


        if (moviesFromApi.empty()) {
            throw std::runtime_error("No movies fetched from TMDB. Check your token or network.");
        }

        std::cout << "Fetched " << moviesFromApi.size() << " movies.\n";

        Graph g;
        for (const auto& m : moviesFromApi) {
            g.addMovie(m);
        }
        std::cout << "Graph nodes: " << g.getMovies().size() << "\n";

        constexpr int K_NEIGHBORS = 20;
        std::cout << "Building similarity graph (K=" << K_NEIGHBORS << ")...\n";
        buildKNNGraph(g, K_NEIGHBORS);
        std::cout << "Graph built.\n";

        while (true) {

            std::cout << "\nEnter a movie title or exit to quit program: ";
            std::string titleInput;
            std::getline(std::cin >> std::ws, titleInput);
            if (titleInput == "exit") {
                break;
            }

            auto candidates = api.searchMoviesByTitle(titleInput, 5);

            if (candidates.empty()) {
                std::cout << "No movies found for \"" << titleInput << "\".\n";
                continue;
            }

            char chosen = '0';
            if (candidates.size() > 1) {
                while (true) {
                    std::cout << "\nSelect which movie you meant:\n";
                    for (int i = 0; i < static_cast<int>(candidates.size()); ++i) {
                        const auto& m = candidates[i];
                        std::cout << "  [" << i << "] " << m.name;
                        if (m.year > 0) {
                            std::cout << " (" << m.year << ")\n";
                        } else {
                            std::cout << " (Not found)\n";
                        }
                    }
                    std::cout << "Enter choice [0-" << static_cast<int>(candidates.size()) - 1 << "]: ";
                    std::cin >> chosen;
                    if (!std::isdigit(chosen)) {
                        std::cout << "Invalid choice.\n";
                        continue;
                    }
                    chosen = chosen - '0';
                    if (chosen < 0 || chosen >= static_cast<int>(candidates.size())) {
                        std::cout << "Invalid choice.\n";
                        continue;
                    }
                    std::cout << "\n" << std::endl;
                    break;
                }
            }

            Movie seed1 = candidates[chosen];

            seed1 = api.fetchMovieById(seed1.tmdbId);
            int seedId = seed1.tmdbId;

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
            std::cout << "\nSource movie: " << movies[src].name << std::endl;

            auto res = dijkstra(src, g.getAdj());

            constexpr int TOP_K = 10;
            auto top = topKRecommendations(src, res, TOP_K);

            std::cout << "\nTop " << TOP_K << " recommended movies:\n";
            for (int idx : top) {
                std::cout << "  " << movies[idx].name << " (" << movies[idx].year << ")" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}


#endif //MOVIERECOMMENDER_RUNGRAPH_H