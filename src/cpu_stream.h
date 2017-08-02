#ifndef SOCIO_CPU_THREAD_H_
#define SOCIO_CPU_THREAD_H_

#include "channel.h"
#include <thread>
#include <condition_variable>

namespace socio {

class CpuStream final {
 public:
  CpuStream();
  ~CpuStream();

  void SendWork(std::function<void()> work);

  void SyncStream();

 private:
  Channel<std::function<void()>> work_channel_;
  std::thread cpu_device_[16];

  int work_cnt_;
  std::condition_variable stream_empty_cond_;
  mutable std::mutex mutex_;
};

}  // namespace socio 

#endif  // SOCIO_CPU_THREAD_H_
