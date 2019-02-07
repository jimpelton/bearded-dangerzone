#ifndef bd_buffered_reader
#define bd_buffered_reader

#include "bufferpool.h"
#include "buffer.h"
#include "readerworker.h"

#include <fstream>
#include <thread>
#include <mutex>
#include <future>

namespace bd
{


/// \brief Reads binary data into Buffers that can be popped from and
///        pushed back to a buffer pool.
/// Template parameter \c Ty is the data type contained in the raw file, and is thus
/// the element type contained in each Buffer.
template<class Ty>
class BufferedReader
{

public:
  BufferedReader(size_t bufSize);


  ~BufferedReader();


  /// \brief Open the raw file at path.
  /// \return True if opened, false otherwise.
  bool
  open(std::string const &path);


  /// \brief Start readering the file.
  void
  start();


  /// \brief Stop reading as soon as possible.
  void
  stop();


  /// \brief Stop reading and wait for the read thread to exit.
  /// \return The number of bytes read so far by the thread.
  long long int
  reset();


  /// \brief Return true if the read thread is still active.
//  bool
//  isReading() const;


  /// \brief Grab the next buffer data from the pool as soon as it is ready.
  /// \note Blocks until a full buffer is ready.
  Buffer<Ty> *
  waitNextFullUntilNone();


  /// \brief Return a buffer to the empty pool to be filled again.
  void
  waitReturnEmpty(Buffer<Ty> *buf);

  /// /brief Get the number of elements in a single buffer in the buffer pool.
  size_t
  singleBufferElements() const;

  size_t
  totalBufferBytes() const;

private:
  std::string m_path;

  size_t m_bufSizeBytes;
  int m_numBuffers;

  BufferPool<Ty> *m_pool;
  std::future<long long int> m_future;
  std::atomic_bool m_stopReaderThread;

};


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
BufferedReader<Ty>::BufferedReader(size_t bufSize)
    : m_path{ }
    , m_bufSizeBytes{ bufSize }
    , m_numBuffers{ 4 }
    , m_pool{ nullptr }
    , m_future{ }
{
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
BufferedReader<Ty>::~BufferedReader()
{
  if (m_pool) {
    delete m_pool;
  }
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
bool
BufferedReader<Ty>::open(std::string const &path)
{
  m_path = path;
  std::ifstream test(m_path);
  if (! test.is_open()) {
    Err() << "Unable to open file: " + m_path;
    return false;
  }
  test.close();
  m_pool = new BufferPool<Ty>(m_bufSizeBytes, 4);
  m_pool->allocate();
  return true;

}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferedReader<Ty>::start()
{
  m_stopReaderThread = false;
  m_future =
      std::async(std::launch::async,
                 [&]() -> long long int {
                   ReaderWorker<Ty> worker(*m_pool);
                   worker.setPath(m_path);
                   return worker(std::ref(m_stopReaderThread));
                 });
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
void
BufferedReader<Ty>::stop()
{
  m_stopReaderThread = true;
}


///////////////////////////////////////////////////////////////////////////////
template<class Ty>
long long int
BufferedReader<Ty>::reset()
{
  m_stopReaderThread = true;
  m_future.wait();
  m_pool->reset();
  return m_future.get();
}


///////////////////////////////////////////////////////////////////////////////
//template<class Ty>
//bool
//BufferedReader<Ty>::isReading() const
//{
//  return m_future.wait_for(std::chrono::seconds(0)) !=
//      std::future_status::ready;
//}


template<class Ty>
Buffer<Ty> *
BufferedReader<Ty>::waitNextFullUntilNone()
{
  return m_pool->nextFullUntilNone();
}


template<class Ty>
void
BufferedReader<Ty>::waitReturnEmpty(Buffer<Ty> *buf)
{
  return m_pool->returnEmpty(buf);
}


template<class Ty>
size_t
BufferedReader<Ty>::singleBufferElements() const
{
  return m_pool->bufferSizeElements();
}


template<class Ty>
size_t
BufferedReader<Ty>::totalBufferBytes() const 
{
  return m_bufSizeBytes;
}


///////////////////////////////////////////////////////////////////////////////
//template<class Ty>
//bool
//BufferedReader<Ty>::hasNextBuffer() const
//{
//   if no buffers in pool, check if the reader thread is done.
//   If it is still reading, then we should return true because there will
//   be a full buffer available soon.
//  return m_pool->hasNext() || isReading();
//}


} // namespace bd
#endif // ! buffered_reader__
