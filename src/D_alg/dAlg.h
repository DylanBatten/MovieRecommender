#ifndef MOVIERECOMMENDER_DALG_H
#define MOVIERECOMMENDER_DALG_H
#include <queue>
#include <vector>
#include "../Graph/graph.h"

struct DijkstraResult {
    std::vector<double> distance;
    std::vector<int> parent;
};

struct NodeState {
    double dist;
    int node;
};

struct CompareState {
    bool operator()(const NodeState& a, const NodeState& b) const {
        return a.dist > b.dist;
    }
};

inline DijkstraResult dijkstra(const int src, const std::vector<std::vector<Edge>>& adj) {
    constexpr double INF = std::numeric_limits<double>::infinity();
    const int n = static_cast<int>(adj.size());

    std::vector<double> dist(n, INF);
    std::vector<int> parent(n, -1);
    dist[src] = 0.0;

    std::priority_queue<NodeState, std::vector<NodeState>, CompareState> pq;
    pq.push(NodeState{0.0, src});

    while (!pq.empty()) {
        NodeState cur = pq.top();
        pq.pop();

        const int u = cur.node;
        const double d = cur.dist;

        if (d > dist[u]) continue;

        for (const Edge& e : adj[u]) {
            const int v = e.to;
            if (const double nd = d + e.weight; nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                pq.push(NodeState{nd, v});
            }
        }
    }

    return {dist, parent};
}

inline std::vector<int> buildPath(const int target, const std::vector<int>& parent) {
    std::vector<int> path;
    for (int curr = target; curr != -1; curr = parent[curr]) {
        path.push_back(curr);
    }
    std::ranges::reverse(path);
    return path;
}


#endif //MOVIERECOMMENDER_DALG_H