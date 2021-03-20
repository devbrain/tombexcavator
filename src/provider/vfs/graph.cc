//
// Created by igor on 20/03/2021.
//

#include "graph.hh"
#include <set>
#include <stack>


namespace provider::vfs::detail
{
    void graph::add_edge(const vertex_t& v, const vertex_t& w)
    {
        m_graph[v].push_back(w);
    }
    // ---------------------------------------------------------------------------------------
    std::vector<graph::vertex_t> graph::topological_sort() const
    {
        std::stack<vertex_t> stack;
        std::set<vertex_t> visited;

        // Call the recursive helper function to store Topological
        // Sort starting from all vertices one by one
        for (const auto& [v, w] : m_graph)
        {
            if (visited.find(v) == visited.end())
            {
                topological_sort_helper(v, visited, stack);
            }
        }
        std::vector<vertex_t> result;
        // Print contents of stack
        while (!stack.empty())
        {
            result.push_back(stack.top());
            stack.pop();
        }

        return result;
    }
    // -----------------------------------------------------------------------------------------
    void graph::topological_sort_helper(const vertex_t& v,
                                 std::set<vertex_t>& visited,
                                 std::stack<vertex_t>& stack) const
    {
        // Mark the current node as visited.
        visited.insert(v);
        // Recur for all the vertices adjacent to this vertex
        auto itr = m_graph.find(v);
        for (const auto& w : itr->second)
        {
            if (visited.find(w) == visited.end())
            {
                topological_sort_helper(w, visited, stack);
            }
        }
        // Push current vertex to stack which stores result
        stack.push(v);
    }
} // ns
