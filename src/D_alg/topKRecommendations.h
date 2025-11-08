#ifndef MOVIERECOMMENDER_TOPKRECC_H
#define MOVIERECOMMENDER_TOPKRECC_H
#include <vector>
#include "../D_alg/dAlg.h"


inline std::vector<int> topKRecommendations(
    const int src,
    const DijkstraResult& res,
    const int k
    ) {
    std::vector<int> idx;
    const int n = static_cast<int>(res.distance.size());
    idx.reserve(n);

    for (int i = 0; i < n; ++i) {
        if (i == src) continue;
        if (!std::isfinite(res.distance[i])) continue;
        idx.push_back(i);
    }

    std::ranges::sort(idx, [&](const int a, const int b)
        {
            return res.distance[a] < res.distance[b];
        });

    if (static_cast<int>(idx.size()) > k) {
        idx.resize(k);
    }

    return idx;
}


#endif //MOVIERECOMMENDER_TOPKRECC_H