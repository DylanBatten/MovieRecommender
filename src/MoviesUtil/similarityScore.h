#ifndef MOVIERECOMMENDER_SIMILARITYSCORE_H
#define MOVIERECOMMENDER_SIMILARITYSCORE_H
#include <iostream>
#include <unordered_set>
#include "../MoviesUtil/Movie.h"


inline double similarityScore(const Movie& a, const Movie& b) {
    double score = 0.0;
    double weightSum = 0.0;


    if (!a.genres.empty() && !b.genres.empty()) {
        double genreSim = 0.0;
        const std::unordered_set ga(a.genres.begin(), a.genres.end());
        int inter = 0;
        for (const auto& g : b.genres) {
            if (ga.contains(g)) ++inter;
        }

        if (const int uni = static_cast<int>(ga.size()) + static_cast<int>(b.genres.size()) - inter; uni > 0) {
            genreSim = static_cast<double>(inter) / uni;
        }

        if (inter == 0) {
            return 0.0;
        }

        constexpr double wGenre = 0.7;
        score += wGenre * genreSim;
        weightSum += wGenre;
    }

    // ---------- Rating similarity ----------
    if (a.rating > 0.0 && b.rating > 0.0) {
        const double diff = std::fabs(a.rating - b.rating);
        const double rSim = std::max(0.0, 1.0 - diff / 1.5);

        constexpr double wRating = 0.2;
        score += wRating * rSim;
        weightSum += wRating;
    }

    // ---------- Year similarity ----------
    if (a.year > 0 && b.year > 0) {
        const int diff = std::abs(a.year - b.year);

        const double ySim = std::max(0.0, 1.0 - static_cast<double>(diff) / 12.0);

        constexpr double wYear = 0.1;
        score += wYear * ySim;
        weightSum += wYear;
    }

    if (weightSum == 0.0) {
        return 0.0;
    }

    const double finalSim = score / weightSum;

    if (finalSim < 0.15) {
        return 0.0;
    }

    return finalSim;
}

inline double weightFromSimilarity(const double similarity) {
    if (similarity <= 0.0) {
        return 1e9;
    }
    constexpr double eps = 1e-6;

    return 1000.0 / (similarity + eps);
}


#endif //MOVIERECOMMENDER_SIMILARITYSCORE_H