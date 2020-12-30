//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Node.h"
#include "MyMessage_m.h"

Define_Module(Node);

bool Node::between(int a,int  b,int c){
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
    return(true);
    else
    return(false);
}


void Node::send_data(int frameNum, int frameAck){
    MyMessage_Base * msg = new MyMessage_Base("");
    if(frameNum == -1){
        msg->setType(1);//type 1 meens ack only without data
        msg->setPayLoad("");
    }
    else{
        msg->setType(0);//type 0 means data and ack
        msg->setPayLoad(buffer[frameNum].c_str());
        float t =simTime().dbl();
        timers[frameNum] = t;
    }
    msg->setSeq(frameNum);
    int ack = (frameAck + maxSequenceNumber) % (maxSequenceNumber+1);
    msg->setAck(ack);
    send(msg,"out");
    EV<<" From node = "<<getName();
    EV<<" sending msg with seq no = "<<std::to_string(frameNum);
    EV<<" with ack = "<<std::to_string(ack);
    EV<<" and msg = "<<buffer[frameNum];
}

int Node::inc_circular(int x){
    x = x+1;
    if (x > maxSequenceNumber)
    x = 0;
    return x;
}

void Node::initialize()
{
    nextFrameToSend = 0;
    ackExpected = 0;
    frameExpected = 0;
    nBuffered = 0;
    maxSequenceNumber = 7;
    buffer = new std::string[maxSequenceNumber+1];
    capped = false;
    timers = new float[maxSequenceNumber+1];
    started = false;
    timedOut = false;
    maxWaitTime = 10.0;
    //intilaize by sending self message
    MyMessage_Base * msg = new MyMessage_Base("");
    msg->setType(0);//self msg type 0 means send data to other node
    scheduleAt(simTime()+2, msg);

}

void Node::handleMessage(cMessage *msg)
{
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
    // TODO - Generated method body
    if(mmsg->isSelfMessage()){//self message
        if(mmsg->getType() == 0){///self msg to send data
            if(capped == false){//sender buffer have available space
                std::string c = "seq number = "+std::to_string(nextFrameToSend);//msg to be sent
                buffer[nextFrameToSend] = c;
                nBuffered++;
                Node::send_data(nextFrameToSend, frameExpected);
                nextFrameToSend = Node::inc_circular(nextFrameToSend);
                started = true;
            }
        }
        else if(mmsg->getType() == 1){//self message to indicate timeout
            EV<<" From node = "<<getName()<<" timed out\n";
            timedOut = false;
            nextFrameToSend = ackExpected;
            for(int i = 0; i<nBuffered;i++){
                Node::send_data(nextFrameToSend, frameExpected);
                nextFrameToSend = Node::inc_circular(nextFrameToSend);
            }
        }
    }
    else{//if not self message
        //first handle the received ack
        EV<<" From node = "<<getName()<<" recevied actual message\n";
        int receivedAck = mmsg->getAck();
        EV<<" ackExpected before while= "<<std::to_string(ackExpected)<<"\n";
        EV<<" receivedAck = "<<std::to_string(receivedAck);
        EV<<" nextFrameToSend = "<<std::to_string(nextFrameToSend);
        while( Node::between(ackExpected, receivedAck, nextFrameToSend) ){
            nBuffered--;
            ackExpected = Node::inc_circular(ackExpected);
            EV<<" Inside While loop ";
        }
        EV<<" ackExpected after while= "<<std::to_string(ackExpected)<<"\n";
        //check for type to know if it contained valid data or not
        if(mmsg->getType() == 0){
            if(mmsg->getSeq() == frameExpected){
                frameExpected = Node::inc_circular(frameExpected);
                std::string m= mmsg->getPayLoad();
                bubble(m.c_str());
            }
        }
    }

    if(started){//check for time out
        if(  ((simTime().dbl() - timers[ackExpected]) >   maxWaitTime) && timedOut == false){//if timed out
            MyMessage_Base * timeoutMsg = new MyMessage_Base("");
            timeoutMsg->setType(1);//self msg type 1 means time out on ack
            timedOut = true;
            scheduleAt(simTime()+1, timeoutMsg);
        }
    }
    if(nBuffered < maxSequenceNumber){
        capped = false;
    }
    else{
        capped = true;
    }
    MyMessage_Base * selfMsg = new MyMessage_Base("");
    selfMsg->setType(0);//self msg type 0 means send data to other node
    scheduleAt(simTime()+2, selfMsg);
    EV<<" From node = "<<getName();
    EV<<" nextFrameToSend = "<<std::to_string(nextFrameToSend);
    EV<<" ackExpected = "<<std::to_string(ackExpected);
    EV<<" nBuffered = "<<std::to_string(nBuffered);
    EV<<" frameExpected = "<<std::to_string(frameExpected);
}
