cmake_minimum_required(VERSION 4.0)
project(MovieRecommender)

set(CMAKE_CXX_STANDARD 20)

add_executable(MovieRecommender main.cpp
        src/Graph/graph.cpp
        src/Graph/graph.h
        src/D_alg/dAlg.cpp
        src/D_alg/dAlg.h
        src/MoviesUtil/Movie.cpp
        src/MoviesUtil/Movie.h
        src/MoviesUtil/similarityScore.cpp
        src/MoviesUtil/similarityScore.h
        src/Graph/buildEdges.cpp
        src/Graph/buildEdges.h
        src/D_alg/topKRecommendations.cpp
        src/D_alg/topKRecommendations.h
        src/ImdbAPI/ImdbAPI.cpp
        src/ImdbAPI/ImdbAPI.h
        src/MoviesRepo/MoviesRepository.cpp
        src/MoviesRepo/MoviesRepository.h
        src/Graph/buildGlobalGraph.cpp
        src/Graph/buildGlobalGraph.h
        src/Graph/loadgraph.cpp
        src/Graph/loadgraph.h
        src/Graph/savegraph.cpp
        src/Graph/savegraph.h
        src/Graph/buildKNNGraph.cpp
        src/Graph/buildKNNGraph.h)

target_include_directories(MovieRecommender PRIVATE
        ${CMAKE_SOURCE_DIR}/single_include
        ${CMAKE_SOURCE_DIR}/src
)

find_package(CURL REQUIRED)
target_link_libraries(MovieRecommender PRIVATE CURL::libcurl)