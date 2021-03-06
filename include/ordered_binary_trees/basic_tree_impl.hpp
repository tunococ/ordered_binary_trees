#pragma once

#include <cassert>
#include <iterator>
#include <memory>

#include <ordered_binary_trees/ordered_binary_tree.hpp>
#include <ordered_binary_trees/ordered_binary_tree_iterator.hpp>
#include <ordered_binary_trees/ordered_binary_tree_node.hpp>

namespace ordered_binary_trees {

/**
 *  @brief
 *  Example template class to demonstrate how to *implement* a binary search
 *    tree data structure that is compatible with `ManagedTree`.
 *
 *  This class only contains types and static functions.
 */
template<class ValueT, class AllocatorT = std::allocator<ValueT>>
struct BasicTreeImpl {
  /// This type.
  using This = BasicTreeImpl<ValueT, AllocatorT>;

  /// Type of values to present to the user.
  using Value = ValueT;

  /// Type of allocators for `Value`.
  using ValueAllocator = AllocatorT;

  /**
   *  @brief
   *  Type of `Data` in `Node`.
   *
   *  This typically contains `Value`.
   *  For example, in a red-black tree, `Data` can be `std::pair<Value, bool>`.
   *
   *  In this simplest case (`BasicTreeImpl`), we simply pick `Data = Value`.
   */
  using Data = Value;

  /// Extraction of `AddPointer` inside `AddPointerFromAllocator`.
  template<class T>
  using AddPointer = typename AddPointerFromAllocator<ValueAllocator>::
      template AddPointer<T>;

  /// Type of nodes in a tree.
  using Node = OrderedBinaryTreeNode<Data, AddPointer>;

  /// Type of node allocators.
  using Allocator = typename std::allocator_traits<ValueAllocator>::
      template rebind_alloc<Node>;

  /// Type of trees.
  using Tree = OrderedBinaryTree<Node, Allocator>;

  /// Type of indices.
  using size_type = typename Tree::size_type;

  /**
   *  @brief
   *  Struct or class that contains a public static function to convert `Data&`
   *    to `Value&`.
   *
   *  In this basic implementation, `DefaultExtractValue`, which provides the
   *    identity function, suffices.
   */
  using ExtractValue = DefaultExtractValue<Node>;

  /// Type of node pointers.
  using NodePtr = typename Tree::NodePtr;

  /// `Tree::InsertPosition`.
  using InsertPosition = typename Tree::InsertPosition;

  /**
   *  @brief
   *  Returns the node at a given index.
   */
  static constexpr NodePtr find_node_at_index(Tree& tree, size_type index) {
    return tree.find_node_at_index(index);
  }

  /**
   *  @brief
   *  Constructs a new node and places it as the first node in a tree.
   */
  template<class... Args>
  static constexpr NodePtr emplace_front(Tree& tree, Args&&... args) {
    return tree.emplace(
        tree.first ?
          tree.first->make_insert_position(true) :
          InsertPosition{},
        std::forward<Args>(args)...);
  }

  /**
   *  @brief
   *  Constructs a new node and places it as the last node in a tree.
   */
  template<class... Args>
  static constexpr NodePtr emplace_back(Tree& tree, Args&&... args) {
    return tree.emplace(
        tree.last ?
          tree.last->make_insert_position(false) :
          InsertPosition{},
        std::forward<Args>(args)...);
  }

  /**
   *  @brief
   *  Constructs a new node and places it as an immediate predecessor of
   *    `node`, then returns the new node.
   */
  template<class... Args>
  static constexpr NodePtr emplace_node_before(
      Tree& tree,
      NodePtr node,
      Args&&... args) {
    return tree.emplace(
        node ?
          node->get_prev_insert_position() :
          tree.get_last_insert_position(),
        std::forward<Args>(args)...);
  }

  /**
   *  @brief
   *  Constructs nodes for values in `[input_i, input_end)` and adds them to
   *    the tree right before `node`, then returns the first new node.
   *
   *  If `input_i == input_end`, this function will return `nullptr`.
   *
   *  Each value extracted from the iterator will be fed as the only argument
   *    to the constructor of `Value`.
   *  That means `InputIterator` can point to any type that is convertible to
   *    `Value` via the constructor of `Value`.
   */
  template<class InputIterator>
  static constexpr NodePtr insert_nodes_before(
      Tree& tree,
      NodePtr node,
      InputIterator input_i,
      InputIterator input_end) {
    if (input_i == input_end) {
      return node;
    }
    NodePtr first_new_node{emplace_node_before(tree, node, *input_i)};
    node = first_new_node;
    for (++input_i; input_i != input_end; ++input_i) {
      auto pos{node->make_insert_position(false)};
      node = tree.create_node(*input_i);
      tree.link(pos, node);
    }
    return first_new_node;
  }

  /**
   *  @brief
   *  Takes data from `other` and inserts them at `pos` in `tree`.
   *  The ownership of all the nodes in `other` is transferred to `tree`
   *    afterwards.
   *
   *  This function does not alter `tree.allocator` or `other.allocator`.
   *  If `tree.allocator` cannot deallocate nodes created by `other.allocator`,
   *    the behavior will be undefined.
   */
  static constexpr void join(Tree& tree, InsertPosition pos, Tree& other) {
    tree.link(pos, other.root);
    other.clear();
  }

  /**
   *  @brief
   *  Similar to `join(tree, tree.get_first_insert_position(), other)`.
   */
  static constexpr void join_front(Tree& tree, Tree& other) {
    join(tree, tree.get_first_insert_position(), other);
  }

  /**
   *  @brief
   *  Similar to `join(tree, tree.get_last_insert_position(), other)`.
   */
  static constexpr void join_back(Tree& tree, Tree& other) {
    join(tree, tree.get_last_insert_position(), other);
  }

  /**
   *  @brief
   *  Clears the tree and assigns values from `[input_begin, input_end)` to the
   *    tree.
   */
  template<class InputIterator>
  static constexpr void assign(
      Tree& tree,
      InputIterator input_begin,
      InputIterator input_end) {
    tree.destroy_all_nodes();
    insert_nodes_before(tree, tree.first, input_begin, input_end);
  }

  /**
   *  @brief
   *  Erases the first node.
   */
  static constexpr void erase_front(Tree& tree) {
    assert(!tree.empty());
    tree.template erase<true, true>(tree.first);
  }

  /**
   *  @brief
   *  Erases the last node.
   */
  static constexpr void erase_back(Tree& tree) {
    assert(!tree.empty());
    tree.template erase<true, true>(tree.last);
  }

  /**
   *  @brief
   *  Erases `node` and returns its former immediate successor.
   */
  static constexpr NodePtr erase_node(Tree& tree, NodePtr node) {
    NodePtr next_node{node->find_next_node()};
    tree.template erase<true, true>(node);
    return next_node;
  }

  /**
   *  @brief
   *  Erases nodes in the interval `[begin, end)`.
   * 
   *  If `begin == end`, this function will not erase any nodes and return
   *    `begin`.
   */
  static constexpr NodePtr erase_nodes(
      Tree& tree,
      NodePtr begin,
      NodePtr end) {
    while (begin != end) {
      assert(begin);
      NodePtr next{begin->find_next_node()};
      tree.erase(begin);
      begin = next;
    }
    return begin;
  }

};

} // namespace ordered_binary_trees
