#ifndef SENDER_H_
#define SENDER_H_

#include <omnetpp.h>
#include <bitset>
#include <vector>
using namespace omnetpp;

#define M 8


/**
 * TODO - Generated class
 */
class Sender : public cSimpleModule
{
  protected:
    virtual std::string randomMsg(int lower , int upper);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual std::vector<std::bitset<M> > noiseModelling(std::vector<std::bitset<M> > message , int errorType);
};

#endif
