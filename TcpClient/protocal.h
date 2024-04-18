#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

using Uint = unsigned;
struct PDU{
    Uint uilPDULen;
    Uint UiMsgType;
    char caData[64];
    Uint uiMsgLen;
    int caMsg[];
};

PDU* mkPDU(Uint uiMsgLend);


#endif // PROTOCAL_H
