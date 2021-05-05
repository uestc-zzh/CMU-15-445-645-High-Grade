//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.

  // 1.1    If P exists, pin it and return it immediately.
  std::lock_guard lg(this->latch_);
  auto itr = this->page_table_.find(page_id);
  if (itr != this->page_table_.end()) {
    frame_id_t frame_id = itr->second;
    this->pages_[frame_id].pin_count_ += 1;
    this->replacer_->Pin(frame_id);
    return &this->pages_[frame_id];
  }

  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  frame_id_t frame_id = this->GetFrameFromFreeList();
  if (frame_id != INVALID_FRAME_ID) {
    // 3.     Delete R from the page table and insert P.
    this->page_table_.erase(this->pages_[frame_id].page_id_);
    this->page_table_[page_id] = frame_id;

    // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
    this->pages_[frame_id].page_id_ = page_id;
    this->pages_[frame_id].pin_count_ = 1;
    this->disk_manager_->ReadPage(page_id, this->pages_[frame_id].data_);

    return &this->pages_[frame_id];
  }

  this->replacer_->Victim(&frame_id);

  if (frame_id == INVALID_FRAME_ID) {
    return nullptr;
  }

  // 2.     If R is dirty, write it back to the disk.
  if (this->pages_[frame_id].is_dirty_) {
    this->disk_manager_->WritePage(this->pages_[frame_id].page_id_, this->pages_[frame_id].data_);
    this->pages_[frame_id].is_dirty_ = false;
  }

  // 3.     Delete R from the page table and insert P.
  this->page_table_.erase(this->pages_[frame_id].page_id_);
  this->page_table_[page_id] = frame_id;

  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  this->pages_[frame_id].page_id_ = page_id;
  this->pages_[frame_id].pin_count_ = 1;
  this->disk_manager_->ReadPage(page_id, this->pages_[frame_id].data_);

  return &this->pages_[frame_id];
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  std::lock_guard lg(this->latch_);
  frame_id_t frame_id = this->page_table_[page_id];


  if (is_dirty) {
    this->pages_[frame_id].is_dirty_ = true;
  }

  int pin_count = this->pages_[frame_id].pin_count_;
  if (pin_count > 0) {
    this->pages_[frame_id].pin_count_ -= 1;
    if (this->pages_[frame_id].pin_count_ == 0) {
      this->replacer_->Unpin(frame_id);
    }
  }

  return pin_count > 0;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  std::lock_guard lg(this->latch_);
  auto itr = this->page_table_.find(page_id);
  if (itr != this->page_table_.end()) {
    frame_id_t frame_id = itr->second;
    this->disk_manager_->WritePage(page_id, this->pages_[frame_id].data_);
    this->pages_[frame_id].is_dirty_ = false;
    return true;
  }
  return false;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.

  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  std::lock_guard lg(this->latch_);
  frame_id_t frame_id = this->GetFrameFromFreeList();
  if (frame_id != INVALID_FRAME_ID) {
    // 3.   Update P's metadata, zero out memory and add P to the page table.
    page_id_t new_page_id = this->disk_manager_->AllocatePage();
    int old_page_id = this->pages_[frame_id].page_id_;
    this->pages_[frame_id].page_id_ = new_page_id;
    this->pages_[frame_id].pin_count_ = 1;
    this->pages_[frame_id].ResetMemory();

    this->page_table_.erase(old_page_id);
    this->page_table_[new_page_id] = frame_id;

    // 4.   Set the page ID output parameter. Return a pointer to P.
    *page_id = new_page_id;
    return &this->pages_[frame_id];
  }

  this->replacer_->Victim(&frame_id);

  if (frame_id == INVALID_FRAME_ID) {
    return nullptr;
  }

  // Write the page to disk if it is dirty.
  if (this->pages_[frame_id].is_dirty_) {
    this->disk_manager_->WritePage(this->pages_[frame_id].page_id_, this->pages_[frame_id].data_);
    this->pages_[frame_id].is_dirty_ = false;
  }
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  page_id_t new_page_id = this->disk_manager_->AllocatePage();
  int old_page_id = this->pages_[frame_id].page_id_;
  this->pages_[frame_id].page_id_ = new_page_id;
  this->pages_[frame_id].pin_count_ = 1;
  this->pages_[frame_id].ResetMemory();

  this->page_table_.erase(old_page_id);
  this->page_table_[new_page_id] = frame_id;

  // 4.   Set the page ID output parameter. Return a pointer to P.
  *page_id = new_page_id;
  return &this->pages_[frame_id];
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.

  std::lock_guard lg(this->latch_);
  // 1.   Search the page table for the requested page (P). If P does not exist, return true.
  auto itr = this->page_table_.find(page_id);
  if (itr == this->page_table_.end()) {
    return true;
  }

  frame_id_t frame_id = itr->second;

  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  int pin_count = this->pages_[frame_id].pin_count_;
  if (pin_count == 0) {
    // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free
    // list.
    this->page_table_.erase(page_id);

    this->pages_[frame_id].page_id_ = INVALID_PAGE_ID;
    this->pages_[frame_id].is_dirty_ = false;

    // Use Pin to remove this frame from LRU replacer.
    // This is because a frame cannot exist in the free list and the LRU replacer at the same time
    this->replacer_->Pin(frame_id);

    this->free_list_.push_back(frame_id);

    this->disk_manager_->DeallocatePage(page_id);
  }
  return pin_count == 0;
}

void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!
  std::lock_guard lg(this->latch_);
  for (auto [page_id, frame_id] : this->page_table_) {
    this->disk_manager_->WritePage(page_id, this->pages_[frame_id].data_);
    this->pages_[frame_id].is_dirty_ = false;
  }
}

frame_id_t BufferPoolManager::GetFrameFromFreeList() {
  frame_id_t frame_id = INVALID_FRAME_ID;
  if (!this->free_list_.empty()) {
    frame_id = this->free_list_.front();
    this->free_list_.pop_front();
  }
  return frame_id;
}

}  // namespace bustub
