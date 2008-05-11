#include "BtHaveMessage.h"
#include "PeerMessageUtil.h"
#include "Peer.h"
#include "MockPieceStorage.h"
#include <cstring>
#include <cppunit/extensions/HelperMacros.h>

namespace aria2 {

class BtHaveMessageTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(BtHaveMessageTest);
  CPPUNIT_TEST(testCreate);
  CPPUNIT_TEST(testGetMessage);
  CPPUNIT_TEST(testDoReceivedAction);
  CPPUNIT_TEST(testToString);
  CPPUNIT_TEST_SUITE_END();
private:

public:
  void setUp() {
  }

  void testCreate();
  void testGetMessage();
  void testDoReceivedAction();
  void testToString();
};


CPPUNIT_TEST_SUITE_REGISTRATION(BtHaveMessageTest);

void BtHaveMessageTest::testCreate() {
  unsigned char msg[9];
  PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 5, 4);
  PeerMessageUtil::setIntParam(&msg[5], 12345);
  SharedHandle<BtHaveMessage> pm = BtHaveMessage::create(&msg[4], 5);
  CPPUNIT_ASSERT_EQUAL((uint8_t)4, pm->getId());
  CPPUNIT_ASSERT_EQUAL((size_t)12345, pm->getIndex());

  // case: payload size is wrong
  try {
    unsigned char msg[10];
    PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 6, 4);
    BtHaveMessage::create(&msg[4], 2);
    CPPUNIT_FAIL("exception must be thrown.");
  } catch(...) {
  }
  // case: id is wrong
  try {
    unsigned char msg[9];
    PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 5, 5);
    BtHaveMessage::create(&msg[4], 1);
    CPPUNIT_FAIL("exception must be thrown.");
  } catch(...) {
  }
}

void BtHaveMessageTest::testGetMessage() {
  BtHaveMessage msg;
  msg.setIndex(12345);
  unsigned char data[9];
  PeerMessageUtil::createPeerMessageString(data, sizeof(data), 5, 4);
  PeerMessageUtil::setIntParam(&data[5], 12345);
  CPPUNIT_ASSERT(memcmp(msg.getMessage(), data, 9) == 0);
}

void BtHaveMessageTest::testDoReceivedAction() {
  SharedHandle<Peer> peer(new Peer("host", 6969));
  peer->allocateSessionResource(16*1024, 256*1024);
  BtHaveMessage msg;
  msg.setIndex(1);
  msg.setPeer(peer);
  SharedHandle<MockPieceStorage> pieceStorage(new MockPieceStorage());
  msg.setPieceStorage(pieceStorage);

  CPPUNIT_ASSERT(!peer->hasPiece(msg.getIndex()));

  msg.doReceivedAction();

  CPPUNIT_ASSERT(peer->hasPiece(msg.getIndex()));
}
  
void BtHaveMessageTest::testToString() {
  BtHaveMessage msg;
  msg.setIndex(1);

  CPPUNIT_ASSERT_EQUAL(std::string("have index=1"), msg.toString());
}

} // namespace aria2
