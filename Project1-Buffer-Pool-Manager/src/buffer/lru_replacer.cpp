//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"
#include <utility>

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
  this->node_buf_top_ = 0;
  this->node_buf_ = std::vector<Node>(num_pages);
  this->node_ptrs_ = std::vector<Node *>(num_pages);

  for (size_t i = 0; i < node_ptrs_.size(); ++i) {
    node_ptrs_[i] = &node_buf_[i];
  }

  this->head = nullptr;
  this->rear = nullptr;

  this->hash_ = std::vector<Node *>(num_pages);
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard lg(this->mutex_);

  if (this->head == nullptr) {
    return false;
  }

  *frame_id = this->rear->val;
  this->list_pop_back();
  this->hash_[*frame_id] = nullptr;
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard lg(this->mutex_);

  if (this->hash_[frame_id] != nullptr) {
    this->list_erase(this->hash_[frame_id]);
    this->hash_[frame_id] = nullptr;
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard lg(this->mutex_);

  if (this->hash_[frame_id] != nullptr) {
    return;
  }

  this->list_push_front(frame_id);
  this->hash_[frame_id] = this->head;
}

size_t LRUReplacer::Size() {
  std::lock_guard lg(this->mutex_);
  return this->node_buf_top_;
}

void LRUReplacer::list_pop_back() {
  this->node_ptrs_[--this->node_buf_top_] = this->rear;
  if (this->rear == this->head) {
    this->rear = nullptr;
    this->head = nullptr;
  } else {
    this->rear = this->rear->prev;
    this->rear->next = nullptr;
  }
}

void LRUReplacer::list_erase(Node *p) {
  this->node_ptrs_[--this->node_buf_top_] = p;
  if (p == this->head && p == this->rear) {
    this->head = nullptr;
    this->rear = nullptr;
  } else if (p == this->head) {
    this->head = this->head->next;
    this->head->prev = nullptr;
  } else if (p == this->rear) {
    this->rear = this->rear->prev;
    this->rear->next = nullptr;
  } else {
    p->prev->next = p->next;
    p->next->prev = p->prev;
  }
}

void LRUReplacer::list_push_front(frame_id_t frame_id) {
  Node *p = this->node_ptrs_[this->node_buf_top_++];
  p->val = frame_id;
  if (this->head != nullptr) {
    p->prev = nullptr;
    p->next = this->head;
    this->head->prev = p;
    this->head = p;
  } else {
    this->head = p;
    this->rear = p;
    p->prev = nullptr;
    p->next = nullptr;
  }
}

}  // namespace bustub
