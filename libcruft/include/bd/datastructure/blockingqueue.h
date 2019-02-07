
#ifndef bd_blockingqueue_h
#define bd_blockingqueue_h

#include <queue>
#include <mutex>
#include <condition_variable>

namespace bd
{

template<class T>
class BlockingQueue
{
public:

  BlockingQueue()
  {
  }


  ~BlockingQueue()
  {
  }


  /// Pushes item and returns immediately
  void
  push(T &item)
  {
    std::unique_lock<std::mutex> lock(m_lock);
    m_queue.push(item);
    lock.unlock();
    m_wait.notify_one();
  }

  /// Pushes item and returns immediately.
  void
  push(T &&item)
  {
    std::unique_lock<std::mutex> lock(m_lock);
    m_queue.push(std::move(item));
    lock.unlock();
    m_wait.notify_one();
  }

  /// Blocks until there is something to return.
  T
  pop()
  {
    std::unique_lock<std::mutex> lock(m_lock);
    while(m_queue.empty()) {
      m_wait.wait(m_lock);
    }
    auto item = m_queue.front();
    m_queue.pop();
    return item;
  }


private:
  std::queue <T> m_queue;
  std::mutex m_lock;
  std::condition_variable_any m_wait;


};

} // namespace bd
#endif // ! bd_blockingqueue_h
