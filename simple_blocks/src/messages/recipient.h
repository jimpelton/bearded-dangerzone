#ifndef RECIPIENT_H
#define RECIPIENT_H

#include "message.h"

namespace subvol
{

class ROVChangingMessage;
class ShownBlocksMessage;
class MinRangeChangedMessage;
class MaxRangeChangedMessage;

class Recipient {
public:
  Recipient()
  {
  }


  virtual ~Recipient()
  {
  }

  virtual void
    deliver(Message &m)
  {
    m(*this);
  }

  virtual void
    handle_ROVChangingMessage(ROVChangingMessage &) { }
  
  virtual void
  handle_ShownBlocksMessage(ShownBlocksMessage &){ }

  virtual void
  handle_MinRangeChangedMessage(MinRangeChangedMessage &){ }

  virtual void
  handle_MaxRangeChangedMessage(MaxRangeChangedMessage &){ }

};

  class ROVChangingMessage : public Message
  {
  public:
    ROVChangingMessage()
      : Message( MessageType::ROV_CHANGING_MESSAGE )
      , IsChanging{ false }
    {
    }

    virtual ~ROVChangingMessage() { }

    virtual void
    operator()(Recipient &r) override
    {
      r.handle_ROVChangingMessage(*this);
    }

    bool IsChanging;
  };

  class ShownBlocksMessage : public Message
  {
  public:
    ShownBlocksMessage()
      : Message( MessageType::SHOWN_BLOCKS_MESSAGE)
      , ShownBlocks{ 0 }
    {
    }

    virtual ~ShownBlocksMessage() { }

    virtual void
    operator()(Recipient &r) override
    {
      r.handle_ShownBlocksMessage(*this);
    }

    int ShownBlocks;
  };

  class MinRangeChangedMessage: public Message
  {
  public:
    MinRangeChangedMessage()
      : Message( MessageType::MIN_RANGE_CHANGED_MESSAGE)
      , Min{ 0 }
    {
    }

    virtual ~MinRangeChangedMessage() { }

    virtual void
    operator()(Recipient &r) override
    {
      r.handle_MinRangeChangedMessage(*this);
    }

    double Min;
  };

  class MaxRangeChangedMessage: public Message
  {
  public:
    MaxRangeChangedMessage()
      : Message( MessageType::MAX_RANGE_CHANGED_MESSAGE)
      , Max{ 0 }
    {
    }

    virtual ~MaxRangeChangedMessage() { }

    virtual void
    operator()(Recipient &r) override
    {
      r.handle_MaxRangeChangedMessage(*this);
    }

    double Max;
  };
}

#endif // RECIPIENT_H
