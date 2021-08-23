#ifndef _SOVOL_BOUNDEDBUFFER_HH
#define _SOVOL_BOUNDEDBUFFER_HH 1

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class NoProducerException : public std::exception {
  const char* what() const throw() { return "NoProducerException"; }
};

template <typename T>
class BoundedBuffer {
 private:
  std::vector<T> circular_buffer_;
  std::size_t begin_;
  std::size_t end_;
  std::size_t buffered_;
  std::condition_variable not_full_cv_;
  std::condition_variable not_empty_cv_;
  std::mutex mutex_;
  int producers_num;

 public:
  BoundedBuffer(const BoundedBuffer& rhs) = delete;
  BoundedBuffer& operator=(const BoundedBuffer& rhs) = delete;

  BoundedBuffer(std::size_t size, int _producers_num)
      : circular_buffer_(size),
        begin_(0),
        end_(0),
        buffered_(0),
        producers_num(_producers_num) {}

  void producerQuit() {
    producers_num--;
    not_empty_cv_.notify_one();
  }

  void push(const T& element) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_full_cv_.wait(lock,
                        [this] { return buffered_ < circular_buffer_.size(); });

      circular_buffer_[end_] = element;
      end_ = (end_ + 1) % circular_buffer_.size();

      ++buffered_;
    }

    not_empty_cv_.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_cv_.wait(lock,
                       [this] { return buffered_ > 0 || producers_num <= 0; });

    if (producers_num <= 0 && buffered_ <= 0) {
      not_empty_cv_.notify_one();
      throw NoProducerException();
    }

    T element = circular_buffer_[begin_];
    begin_ = (begin_ + 1) % circular_buffer_.size();

    --buffered_;

    lock.unlock();
    not_full_cv_.notify_one();
    return element;
  }
};

#endif