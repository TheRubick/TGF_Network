#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <omnetpp.h>
#include <bitset>
#include <vector>
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Receiver : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
