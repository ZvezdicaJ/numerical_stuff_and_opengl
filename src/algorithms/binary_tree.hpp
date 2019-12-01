#pragma once
#include "bitonic_sort.hpp"
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstring>

enum class TREE_TYPE { BALANCED, UNBALANCED };

template <typename T, TREE_TYPE B = TREE_TYPE::BALANCED>
class binary_tree {
  private:
    aligned_vector<T> tree;

    // vector of bools does not work well with std::memset
    aligned_vector<uint8_t> node_exist;
    std::function<bool(T &, T &)> comparator;
    unsigned num_nodes = 0;

    void construct_tree(T *elements_) {
        // if you want a balanced tree, you have to sort elements
        if constexpr (B == TREE_TYPE::BALANCED) {
            if constexpr (std::is_same_v<T, float> ||
                          std::is_same_v<T, double> ||
                          std::is_same_v<T, int>) {
                BITONIC_SORT::sort_vector(elements_, num_nodes);
            } else {
                std::sort(elements_, elements_ + num_nodes,
                          comparator);
            }
        };

        aligned_vector<std::pair<int, int>> intervals = {
            {0, num_nodes - 1}};

        unsigned max_depth =
            (unsigned)std::log2f((double)num_nodes);

        //   std::cout << "max depth: " << max_depth << std::endl;
        // std::cout << "num nodes: " << 1 + ((1 << max_depth) - 1)
        // * 2
        //          << std::endl;

        tree.reserve(1 + ((1 << max_depth) - 1) << 2);
        tree.resize(1 + ((1 << max_depth) - 1) << 2);
        node_exist.reserve(1 + ((1 << (max_depth + 1)) - 1) << 2);
        node_exist.resize(1 + ((1 << (max_depth + 1)) - 1) << 2);
        // std::fill(node_exist.begin(), node_exist.end(),
        //          false);

        std::memset(reinterpret_cast<void *>(node_exist.data()), 0,
                    node_exist.size());

        for (int depth = 0; depth <= max_depth; depth++) {
            aligned_vector<std::pair<int, int>> new_intervals;
            new_intervals.reserve(1 << (depth + 1));

            unsigned first_node_index = (1 << depth) - 1;

            for (int i = 0; i < (1 << depth); i++) {

                // zacetki vrstic v drevesu imajo indekse:  (1 <<
                // depth) - 1;  to je (2^depth - 1)
                //
                // konci vrstic v drevesu imajo indekse:
                // ((1 << depth) - 1) << 1;  to je ((2^depth)-1) * 2

                std::pair<int, int> current_interval = intervals[i];

                unsigned element_index = (current_interval.first +
                                          current_interval.second) /
                                         2;
                tree[first_node_index + i] =
                    *(elements_ + element_index);

                node_exist[first_node_index + i] = 1;

                new_intervals.push_back(std::pair<int, int>(
                    {current_interval.first, element_index - 1}));
                new_intervals.push_back(std::pair<int, int>(
                    {element_index + 1, current_interval.second}));
            }
            intervals = std::move(new_intervals);
        }
    };

  public:
    unsigned number_of_nodes() { return num_nodes; }
    binary_tree(binary_tree &&) = default;
    binary_tree &operator=(binary_tree &&) = default;
    binary_tree(const binary_tree &) = default;
    binary_tree &operator=(const binary_tree &) = default;

    /**
     *@brief Constructor accepts a vector of arbitrary types and
     *constructs a binary tree based on provided comparator
     * @params starting_elements a vector of values or objects
     * @params comparator a std::function object which know how to
     *compare two values/objects It should return true if the first
     *T1<T2 (first provided object is smaller than the second).
     */
    binary_tree(T *elements_, const unsigned num_nodes_,
                std::function<bool(T &, T &)> comparator_)
        : num_nodes(num_nodes_), comparator(comparator_) {
        construct_tree(elements_);
    };

    /**
     *@brief Constructor accepts a vector of arbitrary types and
     *constructs a binary tree based on provided comparator
     * @params starting_elements a vector of values or objects
     * @params comparator a std::function object which know how to
     *compare two values/objects It should return true if the first
     *T1<T2 (first provided object is smaller than the second).
     */
    binary_tree(T *elements_, const unsigned num_nodes_)
        : num_nodes(num_nodes_) {
        construct_tree(elements_);
    };

    void print_to_file(const std::string filename) {

        std::ofstream outfile(filename);
        unsigned max_depth = std::log2f((double)num_nodes);

        for (int i = 0; i <= ((1 << (max_depth - 1)) - 1) << 1;
             i++) {

            if (node_exist[i] == 1) {
                outfile << tree[i];

                if (node_exist[2 * i + 1] == 1)
                    outfile << " " << tree[2 * i + 1];
                else
                    outfile << " n";

                if (node_exist[2 * i + 2] == 1)
                    outfile << " " << tree[2 * i + 2];
                else
                    outfile << " n";
                outfile << std::endl;
            };
        }
    }
};
