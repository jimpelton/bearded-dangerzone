#ifndef RECIPIENT_H
#define RECIPIENT_H

#include "message.h"
#include <iostream>


namespace subvol
{

class ROVChangingMessage;
class ShownBlocksMessage;
class MinRangeChangedMessage;
class MaxRangeChangedMessage;
class BlockCacheStatsMessage;
class RenderStatsMessage;

class Recipient {
public:
  Recipient()
  {
  }


  virtual ~Recipient()
  {
  }

  virtual void
    deliver(Message *m)
  {
    (*m)(*this);
  }

  virtual void
  handle_ROVChangingMessage(ROVChangingMessage &) { }
  
  virtual void
  handle_ShownBlocksMessage(ShownBlocksMessage &) { }

  virtual void
  handle_MinRangeChangedMessage(MinRangeChangedMessage &) { }

  virtual void
  handle_MaxRangeChangedMessage(MaxRangeChangedMessage &) { }

  virtual void
  handle_BlockCacheStatsMessage(BlockCacheStatsMessage &){ }

  virtual void
  handle_RenderStatsMessage(RenderStatsMessage &){ }

};

 /////////////////////////////////////////////////////////////////////////////// 
  class ROVChangingMessage : public Message
  {
  public:
    ROVChangingMessage()
      : Message( MessageType::ROV_CHANGING_MESSAGE )
      , IsChanging{ false }
    {
    }

    virtual ~ROVChangingMessage() { }

    void
    operator()(Recipient &r) override
    {
      r.handle_ROVChangingMessage(*this);
    }

    bool IsChanging;
  };

 /////////////////////////////////////////////////////////////////////////////// 
  class ShownBlocksMessage : public Message
  {
  public:
    ShownBlocksMessage()
      : Message( MessageType::SHOWN_BLOCKS_MESSAGE)
      , ShownBlocks{ 0 }
    {
    }

    virtual ~ShownBlocksMessage() { }

    void
    operator()(Recipient &r) override
    {
      r.handle_ShownBlocksMessage(*this);
    }

    int ShownBlocks;
  };

 /////////////////////////////////////////////////////////////////////////////// 
  class MinRangeChangedMessage: public Message
  {
  public:
    MinRangeChangedMessage()
      : Message( MessageType::MIN_RANGE_CHANGED_MESSAGE)
      , Min{ 0 }
    {
    }

    virtual ~MinRangeChangedMessage() { }

    void
    operator()(Recipient &r) override
    {
      r.handle_MinRangeChangedMessage(*this);
    }

    double Min;
  };

 /////////////////////////////////////////////////////////////////////////////// 
  class MaxRangeChangedMessage: public Message
  {
  public:
    MaxRangeChangedMessage()
      : Message( MessageType::MAX_RANGE_CHANGED_MESSAGE)
      , Max{ 0 }
    {
    }

    virtual ~MaxRangeChangedMessage() { }

    void
    operator()(Recipient &r) override
    {
      r.handle_MaxRangeChangedMessage(*this);
    }

    double Max;
  };


  ///////////////////////////////////////////////////////////////////////////////
  class BlockCacheStatsMessage : public Message
  {
  public:
    BlockCacheStatsMessage()
      : Message{ MessageType::BLOCK_CACHE_STATS_MESSAGE }
    {
    }
    virtual ~BlockCacheStatsMessage() { }

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

    size_t MaxCpuCacheSize;
    size_t MaxGpuCacheSize;

  };


  ///////////////////////////////////////////////////////////////////////////////
  class RenderStatsMessage : public Message
  {
  public:
    RenderStatsMessage()
      : Message{ MessageType::RENDER_STATS_MESSAGE }
    {
    }
    virtual ~RenderStatsMessage() { }

    void
    operator()(Recipient &r) override
    {
      r.handle_RenderStatsMessage(*this);
    }
    

  };

} // namespace subvol
#endif // RECIPIENT_H
