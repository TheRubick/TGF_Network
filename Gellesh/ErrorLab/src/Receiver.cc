#include "Receiver.h"
#include "MyMessage_m.h"


Define_Module(Receiver);

void Receiver::initialize() {
    // TODO - Generated method body
}

void Receiver::handleMessage(cMessage *msg) {

    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
    EV << "received message with sequence number ...   ";
    EV << mmsg->getSeq_Num();
    EV << "  and payload of ... ";
    EV << mmsg->getM_Payload();
    EV << "   and check bits of ...";
    EV << mmsg->getMycheckbits().to_string();
    send(mmsg, "out");

}
