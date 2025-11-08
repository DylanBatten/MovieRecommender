#ifndef MOVIERECOMMENDER_MOVIE_H
#define MOVIERECOMMENDER_MOVIE_H
#include <string>
#include <vector>


struct Movie {
    int tmdbId = 0;
    std::string name;
    std::vector<std::string> genres;
    double rating = 0.0;
    int year = 0;
};


#endif //MOVIERECOMMENDER_MOVIE_H