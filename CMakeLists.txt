cmake_minimum_required(VERSION 3.30.5)
project(MovieRecommender)

set(CMAKE_CXX_STANDARD 20)

add_executable(MovieRecommender main.cpp
src/Graph/graph.cpp
src/Graph/graph.h
src/D_alg/dAlg.h
src/MoviesUtil/Movie.h
src/MoviesUtil/similarityScore.h
src/Graph/buildEdges.h
src/D_alg/topKRecommendations.h
src/ImdbAPI/ImdbAPI.cpp
src/ImdbAPI/ImdbAPI.h
src/MoviesRepo/MoviesRepository.h
src/Graph/buildGlobalGraph.h
src/Graph/loadgraph.h
src/Graph/savegraph.h
src/Graph/buildKNNGraph.h
RunGraph.h
src/Benchmarking/benchmark.h
src/Heap/heapTopK.h)

target_include_directories(MovieRecommender PRIVATE
${CMAKE_SOURCE_DIR}/single_include
${CMAKE_SOURCE_DIR}/src
)

set(CURL_INCLUDE_DIR "C:/curl/curl-8.17.0_1-win64-mingw/include")
set(CURL_LIBRARY "C:/curl/curl-8.17.0_1-win64-mingw/lib/libcurl.dll.a")
set(OPENSSL_LIB_PATH "C:/curl/curl-8.17.0_1-win64-mingw/lib")

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(MovieRecommender PRIVATE
${CURL_LIBRARY}
${OPENSSL_LIB_PATH}/libssl.a
${OPENSSL_LIB_PATH}/libcrypto.a
ws2_32
wldap32
crypt32
z
)
