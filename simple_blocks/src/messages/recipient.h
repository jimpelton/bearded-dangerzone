#ifndef RECIPIENT_H
#define RECIPIENT_H

#include "message.h"
#include <iostream>
#include "src/sliceset.h"

namespace subvol
{

class ROVChangingMessage;

class ShownBlocksMessage;

class MinRangeChangedMessage;

class MaxRangeChangedMessage;

class BlockCacheStatsMessage;

//class RenderStatsMessage;
class SliceSetChangedMessage;

class BlockLoadedMessage;

class Recipient
{
public:
  Recipient(std::string const &name)
      : m_name{ name }
  {
  }


  virtual ~Recipient()
  {
  }


  virtual void
  deliver(Message *m)
  {
    ( *m )(*this);
  }


  virtual void
  handle_ROVChangingMessage(ROVChangingMessage &)
  {
  }


  virtual void
  handle_ShownBlocksMessage(ShownBlocksMessage &)
  {
  }


  virtual void
  handle_MinRangeChangedMessage(MinRangeChangedMessage &)
  {
  }


  virtual void
  handle_MaxRangeChangedMessage(MaxRangeChangedMessage &)
  {
  }


  virtual void
  handle_BlockCacheStatsMessage(BlockCacheStatsMessage &)
  {
  }


  virtual void
  handle_SliceSetChangedMessage(SliceSetChangedMessage &)
  {
  }


  virtual void
  handle_BlockLoadedMessage(BlockLoadedMessage &)
  {
  }


  std::string const &
  name() const
  {
    return m_name;
  }


private:
  std::string m_name;

};

/////////////////////////////////////////////////////////////////////////////// 
class ROVChangingMessage
    : public Message
{
public:
  ROVChangingMessage()
      : Message(MessageType::ROV_CHANGING_MESSAGE)
      , IsChanging{ false }
  {
  }


  virtual ~ROVChangingMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_ROVChangingMessage(*this);
  }


  bool IsChanging;
};

/////////////////////////////////////////////////////////////////////////////// 
class ShownBlocksMessage
    : public Message
{
public:
  ShownBlocksMessage()
      : Message(MessageType::SHOWN_BLOCKS_MESSAGE)
      , ShownBlocks{ 0 }
  {
  }


  virtual ~ShownBlocksMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_ShownBlocksMessage(*this);
  }


  int ShownBlocks;
};

/////////////////////////////////////////////////////////////////////////////// 
class MinRangeChangedMessage
    : public Message
{
public:
  MinRangeChangedMessage()
      : Message(MessageType::MIN_RANGE_CHANGED_MESSAGE)
      , Min{ 0 }
  {
  }


  virtual ~MinRangeChangedMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_MinRangeChangedMessage(*this);
  }


  double Min;
};

///////////////////////////////////////////////////////////////////////////////
class MaxRangeChangedMessage
    : public Message
{
public:
  MaxRangeChangedMessage()
      : Message(MessageType::MAX_RANGE_CHANGED_MESSAGE)
      , Max{ 0 }
  {
  }


  virtual ~MaxRangeChangedMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_MaxRangeChangedMessage(*this);
  }


  double Max;
};

///////////////////////////////////////////////////////////////////////////////
class BlockCacheStatsMessage
    : public Message
{
public:
  BlockCacheStatsMessage()
      : Message{ MessageType::BLOCK_CACHE_STATS_MESSAGE }
  {
  }


  virtual ~BlockCacheStatsMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_BlockCacheStatsMessage(*this);
  }


  size_t CpuCacheSize;
  size_t GpuCacheSize;
  size_t CpuLoadQueueSize;
  size_t GpuLoadQueueSize;
  size_t CpuBuffersAvailable;
  size_t GpuTexturesAvailable;
};

class SliceSetChangedMessage
    : public Message
{
public:

  SliceSetChangedMessage()
      : Message{ MessageType::SLICESET_CHANGED_MESSAGE }
      , Set{ SliceSet::NoneOfEm }
  {
  }


  virtual ~SliceSetChangedMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_SliceSetChangedMessage(*this);
  }


  SliceSet Set;
};

class BlockLoadedMessage
    : public Message
{
public:

  BlockLoadedMessage()
      : Message{ MessageType::BLOCK_LOADED_MESSAGE }
  {
  }


  virtual ~BlockLoadedMessage()
  {
  }


  void
  operator()(Recipient &r) override
  {
    r.handle_BlockLoadedMessage(*this);
  }


  size_t GpuLoadQueueSize;
};

} // namespace subvol
#endif // RECIPIENT_H
