#ifndef PARSE_H
#define PARSE_H
#include <iostream>
#include <vector>

enum MessageType {
  GRANT = 0,
  STATUS = 1,
  UPDATE = 2,
  CONTROL_CHANNEL = 3,
  REGISTRATION = 4,
  DEREGISTRATION = 5,
  AFFILIATION = 6,
  SYSID = 7,
  ACKNOWLEDGE = 8,
  LOCATION = 9,
  UNKNOWN = 99
};

struct TrunkMessage {
  MessageType message_type;
  std::string meta;
  double freq;
  long talkgroup;
  bool encrypted;
  bool emergency;
  bool duplex;
  bool mode;
  long priority;
  int tdma_slot;
  bool phase2_tdma;
  long source;
  int sys_num;
  unsigned long sys_id;
  unsigned long nac;
  unsigned long wacn;
};

class TrunkParser {
  std::vector<TrunkMessage> parse_message(std::string s);
};
#endif
