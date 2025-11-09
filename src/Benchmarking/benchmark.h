#ifndef MOVIERECOMMENDER_BENCHMARK_H
#define MOVIERECOMMENDER_BENCHMARK_H

#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Graph/graph.h"
#include "D_alg/dAlg.h"
#include "D_alg/topKRecommendations.h"
#include "../Heap/heapTopK.h"

class Benchmark {
public:
    struct BenchmarkResult {
        double time_ms;
        std::vector<int> recommendations;
        size_t memory_usage_kb;
    };

    static void compareAlgorithms(Graph& graph, int sourceMovieIndex, int k = 10) {
        std::cout << "\n=== ALGORITHM COMPARISON ===\n";
        std::cout << "Source Movie: " << graph.getMovies()[sourceMovieIndex].name << "\n";
        std::cout << "K: " << k << "\n";
        std::cout << "Total Movies: " << graph.getMovies().size() << "\n\n";

        auto graphResult = benchmarkGraphApproach(graph, sourceMovieIndex, k);

        auto heapResult = benchmarkHeapApproach(graph, sourceMovieIndex, k);

        printComparison(graphResult, heapResult, graph, sourceMovieIndex, k);
    }

private:
    static BenchmarkResult benchmarkGraphApproach(Graph& graph, int sourceIndex, int k) {
        auto start = std::chrono::high_resolution_clock::now();

        auto dijkstraResult = dijkstra(sourceIndex, graph.getAdj());

        auto recommendations = topKRecommendations(sourceIndex, dijkstraResult, k);

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();

        return {duration, recommendations, 0};
    }

    static BenchmarkResult benchmarkHeapApproach(Graph& graph, int sourceIndex, int k) {
        auto start = std::chrono::high_resolution_clock::now();

        const auto& movies = graph.getMovies();
        const auto& sourceMovie = movies[sourceIndex];

        auto recommendations = heapTopKRecommendations(sourceMovie, movies, sourceIndex, k);

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();

        return {duration, recommendations, 0};
    }

    static void printComparison(const BenchmarkResult& graphResult,
                               const BenchmarkResult& heapResult,
                               Graph& graph,
                               int sourceIndex,
                               int k) {
        const auto& movies = graph.getMovies();
        const auto& sourceMovie = movies[sourceIndex];

        std::cout << "RESULTS:\n";
        std::cout << std::string(50, '-') << "\n";

        std::cout << "TIME PERFORMANCE:\n";
        std::cout << "  Graph (Dijkstra): " << std::fixed << std::setprecision(3)
                  << graphResult.time_ms << " ms\n";
        std::cout << "  Heap-based:       " << std::fixed << std::setprecision(3)
                  << heapResult.time_ms << " ms\n";
        std::cout << "  Speedup:          " << std::fixed << std::setprecision(2)
                  << (graphResult.time_ms / heapResult.time_ms) << "x\n";

        std::cout << "\nTOP-" << k << " RECOMMENDATIONS:\n";
        std::cout << "Graph-based (Dijkstra):\n";
        printRecommendations(graphResult.recommendations, movies, sourceMovie);

        std::cout << "\nHeap-based:\n";
        printRecommendations(heapResult.recommendations, movies, sourceMovie);

        std::cout << "\nALGORITHM CONSISTENCY:\n";
        checkRecommendationOverlap(graphResult.recommendations, heapResult.recommendations, k);
    }

    static void printRecommendations(const std::vector<int>& indices,
                                    const std::vector<Movie>& movies,
                                    const Movie& sourceMovie) {
        for (size_t i = 0; i < indices.size(); ++i) {
            int movieIdx = indices[i];
            double similarity = similarityScore(sourceMovie, movies[movieIdx]);
            std::cout << "  " << (i + 1) << ". " << movies[movieIdx].name
                      << " (similarity: " << std::fixed << std::setprecision(3)
                      << similarity << ")\n";
        }
    }

    static void checkRecommendationOverlap(const std::vector<int>& graphRecs,
                                         const std::vector<int>& heapRecs,
                                         int k) {
        std::vector<int> graphSet(graphRecs.begin(), graphRecs.end());
        std::vector<int> heapSet(heapRecs.begin(), heapRecs.end());

        std::sort(graphSet.begin(), graphSet.end());
        std::sort(heapSet.begin(), heapSet.end());

        std::vector<int> intersection;
        std::set_intersection(graphSet.begin(), graphSet.end(),
                             heapSet.begin(), heapSet.end(),
                             std::back_inserter(intersection));

        double overlapPercent = (intersection.size() * 100.0) / k;
        std::cout << "  Overlap: " << intersection.size() << "/" << k
                  << " (" << std::fixed << std::setprecision(1)
                  << overlapPercent << "%)\n";

        if (overlapPercent < 70.0) {
            std::cout << "  ⚠️  Significant difference in recommendations!\n";
        } else if (overlapPercent > 90.0) {
            std::cout << "  ✅ High consistency between algorithms\n";
        }
    }
};

#endif //MOVIERECOMMENDER_BENCHMARK_H