#ifndef MOVIERECOMMENDER_MOVIESREPOSITORY_H
#define MOVIERECOMMENDER_MOVIESREPOSITORY_H
#include "ImdbAPI/ImdbAPI.h"



class MoviesRepository {
public:
    explicit MoviesRepository(TmdbAPI& api) : api(api) {}

    const Movie& getMovie(int tmdbId) {
        if (const auto it = cache.find(tmdbId); it != cache.end()) return it->second;

        Movie m = api.fetchMovieById(tmdbId);
        auto [pos, _] = cache.emplace(tmdbId, std::move(m));
        return pos->second;
    }
private:
    TmdbAPI& api;
    std::unordered_map<int, Movie> cache;
};


#endif //MOVIERECOMMENDER_MOVIESREPOSITORY_H