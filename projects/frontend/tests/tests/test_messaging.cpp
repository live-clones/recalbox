#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 25/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <utils/sync/SyncMessageSender.h>
#include <gtest/gtest.h>
#include <utils/os/system/Thread.h>
#include <utils/String.h>


class Message1
{
  public:
    int Data;
    long long DataLL;
    float DataF;
};

class Message2
{
  public:
    bool DataB;
    void* DataP[3];
};

class MessageHolder
  : public ISyncMessageReceiver<Message1>
  , public ISyncMessageReceiver<Message2>
{
  public:
    std::vector<Message1>& List1() { return Messages1; }
    std::vector<Message2>& List2() { return Messages2; }

  private:
    void ReceiveSyncMessage(const Message1& message) final
    {
      Messages1.push_back(message);
    }

    void ReceiveSyncMessage(const Message2& message) final
    {
      Messages2.push_back(message);
    }

    std::vector<Message1> Messages1;
    std::vector<Message2> Messages2;
};

class MessageBombing: public Thread
{
  public:
    MessageBombing(int index, int count, MessageHolder& holder)
      : mHolder(holder)
      , mIndex(index)
      , mCount(count)
    {
      Start(String("Bomber").Append(index).data());
    }

    ~MessageBombing() override
    {
      delete Sender1;
      delete Sender2;
    }

    void Run() final
    {
      Sender1 = new SyncMessageSender<Message1>(mHolder);
      Sender2 = new SyncMessageSender<Message2>(mHolder);
      usleep(10000);
      for(int i = 0; i < mCount; ++i)
      {
        Sender1->Send({ .Data = 123 + i + (mCount * mIndex), .DataLL = 0x123456789ABCDEF - i, .DataF = 3.456f + (float)i });
        Sender2->Send({ .DataB = (i & 1) != 0, .DataP = { &mHolder, nullptr, &mHolder } });
      }
    }

  private:
    SyncMessageSender<Message1>* Sender1 = nullptr;
    SyncMessageSender<Message2>* Sender2 = nullptr;

    MessageHolder& mHolder;
    int mIndex;
    int mCount;
};

class MessagingTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    SyncMessageFactory mFactory;
    MessageHolder mHolder;
};

TEST_F(MessagingTest, TestNoThreadSimpleMessage)
{
  SyncMessageSender<Message1> s(mHolder);

  s.Send({ .Data = 123, .DataLL = 0x123456789ABCDEF, .DataF = 3.456f });
  ASSERT_TRUE(mHolder.List1().empty());

  mFactory.DispatchMessage();

  ASSERT_TRUE(mHolder.List1().size() == 1);
  ASSERT_TRUE(mHolder.List1().back().Data == 123);
  ASSERT_TRUE(mHolder.List1().back().DataLL == 0x123456789ABCDEF);
  ASSERT_TRUE(mHolder.List1().back().DataF == 3.456f);
}

TEST_F(MessagingTest, TestNoThreadOrderedMessages)
{
  SyncMessageSender<Message1> s(mHolder);

  for(int i = 0; i < 10; ++i)
    s.Send({ .Data = 123 + i, .DataLL = 0x123456789ABCDEF - i, .DataF = 3.456f + (float)i });
  ASSERT_TRUE(mHolder.List1().empty());

  mFactory.DispatchMessage();

  ASSERT_TRUE(mHolder.List1().size() == 10);
  for(int i = 0; i < 10; ++i)
  {
    ASSERT_TRUE(mHolder.List1()[i].Data == 123 + i);
    ASSERT_TRUE(mHolder.List1()[i].DataLL == 0x123456789ABCDEF - i);
    ASSERT_TRUE(mHolder.List1()[i].DataF == 3.456f + (float)i);
  }
}

TEST_F(MessagingTest, TestNoThreadOrderedMessagesMultipleDispatch)
{
  SyncMessageSender<Message1> s(mHolder);

  for(int i = 0; i < 5; ++i)
    s.Send({ .Data = 123 + i, .DataLL = 0x123456789ABCDEF - i, .DataF = 3.456f + (float)i });
  ASSERT_TRUE(mHolder.List1().empty());
  mFactory.DispatchMessage();
  ASSERT_TRUE(mHolder.List1().size() == 5);

  for(int i = 5; i < 10; ++i)
    s.Send({ .Data = 123 + i, .DataLL = 0x123456789ABCDEF - i, .DataF = 3.456f + (float)i });
  mFactory.DispatchMessage();

  ASSERT_TRUE(mHolder.List1().size() == 10);
  for(int i = 0; i < 10; ++i)
  {
    ASSERT_TRUE(mHolder.List1()[i].Data == 123 + i);
    ASSERT_TRUE(mHolder.List1()[i].DataLL == 0x123456789ABCDEF - i);
    ASSERT_TRUE(mHolder.List1()[i].DataF == 3.456f + (float)i);
  }
}

TEST_F(MessagingTest, TestThreadedFastSendingAndDispatch)
{
  MessageBombing mb(1, 100000, mHolder);
  while(mb.IsRunning())
  {
    usleep(0);
    mFactory.DispatchMessage();
  }
  // Ensure all messages have been dispatched
  mFactory.DispatchMessage();

  int l1 = (int)mHolder.List1().size();
  int l2 = (int)mHolder.List2().size();
  ASSERT_TRUE(l1 == 100000);
  ASSERT_TRUE(l2 == 100000);
  for(int i = 0; i < 100000; ++i)
  {
    ASSERT_TRUE(mHolder.List1()[i].Data == 123 + i + (1 * 100000));
    ASSERT_TRUE(mHolder.List1()[i].DataLL == 0x123456789ABCDEF - i);
    ASSERT_TRUE(mHolder.List1()[i].DataF == 3.456f + (float)i);
    ASSERT_TRUE(mHolder.List2()[i].DataB == ((i & 1) != 0));
    ASSERT_TRUE(mHolder.List2()[i].DataP[0] == &mHolder);
    ASSERT_TRUE(mHolder.List2()[i].DataP[1] == nullptr);
    ASSERT_TRUE(mHolder.List2()[i].DataP[2] == &mHolder);
  }
}

TEST_F(MessagingTest, TestThreadedMassiveBombing)
{
  Array<MessageBombing*> bombers;
  for(int i = 100; --i >= 0; )
    bombers.Add(new MessageBombing(i, 1000, mHolder));
  for(;;)
  {
    int alive = 0;
    for(int i = bombers.Count(); --i>= 0; ) if (bombers[i]->IsRunning()) ++alive;
    if (alive == 0) break;
    usleep(0);
    mFactory.DispatchMessage();
  }
  // Ensure all messages have been dispatched
  mFactory.DispatchMessage();
  for(int i = bombers.Count(); --i >= 0; )
    delete bombers[i];

  int l1 = (int)mHolder.List1().size();
  int l2 = (int)mHolder.List2().size();
  ASSERT_TRUE(l1 == 100000);
  ASSERT_TRUE(l2 == 100000);
}

#pragma clang diagnostic pop