template <typename T>
class binary_tree() {
  private:
    class node {
      private:
        T value;
        unsigned left_leaf;
        unsigned right_leaf;
        unsigned parent;

      public:
        node() = default;
        node(T value, ){};
        unsigned left_leaf_index() { return left_leaf; }
        unsigned right_leaf_index() { return right_leaf; }
        T get_value() { return value; }
    };

    aligend_vector<node> nodes;

  public:
    unsigned number_of_nodes() { return nodes.size(); }
    binary_tree(binary_tree &&) = default;
    binary_tree &operator=(binary_tree &&) = default;
    binary_tree(const binary_tree &) = default;
    binary_tree &operator=(const binary_tree &) = default;

    /**
     * @brief The function helps with the construction of binary tree.
     * @params starting_elements elements to be inserted into the tree
     * @params start_index index at which to start reading the starting_elements
     * vektor
     * @params end_index index at which to end reading the starting_elements
     * vektor - index of the last element to be read
     */
    unsigned construction_helper(
        const std::aligned_vector<T> &starting_elements, unsinged start_index,
        unsigned end_index, unsigned parent_index) {
        unsigned mid = start_index + end_index;
        unsigned id = nodes.size();
        if (mid % 2 == 0) {
        }
        return id;
    };

    binary_tree(const aligned_vector<T> &starting_elements,
                std::function<void(T &, T &)> comparator) {
        std::sort(starting_elements.begin(), starting_elements.end(),
                  comparator);

        nodes.reserve(starting_elements.size());
        unsigned size = starting_elements.size();
        unsigned mid_element = size / 2;
        construction_helper(starting_elements, left_half_index, end_index, 0);
        construction_helper(starting_elements, 0, right_half_index, 0);
    };
}
