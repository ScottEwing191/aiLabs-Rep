#ifndef _GRAPH_UTILS_HPP_
#define _GRAPH_UTILS_HPP_

#include <numeric>
#include "graph.hpp"

auto num_nodes(const Graph& g) {
  return std::distance(g.edges.cbegin(), g.edges.cend());
}

auto num_edges(const Graph& g) {
  auto f = [](const auto&x, const auto& y) { return x + y.second.size(); };
  return std::accumulate(g.edges.cbegin(), g.edges.cend(), 0, f);
}

void add_node(Graph& g, const node_t& n, const Vector& v) {
  g.edges[n] = {};
  node_info[n] = v;
}

// Add two edges to the Graph g: one between n1 and n2; and the other
// between n2 and n1. The costs are set at cost1 and cost2 respectively.
void add_double_edge(Graph& g, const node_t& n1, const node_t& n2,
                     const double cost1, const double cost2)
{
  g.edges[n1].push_back(n2);
  g.edges[n2].push_back(n1);
  edge_info[{n1,n2}] = cost1;
  edge_info[{n2,n1}] = cost2;
}

// Add two edges to the Graph g: one between n1 and n2; and the other
// between n2 and n1. The edge costs default to the euclidean distance.
void add_double_edge(Graph& g, const node_t& n1, const node_t& n2)
{
  const double cost = (node_info[n1]-node_info[n2]).length();
  add_double_edge(g, n1, n2, cost, cost);
}

void draw_graph(Graph& g)
{
  for (const auto& p : g.edges)
  {
    const auto& [n1, edges] = p;
    const Vector& pos_n1 = node_info[n1];

    for (const auto& n2 : edges) {
      const Vector& pos_n2 = node_info[n2];
      DrawLine(pos_n1.z, pos_n1.x, pos_n2.z, pos_n2.x, GRAY);
    }

    DrawCircle(pos_n1.z, pos_n1.x, 4, BLACK);
    DrawText(TextFormat("%c", n1), pos_n1.z + 2, pos_n1.x + 2, 26, BEIGE);
  }
}

#endif // _GRAPH_UTILS_HPP_
