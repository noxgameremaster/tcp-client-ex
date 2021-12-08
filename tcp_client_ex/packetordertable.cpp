
#include "packetordertable.h"
#include "chatpacket.h"
#include "echopacket.h"
#include "testpacket.h"
#include "filepacket.h"

int PacketOrderTable<ChatPacket>::s_id = 1;
int PacketOrderTable<EchoPacket>::s_id = 2;
int PacketOrderTable<TestPacket>::s_id = 3;
int PacketOrderTable<FilePacket>::s_id = 4;

