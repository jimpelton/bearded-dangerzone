#ifndef bd_buffer_h__
#define bd_buffer_h__

#include <cstddef>
namespace bd
{

/// \brief A buffer of elements.
/// The buffer holds a raw pointer to it's memory, the number of elements contained
/// in the buffer, and the offset into the original data that was copied into this buffer.
template<class ElementType>
class Buffer
{
public:
  Buffer(ElementType *data, size_t nMaxElements, size_t elementIndex = 0)
      : m_data{ data }
        , m_elementIndexOffset{ elementIndex }
        , m_elementLength{ 0 }
        , m_maxElementLength{ nMaxElements }
  { }

  bool operator==(Buffer<ElementType> const  &rhs)
  {
    //Compare pointers to the start of the memory for this buffer.
    return this->m_data == rhs.m_data;
  }

  /// \brief Get a raw pointer to the start of this buffer's memory.
  ElementType * getPtr() { return m_data; }

  /// \brief Get a pointer-to-const to the start of this buffer's memory.
  ElementType const * getPtr() const { return m_data; }

  /// \brief Get the size in elements of this buffer.
  /// \note The byte size of this buffer's memory can be larger than
  ///       the number of bytes consumed by the elements in the buffer.
  void setNumElements(size_t l) { m_elementLength = l; }

  /// \brief Get the number of elements in this buffer.
  /// \note The byte size of this buffer's memory may be larger than
  ///       the number of bytes consumed by the elements in the buffer.
  size_t getNumElements() const { return m_elementLength; }

  size_t getMaxNumElements() const { return m_maxElementLength; }

  /// \brief Get the element offset into the data of the first element in this buffer.
  /// If this buffer is being filled from some big block of data (for example, a large
  /// file on disk is being read into memory) then the buffer's index offset would be
  /// the offset from the start of the file into that file. The offset should be in units
  /// of elements.
  size_t getIndexOffset() const { return m_elementIndexOffset; }

  void setIndexOffset(size_t ele) { m_elementIndexOffset = ele; }

private:
  ElementType *m_data;                   ///< Ptr to memory of this buffer.
  size_t m_elementIndexOffset;  ///< Starting index in data stream of this buffer.
  size_t m_elementLength;       ///< Length of buffer in elements.
  size_t const m_maxElementLength;

}; // Buffer
}
#endif // ! preproc_buffer_h__
