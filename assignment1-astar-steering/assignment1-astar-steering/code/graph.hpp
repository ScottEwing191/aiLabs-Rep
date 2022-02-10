#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <numeric>
#include <unordered_map>
#include <vector>
#include <utility>        // std::pair
#include <cmath>
#include <functional>

using node_t = char;

inline double heuristic(node_t, node_t);
#include "redblob_code/implementation.hpp"

namespace std
{
  template <typename T, typename U>
  struct hash<std::pair<T,U>>
  {
    std::size_t operator()(const std::pair<T,U>& n) const {
      return std::hash<T>{}(n.first) ^ std::hash<U>{}(n.second);
    }
  };
}

using edge_t = std::pair<node_t, node_t>;
std::unordered_map<node_t, Vector> node_info;
std::unordered_map<edge_t, double> edge_info;

struct Graph
{
  std::unordered_map<node_t, std::vector<node_t>> edges;

  std::vector<node_t> neighbors(node_t id) {
    return edges[id];
  }

  double cost(node_t current, node_t next) { return edge_info[{current,next}]; }
};

inline double heuristic(node_t next, node_t goal) {
  const Vector& npos = node_info[next];
  const Vector& gpos = node_info[goal];
  return std::abs(npos.x - gpos.x) + std::abs(npos.y - gpos.y);
}

#endif // _GRAPH_HPP_
