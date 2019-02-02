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
        node(T value_, unsigned parent_index_)
            : value(value_), parent_index(parent_index_){};
        void set_left_leaf_index(unsigned left_leaf_) {
            left_leaf = left_leaf_;
        }
        void set_right_leaf_index(unsigned right_leaf_) {
            right_leaf = right_leaf_;
        }
        unsigned get_left_leaf_index() { return left_leaf; }
        unsigned get_right_leaf_index() { return right_leaf; }
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
     *@brief Constructor accepts a vector of arbitrary types and constructs a
     *binary tree based on provided comparator
     * @params starting_elements a vector of values or objects
     * @params comparator a std::function object which know how to compare two
     *values/objects It should return true if the first T1<T2 (first provided
     *object is smaller than the second).
     */
    binary_tree(const aligned_vector<T> &starting_elements,
                std::function<bool(T &, T &)> comparator) {
        std::sort(starting_elements.begin(), starting_elements.end(),
                  comparator);

        nodes.reserve(starting_elements.size());
        unsigned size = starting_elements.size();
        unsigned mid_element = size / 2;
        construction_helper(starting_elements, left_half_index, end_index, 0);
        construction_helper(starting_elements, 0, right_half_index, 0);
    };

    /**
     * @brief The function helps with the construction of binary tree.
     * @params starting_elements elements to be inserted into the tree
     * @params start_index index at which to start reading the starting_elements
     * vektor
     * @params end_index index at which to end reading the starting_elements
     * vektor - index of the last element to be read
     */
    unsigned construction_helper(
        const std::aligned_vector<T> &starting_elements, unsigned start_index,
        unsigned end_index, unsigned parent_index) {
        // end of recursion case
        if (start_index == end_index) {
            nodes.emplace_back(starting_elements[start_index], parent_index);
            return start_index;
        }

        unsigned mid = (start_index + end_index) / 2;
        unsigned id = nodes.size();
        else {
            mid = mid / 2;
            nodes.emplace_back(starting_elements[mid], parent_index);

            unsigned id_smaller_child = construction_helper(
                starting_elements, start_index, end_index + mid - 1);
            set_left_leaf_index(id_smaller_child);

            unsigned id_larger_child = construction_helper(
                starting_elements, start_index + mid + 1, end_index);
            set_right_leaf_index(id_larger_child);
        }

        return id;
    };
}
