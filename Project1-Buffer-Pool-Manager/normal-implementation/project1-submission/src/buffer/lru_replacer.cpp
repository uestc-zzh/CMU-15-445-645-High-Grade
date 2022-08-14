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

LRUReplacer::LRUReplacer(size_t num_pages) : list_(num_pages), hash_(num_pages) {}

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  std::lock_guard<std::mutex> lg(this->mutex_);

  if (this->list_.IsEmpty()) {
    return false;
  }

  *frame_id = this->list_.PopFront();
  this->hash_.Erase(*frame_id);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lg(this->mutex_);
  if (this->hash_.Contains(frame_id)) {
    this->list_.Erase(this->hash_.Get(frame_id));
    this->hash_.Erase(frame_id);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lg(this->mutex_);
  if (!this->hash_.Contains(frame_id)) {
    this->list_.PushBack(frame_id);
    this->hash_.Set(frame_id, this->list_.Rbegin());
  }
}

auto LRUReplacer::Size() -> size_t {
  std::lock_guard<std::mutex> lg(this->mutex_);
  return this->list_.Size();
}

}  // namespace bustub
