#ifndef MOVIERECOMMENDER_IMDBAPI_H
#define MOVIERECOMMENDER_IMDBAPI_H
#include <string>
#include <vector>
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "../MoviesUtil/Movie.h"

class TmdbAPI {
public:
    explicit TmdbAPI(std::string apiKey)
        : apiKey_(std::move(apiKey)),
          baseUrl_("https://api.themoviedb.org/3") {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~TmdbAPI() {
        curl_global_cleanup();
    }

    Movie fetchMovieById(int tmdbId) const {
        const std::string url = baseUrl_ + "/movie/" + std::to_string(tmdbId)
            + "?api_key=" + apiKey_ + "&language=en-US";

        nlohmann::json j = getJson(url);

        Movie m;
        m.tmdbId = j.value("id", tmdbId);
        m.name  = j.value("title", "");

        if (j.contains("genres")) {
            for (auto& g : j["genres"]) {
                if (g.contains("name") && !g["name"].is_null()) {
                    m.genres.push_back(g["name"].get<std::string>());
                }
            }
        }

        if (j.contains("vote_average") && !j["vote_average"].is_null())
            m.rating = j["vote_average"].get<double>();

        if (j.contains("release_date") && j["release_date"].is_string()) {
            std::string date = j["release_date"].get<std::string>(); // "YYYY-MM-DD"
            if (date.size() >= 4) {
                m.year = std::stoi(date.substr(0, 4));
            }
        }

        return m;
    }

    // GET /search/movie
    std::vector<Movie> searchByTitle(const std::string& query, int limit = 5) const {
        std::string url = baseUrl_ + "/search/movie?api_key=" + apiKey_
        + "&language=en-US&include_adult=false&page=1&query=" + urlEncode(query);

        nlohmann::json j = getJson(url);

        std::vector<Movie> out;
        if (!j.contains("results") || !j["results"].is_array())
            return out;

        int count = 0;
        for (auto& r : j["results"]) {
            if (count >= limit) break;

            Movie m;
            m.tmdbId = r.value("id", 0);
            m.name  = r.value("title", "");

            if (r.contains("vote_average") && !r["vote_average"].is_null())
                m.rating = r["vote_average"].get<double>();

            if (r.contains("release_date") && r["release_date"].is_string()) {
                std::string date = r["release_date"].get<std::string>();
                if (date.size() >= 4)
                    m.year = std::stoi(date.substr(0, 4));
            }

            if (m.tmdbId != 0 && !m.name.empty()) {
                out.push_back(std::move(m));
                ++count;
            }
        }

        return out;
    }

    std::vector<Movie> fetchPopularMovies(const int numPages) const {
        std::vector<Movie> result;
        if (numPages < 1) return result;

        for (int page = 1; page <= numPages; ++page) {
            std::string url = baseUrl_
                + "/movie/popular"
                + "?api_key=" + apiKey_
                + "&language=en-US"
                + "&page=" + std::to_string(page);

            nlohmann::json j = getJson(url);
            if (!j.contains("results") || !j["results"].is_array())
                continue;

            for (const auto& r : j["results"]) {
                Movie m;
                m.tmdbId = r.value("id", 0);
                m.name  = r.value("title", "");
                if (m.tmdbId == 0 || m.name.empty())
                    continue;

                if (r.contains("vote_average") && r["vote_average"].is_number())
                    m.rating = r["vote_average"].get<double>();

                if (r.contains("release_date") && r["release_date"].is_string()) {
                    std::string date = r["release_date"].get<std::string>();
                    if (date.size() >= 4)
                        m.year = std::stoi(date.substr(0, 4));
                }

                result.push_back(std::move(m));
            }
        }

        return result;
    }

private:
    std::string apiKey_;
    std::string baseUrl_;

    static size_t writeCallback(const char* ptr, size_t size, size_t nmemb, void* userdata) {
        auto* stream = static_cast<std::string*>(userdata);
        size_t count = size * nmemb;
        stream->append(ptr, count);
        return count;
    }

    static nlohmann::json getJson(const std::string& url) {
        CURL* curl = curl_easy_init();
        if (!curl) throw std::runtime_error("curl_easy_init failed");

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &TmdbAPI::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("curl_easy_perform failed: ")
                + curl_easy_strerror(res));
        }
        if (httpCode < 200 || httpCode >= 300) {
            throw std::runtime_error("TMDB HTTP error " + std::to_string(httpCode)
                + " | body: " + response);
        }

        try {
            return nlohmann::json::parse(response);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("JSON parse failed: ")
                + e.what() + " | body: " + response);
        }
    }

    static std::string urlEncode(const std::string& s) {
        static auto hex = "0123456789ABCDEF";
        std::string out;
        for (unsigned char c : s) {
            if (('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                ('0' <= c && c <= '9') ||
                c == '-' || c == '_' || c == '.' || c == '~') {
                out += c;
            } else if (c == ' ') {
                out += "%20";
            } else {
                out += '%';
                out += hex[c >> 4];
                out += hex[c & 0xF];
            }
        }
        return out;
    }
};


#endif //MOVIERECOMMENDER_IMDBAPI_H