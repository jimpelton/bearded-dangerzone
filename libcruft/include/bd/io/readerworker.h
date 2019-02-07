
#ifndef bd_readerworker_h
#define bd_readerworker_h

#include <bd/io/bufferpool.h>
#include <bd/io/buffer.h>
#include <bd/log/logger.h>

#include <fstream>
#include <atomic>
#include <string>

namespace bd
{


template<class Ty>
class ReaderWorker
{
public:

  ReaderWorker(BufferPool<Ty> &p)
    //: m_reader{ &r }
    : m_pool{ &p }
    , m_is{ nullptr }
  { }

  ~ReaderWorker()
  {
    if (m_is)
      delete m_is;
  }

  /// \brief Pop buffers and fill them from the ifstream.
  /// \returns -1 if file could not be opened, or the total bytes read.
  long long
  operator()(std::atomic_bool const &quit)
  {
    if (! open()) {
        Err() << "Could not open file " << m_path << ". Exiting readerworker loop.";
        return -1;
    }

    // bytes to attempt to read from file.
//    long long const buffer_size_bytes{ static_cast<long long>(m_pool->bufferSizeElements() * sizeof(Ty)) };
    size_t total_read_bytes{ 0 };

    Dbg() << "Starting reader loop.";
    std::cout << std::endl;
    while(!m_is->eof() && !quit) {

      // wait for the next empty buffer in the pool.
      Buffer<Ty> *buf = m_pool->nextEmpty();
      if (buf == nullptr) {
        break;
      }


      // set the element index this buffer starts at.
      buf->setIndexOffset(total_read_bytes/sizeof(Ty));
      Ty *data = buf->getPtr();

      m_is->read(reinterpret_cast<char*>(data), buf->getMaxNumElements());
      std::streamsize amount{ m_is->gcount() };
      buf->setNumElements(amount / sizeof(Ty));
      
      // the last buffer filled may not be a full buffer, so resize!
      if (amount == 0) {
      //Dbg() << "Reader read 0 bytes.";
        m_pool->returnEmpty(buf);
        break;
      }


      total_read_bytes += amount;
      std::cout << "\rRead " << total_read_bytes << " bytes." << std::flush;

      m_pool->returnFull(buf);

    } // while

    std::cout << std::endl;

    m_pool->requestStop();

    m_is->close();
    Dbg() << "Reader done after reading " << total_read_bytes << " bytes";
//    m_pool->kickThreads();
    return static_cast<long long int>(total_read_bytes);
  }

    
  void
  setPath(std::string const &path)
  {
    m_path = path;
  }


private:
  bool
  open()
  {
    m_is = new std::ifstream();
    m_is->open(m_path, std::ios::binary);
    return m_is->is_open();
  }

  BufferPool<Ty> *m_pool;
  std::ifstream *m_is;
  std::string m_path;

}; // ReaderWorker


} //namespace preproc

#endif // ! readerworker_h__
