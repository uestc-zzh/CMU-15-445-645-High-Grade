//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {
/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  auto Victim(frame_id_t *frame_id) -> bool override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  auto Size() -> size_t override;

 private:
  // TODO(student): implement me!

  struct Node {
    frame_id_t val_;
    Node *prev_;
    Node *next_;

    inline void AdjustLink() {
      this->next_->prev_ = this;
      this->prev_->next_ = this;
    }

    inline void Detach() {
      this->next_->prev_ = this->prev_;
      this->prev_->next_ = this->next_;
    }
  };

  // Circular Doubly Linked List
  class List {
   public:
    inline explicit List(size_t n) : node_buffer_(n) {
      this->head_ = &this->head_node_;
      this->head_->next_ = this->head_;
      this->head_->prev_ = this->head_;
    }

    inline void Erase(Node *p) {
      this->node_buffer_.Deallocate(p);
      p->Detach();
    }

    inline void PushBack(frame_id_t frame_id) {
      Node *p = this->node_buffer_.Allocate();
      Node *rear = this->Rbegin();
      p->val_ = frame_id;
      p->prev_ = rear;
      p->next_ = rear->next_;
      p->AdjustLink();
    }

    inline auto PopFront() -> frame_id_t {
      Node *head = this->Begin();
      this->Erase(head);
      return head->val_;
    }

    inline auto IsEmpty() -> bool { return this->head_->next_ == this->head_; }

    inline auto Begin() -> Node * { return this->head_->next_; }

    inline auto Rbegin() -> Node * { return this->head_->prev_; }

    inline auto Size() -> size_t { return this->node_buffer_.Size(); }

   private:
    class NodeBuffer {
     public:
      inline explicit NodeBuffer(size_t n) : node_buf_size_(0), node_buf_(n), node_ptrs_(n) {
        for (size_t i = 0; i < n; ++i) {
          node_ptrs_[i] = &node_buf_[i];
        }
      }

      inline auto Size() -> size_t { return this->node_buf_size_; }

      inline auto Allocate() -> Node * { return this->node_ptrs_[this->node_buf_size_++]; }

      inline void Deallocate(Node *p) { this->node_ptrs_[--this->node_buf_size_] = p; }

     private:
      size_t node_buf_size_;
      std::vector<Node> node_buf_;
      std::vector<Node *> node_ptrs_;
    };

    NodeBuffer node_buffer_;
    Node head_node_;
    Node *head_;
  };

  class Hash {
   public:
    inline explicit Hash(size_t n) : v_(n, nullptr) {}

    inline void Erase(frame_id_t frame_id) { this->v_[frame_id] = nullptr; }

    inline auto Contains(frame_id_t frame_id) -> bool { return this->v_[frame_id] != nullptr; }

    inline auto Get(frame_id_t frame_id) -> Node * { return this->v_[frame_id]; }

    inline void Set(frame_id_t frame_id, Node *p) { this->v_[frame_id] = p; }

   private:
    std::vector<Node *> v_;
  };

  List list_;

  Hash hash_;

  std::mutex mutex_;
};

}  // namespace bustub
