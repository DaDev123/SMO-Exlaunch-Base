#pragma once

#include "sead/math/seadVector.h"
#include "sead/math/seadQuat.h"

#include "nn/account.h"

#include "types.h"

#define PACKBUFSIZE      0x30
#define COSTUMEBUFSIZE   0x20

#define MAXPACKSIZE      0x100

enum PacketType : short {
    UNKNOWN,
    CLIENTINIT,
    PLAYERCON,
    PLAYERDC,
    CHANGESTAGE,
    CMD,
    End // end of enum for bounds checking
};

// attribute otherwise the build log is spammed with unused warnings
USED static const char *packetNames[] = {
    "Unknown",
    "Client Initialization",
    "Player Connect",
    "Player Disconnect",
    "Change Stage",
    "Server Command"
};

enum SenderType {
    SERVER,
    CLIENT
};

enum ConnectionTypes {
    INIT,
    RECONNECT
};

// unused
/*
static const char *senderNames[] = {
    "Server",
    "Client"
};
*/

struct PACKED Packet {
    nn::account::Uid mUserID; // User ID of the packet owner
    PacketType mType = PacketType::UNKNOWN;
    short mPacketSize = 0; // represents packet size without size of header
};

// all packet types

#include "packets/PlayerConnect.h"
#include "packets/PlayerDC.h"
#include "packets/ServerCommand.h"
#include "packets/ChangeStagePacket.h"
#include "packets/InitPacket.h"
