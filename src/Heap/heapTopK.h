#ifndef MOVIERECOMMENDER_HEAPTOPK_H
#define MOVIERECOMMENDER_HEAPTOPK_H

#include <vector>
#include <queue>
#include <functional>
#include <algorithm>
#include "../MoviesUtil/Movie.h"
#include "../MoviesUtil/similarityScore.h"

inline std::vector<int> heapTopKRecommendations(
    const Movie& sourceMovie,
    const std::vector<Movie>& allMovies,
    int sourceIndex,
    int k) {

    using HeapElement = std::pair<double, int>;
    std::priority_queue<HeapElement, std::vector<HeapElement>, 
                       std::greater<HeapElement>> minHeap;

    for (int i = 0; i < static_cast<int>(allMovies.size()); ++i) {
        if (i == sourceIndex) continue;
        
        const Movie& targetMovie = allMovies[i];
        double similarity = similarityScore(sourceMovie, targetMovie);

        if (static_cast<int>(minHeap.size()) < k) {
            minHeap.push({similarity, i});
        } 

        else if (similarity > minHeap.top().first) {
            minHeap.pop();
            minHeap.push({similarity, i});
        }
    }

    std::vector<HeapElement> results;
    results.reserve(minHeap.size());
    while (!minHeap.empty()) {
        results.push_back(minHeap.top());
        minHeap.pop();
    }

    std::ranges::reverse(results);

    std::vector<int> topKIndices;
    topKIndices.reserve(results.size());
    for (const auto& [similarity, index] : results) {
        topKIndices.push_back(index);
    }
    
    return topKIndices;
}

#endif //MOVIERECOMMENDER_HEAPTOPK_H