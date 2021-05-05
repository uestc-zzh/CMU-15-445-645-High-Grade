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

LRUReplacer::LRUReplacer(size_t num_pages) : num_pages_(num_pages) {}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard lg(this->mutex_);
  if (this->hash_.empty()) {
    return false;
  }
  std::swap(*frame_id, this->list_.back());
  this->list_.pop_back();
  this->hash_.erase(*frame_id);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard lg(this->mutex_);
  if (this->hash_.count(frame_id) > 0) {
    this->list_.erase(this->hash_[frame_id]);
    this->hash_.erase(frame_id);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard lg(this->mutex_);
  if (this->hash_.count(frame_id) > 0) {
    return;
  }

  if (this->hash_.size() == this->num_pages_) {
    this->pop();
  }

  this->list_.emplace_front(frame_id);
  this->hash_[frame_id] = this->list_.begin();
}

size_t LRUReplacer::Size() {
  std::lock_guard lg(this->mutex_);
  return this->hash_.size();
}

void LRUReplacer::pop() {
  std::lock_guard lg(this->mutex_);
  if (this->hash_.empty()) {
    return;
  }
  this->hash_.erase(this->list_.back());
  this->list_.pop_back();
}
}  // namespace bustub
