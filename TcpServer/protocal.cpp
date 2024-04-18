#include "protocal.h"


PDU* mkPDU(Uint uiMsgLend)
{
    Uint len = sizeof(PDU) + uiMsgLend;
    PDU *pdu = (PDU *)malloc(len);
    if(len == NULL){
        exit(1);
    }
    memset(pdu, 0, len);
    pdu->uilPDULen = len;
    pdu->uiMsgLen = uiMsgLend;
    return pdu;
}
