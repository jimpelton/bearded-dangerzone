//
// Created by jim on 4/10/16.
//

#ifndef bufferpool_h__
#define bufferpool_h__

#include <bd/log/logger.h>
#include <bd/io/buffer.h>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace bd
{

/// \brief Manager a pool of buffers to hand out to consumers and producers.
template<class Ty>
class BufferPool
{
public:

  BufferPool(size_t bufSize, int numBuffers);


  ~BufferPool();


  /// \brief Allocate the memory for this BufferPool.
  void
  allocate();


  /// \brief Wait for a full buffer to be placed in the queue and return it.
  /// If a stop was requested this method will keep returning full buffers
  /// until there are no more.
  Buffer<Ty> *
  nextFullUntilNone();


  /// \brief Return an empty buffer to the queue to be filled again.
  void
  returnEmpty(Buffer<Ty> *);


  /// \brief Get an empty buffer from the queue.
  Buffer<Ty> *
  nextEmpty();


  /// \brief Return a full buffer to the queue.
  void
  returnFull(Buffer<Ty> *);


  /// \brief Return the maximum number of elements that the buffers may contain.
  /// \note This may not be the same as the number of elements the buffer was filled with.
  size_t
  bufferSizeElements() const;


//  bool
//  hasNext();


  void
  kickThreads();

  void
  requestStop();

  /// \brief Return all buffers to empty pool.
  /// \note Function not thread safe.
  void
  reset();


private:

  Ty *m_mem;
  std::vector<Buffer<Ty> *> m_allBuffers;
  std::queue<Buffer<Ty> *> m_emptyBuffers;
  std::queue<Buffer<Ty> *> m_fullBuffers;

  int m_nBufs;
  size_t m_szBytesTotal;

  std::atomic_bool m_stopRequested;

  std::mutex m_emptyBuffersLock;
  std::mutex m_fullBuffersLock;
  std::condition_variable_any m_emptyBuffersAvailable;
  std::condition_variable_any m_fullBuffersAvailable;


};


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
BufferPool<Ty>::BufferPool(size_t bufSize, int nbuf)
    : m_mem{ nullptr }
    , m_nBufs{ nbuf }
    , m_szBytesTotal{ bufSize }
    , m_stopRequested{ false }
{
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
BufferPool<Ty>::~BufferPool()
{
  for (auto buf : m_allBuffers) {
    delete buf;
  }

  if (m_mem) {
    delete[] m_mem;
  }
}


template<class Ty>
void
BufferPool<Ty>::allocate()
{
  size_t buffer_size_elems{ bufferSizeElements() };

  m_mem = new Ty[buffer_size_elems * m_nBufs];
  Info() << "Allocated " << buffer_size_elems * m_nBufs << " elements ( " <<
         m_szBytesTotal << " bytes).";

  for (int i = 0; i < m_nBufs; ++i) {
    size_t offset{ i * buffer_size_elems };
    Ty *start{ m_mem + offset };
    Buffer<Ty> *buf{ new Buffer<Ty>{ start, buffer_size_elems }};
    m_allBuffers.push_back(buf);
    m_emptyBuffers.push(buf);
  }

  Info() << "Generated " << m_allBuffers.size() << " buffers of size " <<
         buffer_size_elems;

}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
Buffer<Ty> *
BufferPool<Ty>::nextFullUntilNone()
{
  std::lock_guard<std::mutex> lck(m_fullBuffersLock);
  while (m_fullBuffers.size() == 0 && !m_stopRequested) {
    m_fullBuffersAvailable.wait(m_fullBuffersLock);
  }

  // keep going until no full buffers are left, and then
  // quit.
  if (m_fullBuffers.size() == 0 && m_stopRequested) {
    return nullptr;
  }

  Buffer<Ty> *buf = m_fullBuffers.front();
  m_fullBuffers.pop();

  return buf;
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferPool<Ty>::returnEmpty(Buffer<Ty> *buf)
{
  std::lock_guard<std::mutex> lck(m_emptyBuffersLock);
  m_emptyBuffers.push(buf);
  m_emptyBuffersAvailable.notify_all();
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
Buffer<Ty> *
BufferPool<Ty>::nextEmpty()
{
  std::lock_guard<std::mutex> lck(m_emptyBuffersLock);
  while (m_emptyBuffers.size() == 0 && !m_stopRequested) {
    // all the buffers are in the full queue.
    m_emptyBuffersAvailable.wait(m_emptyBuffersLock);
  }
  if (m_stopRequested) {
    return nullptr;
  }

  Buffer<Ty> *buf{ m_emptyBuffers.front() };
  m_emptyBuffers.pop();

  return buf;
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferPool<Ty>::returnFull(Buffer<Ty> *buf)
{
  std::lock_guard<std::mutex> lck(m_fullBuffersLock);
  m_fullBuffers.push(buf);
  m_fullBuffersAvailable.notify_all();
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
size_t
BufferPool<Ty>::bufferSizeElements() const
{
  return ( m_szBytesTotal / m_nBufs ) / sizeof(Ty);
}


///////////////////////////////////////////////////////////////////////////////
//template<class Ty>
//bool
//BufferPool<Ty>::hasNext()
//{
//  m_emptyBuffersLock.lock();
//  std::lock_guard<std::mutex> lck(m_fullBuffersLock);
//  return m_emptyBuffers.size() != m_allBuffers.size();
//  return m_fullBuffers.size() > 0;
  // at least one buffer was in the full queue
//  m_emptyBuffersLock.unlock();
//}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferPool<Ty>::kickThreads()
{
  m_emptyBuffersAvailable.notify_all();
  m_fullBuffersAvailable.notify_all();
}

template <class Ty>
void BufferPool<Ty>::requestStop()
{
  m_stopRequested = true;
  kickThreads();
}

///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferPool<Ty>::reset()
{
  // put the buffers into the full buffers queue
  for (Buffer<Ty> *b : m_allBuffers) {
    b->setNumElements(bufferSizeElements());
    b->setIndexOffset(0);
    m_emptyBuffers.push(b);
  }
  while (!m_fullBuffers.empty()) {
    m_fullBuffers.pop();
  }

  m_stopRequested = false;
}

} // namespace preproc

#endif // ! bufferpool_h__ 
