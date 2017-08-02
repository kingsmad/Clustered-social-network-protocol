#include "cpu_stream.h"

namespace socio {

CpuStream::CpuStream() : work_cnt_(0) {
  for (size_t i = 0; i < 16; ++i) {
    cpu_device_[i] = std::thread([this]() {
      std::function<void()> work;
      while (work_channel_.Receive(&work) == 0) {
        work();
        std::unique_lock<std::mutex> lock(mutex_);
        if (--work_cnt_ == 0) { stream_empty_cond_.notify_all(); }
        lock.unlock();
      }
    });
  }
}

void CpuStream::SendWork(std::function<void()> work) {
  if (work_channel_.Send(work) == 0) {
    std::unique_lock<std::mutex> lock(mutex_);
    ++work_cnt_;
    lock.unlock();
  }
}

void CpuStream::SyncStream() {
  std::unique_lock<std::mutex> lock(mutex_);
  stream_empty_cond_.wait(lock, [this]() { return work_cnt_ == 0; });
}

CpuStream::~CpuStream() {
  work_channel_.CloseSendEnd();
  for (size_t i = 0; i < 16; ++i) { cpu_device_[i].join(); }
  work_channel_.CloseReceiveEnd();
}

}  // namespace socio
