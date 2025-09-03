#include <atomic>
#include <utility>  // std::swap
#include <cstddef>  // std::size_t

namespace base {

template <typename T>
class SimpleSharedPtr {
public:
  explicit SimpleSharedPtr(T* p = nullptr) : ptr_(p) {
    if (ptr_) cnt_ = new std::atomic<long>(1);
  }

  SimpleSharedPtr(const shared_ptr& other) noexcept
      : ptr_(other.ptr_), cnt_(other.cnt_) {
    if (cnt_) cnt_->fetch_add(1, std::memory_order_relaxed);
  }

  SimpleSharedPtr(shared_ptr&& other) noexcept
      : ptr_(other.ptr_), cnt_(other.cnt_) {
    other.ptr_ = nullptr;
    other.cnt_ = nullptr;
  }

  shared_ptr& operator=(const shared_ptr& other) noexcept {
    if (this != &other) {
      release();
      ptr_ = other.ptr_;
      cnt_ = other.cnt_;
      if (cnt_) cnt_->fetch_add(1, std::memory_order_relaxed);
    }
    return *this;
  }

  shared_ptr& operator=(shared_ptr&& other) noexcept {
    if (this != &other) {
      release();
      ptr_ = other.ptr_;
      cnt_ = other.cnt_;
      other.ptr_ = nullptr;
      other.cnt_ = nullptr;
    }
    return *this;
  }

  ~SimpleSharedPtr() { release(); }

  // 基本接口
  T* get() const noexcept { return ptr_; }
  T& operator*() const noexcept { return *ptr_; }
  T* operator->() const noexcept { return ptr_; }
  explicit operator bool() const noexcept { return ptr_ != nullptr; }

  long use_count() const noexcept {
    return cnt_ ? cnt_->load(std::memory_order_acquire) : 0;
  }

  void reset(T* p = nullptr) {
    if (ptr_ == p) return;
    release();
    if (p) {
      ptr_ = p;
      cnt_ = new std::atomic<long>(1);
    }
  }

  void swap(shared_ptr& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(cnt_, other.cnt_);
  }

private:
  void release() noexcept {
    if (!cnt_) return;
    if (cnt_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete ptr_;
      delete cnt_;
    }
    ptr_ = nullptr;
    cnt_ = nullptr;
  }

  T* ptr_ = nullptr;
  std::atomic<long>* cnt_ = nullptr;
};

} // namespace base
