#ifndef MOVIERECOMMENDER_IMDBAPI_H
#define MOVIERECOMMENDER_IMDBAPI_H
#include <string>
#include <vector>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "../MoviesUtil/Movie.h"

class TmdbAPI {
public:
    explicit TmdbAPI(std::string apiKey);
    ~TmdbAPI();

    [[nodiscard]] Movie fetchMovieById(int tmdbId) const;

    [[nodiscard]] std::vector<Movie> searchByTitle(const std::string& query,
                                                   int limit = 5) const;

    [[nodiscard]] std::vector<Movie> fetchPopularMovies(int poolSize) const;

    [[nodiscard]] std::vector<Movie> searchMoviesByTitle(const std::string& query,
                                                         int limit = 5) const;

private:
    std::string apiKey_;
    std::string baseUrl_;

    static size_t writeCallback(const char* ptr, size_t size, size_t nmemb, void* userdata);
    static nlohmann::json getJson(const std::string& url);
    static std::string urlEncode(const std::string& s);

    [[nodiscard]] std::unordered_map<int, std::string> fetchGenreMap() const;
};


#endif //MOVIERECOMMENDER_IMDBAPI_H