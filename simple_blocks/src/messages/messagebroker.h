#ifndef subvol_messagebroker_h__
#define subvol_messagebroker_h__

#include "message.h"
#include "recipient.h"

#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <iostream>
#include <bd/log/logger.h>


namespace subvol {

  class Broker {

  public:
    
    static void
    send(Message &m) 
    {
      m_myself->m_messagesMutex.lock();
      m_myself->m_messages.push(m);
      m_myself->m_messagesMutex.unlock();
      m_myself->m_waitMessage.notify_all();
    }

    static void
    subscribeRecipient(Recipient *r)
    {
      m_myself->m_recipients.push_back(r);
    }

    static void
    start()
    {
      if (m_myself == nullptr) {
        m_myself = new Broker();
      }

      m_myself->m_workFuture = std::async(std::launch::async,
        [] { m_myself->work(); });
    }



  private:
    void
    work()
    {
      while(true) {
        Message m{ getNextMessage() };

        for (auto &r : m_recipients) {
          std::cout << "Delivering message\n";
          r->deliver(m);
        }

        if (m.type == MessageType::EMPTY_MESSAGE) {
          break;
        }

      } // while
      bd::Info() << "Message worker exiting.";
    }
    
    Message 
    getNextMessage()
    {
      std::unique_lock<std::mutex>(m_waitMessageMutex);
      while(m_messages.size() == 0) {
        m_waitMessage.wait(m_waitMessageMutex);
      }

      Message m{ m_messages.front() };
      m_messages.pop();
      return m;
    }


    std::vector<Recipient*> m_recipients;

    std::mutex m_messagesMutex;
    std::queue<Message> m_messages;

    std::mutex m_waitMessageMutex;
    std::condition_variable_any m_waitMessage;

    std::future<void> m_workFuture;

    static Broker *m_myself;
    //    std::map<Recipient, std::list<MessageType>> m_receipients;


  };
}
#endif // ! subvol_messagebroker_h__
