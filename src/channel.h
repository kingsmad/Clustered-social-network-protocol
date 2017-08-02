#ifndef SOCIO_CHANNEL_H_
#define SOCIO_CHANNEL_H_

#include <condition_variable>
#include <mutex>
#include <queue>

namespace socio {

template<typename T>
class Channel final {
 public:
  Channel() : is_send_closed_(false), is_receive_closed_(false) {}
  ~Channel() = default;

  int Send(const T& item);

  int Receive(T* item);

  void CloseSendEnd();

  void OpenSendEnd();

  void CloseReceiveEnd();

 private:
  std::queue<T> val_;
  mutable std::mutex mutex_;
  bool is_send_closed_;
  bool is_receive_closed_;
  std::condition_variable cond_;
};

template<typename T>
int Channel<T>::Send(const T& item) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (is_send_closed_) { return -1; }
  val_.push(item);
  cond_.notify_one();
  return 0;
}

template<typename T>
int Channel<T>::Receive(T* item) {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() {
    return !val_.empty() || is_receive_closed_ || is_send_closed_;
  });
  if (val_.empty() || is_receive_closed_) { return -1; }
  *item = val_.front();
  val_.pop();
  return 0;
}

template<typename T>
void Channel<T>::CloseSendEnd() {
  std::unique_lock<std::mutex> lock(mutex_);
  is_send_closed_ = true;
  cond_.notify_all();
}

template<typename T>
void Channel<T>::CloseReceiveEnd() {
  std::unique_lock<std::mutex> lock(mutex_);
  is_receive_closed_ = true;
  cond_.notify_all();
}

}  // namespace socio 

#endif  // SOCIO_CHANNEL_H_
