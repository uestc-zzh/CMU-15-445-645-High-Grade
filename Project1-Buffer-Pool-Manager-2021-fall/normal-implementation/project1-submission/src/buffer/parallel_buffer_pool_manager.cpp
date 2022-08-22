//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// parallel_buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/parallel_buffer_pool_manager.h"

namespace bustub {

ParallelBufferPoolManager::ParallelBufferPoolManager(size_t num_instances, size_t pool_size, DiskManager *disk_manager,
                                                     LogManager *log_manager)
    : buffer_pool_managers_(num_instances), buffer_pool_manager_index_(0) {
  // Allocate and create individual BufferPoolManagerInstances
  for (size_t i = 0; i < num_instances; ++i) {
    this->buffer_pool_managers_[i] =
        new BufferPoolManagerInstance(pool_size, num_instances, i, disk_manager, log_manager);
  }
}

// Update constructor to destruct all BufferPoolManagerInstances and deallocate any associated memory
ParallelBufferPoolManager::~ParallelBufferPoolManager() {
  for (BufferPoolManager *b : this->buffer_pool_managers_) {
    delete b;
  }
}

auto ParallelBufferPoolManager::GetPoolSize() -> size_t {
  // Get size of all BufferPoolManagerInstances
  size_t total_size = 0;
  for (BufferPoolManager *b : this->buffer_pool_managers_) {
    total_size += b->GetPoolSize();
  }

  return total_size;
}

auto ParallelBufferPoolManager::GetBufferPoolManager(page_id_t page_id) -> BufferPoolManager * {
  // Get BufferPoolManager responsible for handling given page id. You can use this method in your other methods.
  return this->buffer_pool_managers_[page_id % this->buffer_pool_managers_.size()];
}

auto ParallelBufferPoolManager::FetchPgImp(page_id_t page_id) -> Page * {
  // Fetch page for page_id from responsible BufferPoolManagerInstance
  BufferPoolManager *b = this->GetBufferPoolManager(page_id);
  return b->FetchPage(page_id);
}

auto ParallelBufferPoolManager::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  // Unpin page_id from responsible BufferPoolManagerInstance
  BufferPoolManager *b = this->GetBufferPoolManager(page_id);
  return b->UnpinPage(page_id, is_dirty);
}

auto ParallelBufferPoolManager::FlushPgImp(page_id_t page_id) -> bool {
  // Flush page_id from responsible BufferPoolManagerInstance
  BufferPoolManager *b = this->GetBufferPoolManager(page_id);
  return b->FlushPage(page_id);
}

auto ParallelBufferPoolManager::NewPgImp(page_id_t *page_id) -> Page * {
  // create new page. We will request page allocation in a round robin manner from the underlying
  // BufferPoolManagerInstances
  // 1.   From a starting index of the BPMIs, call NewPageImpl until either 1) success and return 2) looped around to
  // starting index and return nullptr
  // 2.   Bump the starting index (mod number of instances) to start search at a different BPMI each time this function
  // is called
  std::lock_guard<std::mutex> lg(this->latch_);
  for (size_t i = 0; i < this->buffer_pool_managers_.size(); ++i) {
    BufferPoolManager *b = this->buffer_pool_managers_[this->buffer_pool_manager_index_];
    this->buffer_pool_manager_index_ = (this->buffer_pool_manager_index_ + 1) % this->buffer_pool_managers_.size();
    Page *page_ptr = b->NewPage(page_id);
    if (page_ptr != nullptr) {
      return page_ptr;
    }
  }

  return nullptr;
}

auto ParallelBufferPoolManager::DeletePgImp(page_id_t page_id) -> bool {
  // Delete page_id from responsible BufferPoolManagerInstance
  BufferPoolManager *b = this->GetBufferPoolManager(page_id);
  return b->DeletePage(page_id);
}

void ParallelBufferPoolManager::FlushAllPgsImp() {
  // flush all pages from all BufferPoolManagerInstances
  for (BufferPoolManager *b : buffer_pool_managers_) {
    b->FlushAllPages();
  }
}

}  // namespace bustub
