#include <iostream>
#include <string>
#include "RunGraph.h"
#include "src/Benchmarking/benchmark.h"
#include "Graph/loadgraph.h"
#include "Graph/savegraph.h"
#include "Graph/buildKNNGraph.h"

void runBenchmarkDemo() {
    std::cout << "=== Movie Recommender Benchmarking ===\n\n";
    
    // Try to load existing graph first
    Graph graph;
    try {
        std::cout << "Attempting to load graph from disk...\n";
        graph = loadGraphFromDisk("movie_graph.json");
        std::cout << "✓ Graph loaded successfully!\n";
    } catch (const std::exception& e) {
        std::cout << "✗ Could not load graph: " << e.what() << "\n";
        std::cout << "Please build the graph first using option 1.\n";
        return;
    }
    
    const auto& movies = graph.getMovies();
    if (movies.empty()) {
        std::cout << "✗ No movies found in graph.\n";
        return;
    }
    
    std::cout << "\nLoaded " << movies.size() << " movies.\n\n";
    
    // Display some sample movies for user to choose from
    std::cout << "Sample Movies (0-" << std::min(9, (int)movies.size()-1) << "):\n";
    for (int i = 0; i < std::min(10, (int)movies.size()); ++i) {
        std::cout << "  [" << i << "] " << movies[i].name 
                  << " (" << movies[i].year << ") - Rating: " << movies[i].rating << "\n";
    }
    
    // Get user input
    int sourceIndex, k;
    std::cout << "\nEnter source movie index (0-" << movies.size()-1 << "): ";
    std::cin >> sourceIndex;
    
    if (sourceIndex < 0 || sourceIndex >= movies.size()) {
        std::cout << "Invalid movie index!\n";
        return;
    }
    
    std::cout << "Enter number of recommendations (K): ";
    std::cin >> k;
    
    if (k <= 0 || k > 100) {
        std::cout << "K should be between 1 and 100.\n";
        return;
    }
    
    // Run the benchmark comparison
    Benchmark::compareAlgorithms(graph, sourceIndex, k);
}

int main() {
    std::cout << "Movie Recommender System\n";
    std::cout << "========================\n";
    std::cout << "1. Build and Run Graph Recommender\n";
    std::cout << "2. Benchmark Algorithms (Graph vs Heap)\n";
    std::cout << "3. Exit\n";
    std::cout << "Choose option: ";
    
    char choice;
    std::cin >> choice;
    
    switch (choice) {
        case '1':
            runGraph();
            break;
        case '2':
            runBenchmarkDemo();
            break;
        case '3':
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid option!\n";
            break;
    }
    
    return 0;
}