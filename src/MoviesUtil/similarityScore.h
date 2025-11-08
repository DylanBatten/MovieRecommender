#ifndef MOVIERECOMMENDER_SIMILARITYSCORE_H
#define MOVIERECOMMENDER_SIMILARITYSCORE_H
#include <iostream>
#include <unordered_set>
#include "../MoviesUtil/Movie.h"


inline double similarityScore(const Movie& a, const Movie& b) {
    double score = 0.0;

    const std::unordered_set<std::string> ga(a.genres.begin(), a.genres.end());
    int commonGenres = 0;
    for (const auto& genre : b.genres) {
        if (ga.contains(genre)) commonGenres++;
    }

    score += 1.5 * commonGenres;

    const double diff = std::fabs(a.rating - b.rating);
    score += std::max(0.0, 2.0 - diff);

    int yearDiff = std::abs(a.year - b.year);
    if (yearDiff <= 5) {
        score += 0.5;
    }
    return score;
}

inline double weightFromSimilarity(double similarity) {
    constexpr double eps = 1e-3;
    return 1000 * (1.0 / (similarity + eps));
}


#endif //MOVIERECOMMENDER_SIMILARITYSCORE_H