//
// Created by igor on 20/03/2021.
//

#ifndef TOMBEXCAVATOR_GRAPH_HH
#define TOMBEXCAVATOR_GRAPH_HH

#include <string>
#include <map>
#include <vector>
#include <set>
#include <stack>

namespace provider::vfs::detail
{
    class graph
    {
    public:
        using vertex_t = std::string;
    public:
        void add_edge(const vertex_t& v, const vertex_t& w);

        std::vector<vertex_t> topological_sort() const;
    private:
        using adj_t = std::vector<vertex_t>;
        using graph_t = std::map<vertex_t, adj_t>;
    private:
        void topological_sort_helper(const vertex_t& v,
                                     std::set<vertex_t>& visited,
                                     std::stack<vertex_t>& stack) const;
    private:
        graph_t m_graph;
    };
}

#endif //TOMBEXCAVATOR_GRAPH_HH
