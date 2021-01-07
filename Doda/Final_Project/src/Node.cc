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
#include <fstream>
#include <stdio.h>
using namespace std;
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
        EV<<" Sending Msg = "<<buffer[frameNum];
        float t =simTime().dbl();
        timers[frameNum] = t;
    }
    msg->setSeq(frameNum);
    int ack = (frameAck + maxSequenceNumber) % (maxSequenceNumber+1);
    msg->setAck(ack);
    msg->setDst(peer);
    send(msg,"out");


    EV<<" seq no = "<<std::to_string(frameNum);
    EV<<" with ack = "<<std::to_string(ack);
    EV<<" and msg = "<<buffer[frameNum];
}

int Node::inc_circular(int x){
    x = x+1;
    if (x > maxSequenceNumber)
    x = 0;
    return x;
}

void Node::reset(){
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
    peer = -1;
    resetFlag = true;
    msgNum = 0;
    if( remove(fileName.c_str()) != 0 )
        perror( "Error deleting file" );
    else
        puts( "File successfully deleted" );
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
    resetFlag = false;
    msgNum = 0;
    nodeNumber = 0;
    fileName = "";
}

void Node::handleMessage(cMessage *msg)
{
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
        // TODO - Generated method body
    //self msg either to cont working or time out
    if(mmsg->isSelfMessage()){//self message
        if(mmsg->getType() == 0){///self msg to send data
            if(capped == false && resetFlag == false){//sender buffer have available space
                //std::string c = "seq number = "+std::to_string(nextFrameToSend);//msg to be sent
                std::string c = Node::readLine();
                buffer[nextFrameToSend] = c;
                nBuffered++;
                Node::send_data(nextFrameToSend, frameExpected);
                nextFrameToSend = Node::inc_circular(nextFrameToSend);
                started = true;
            }
            if(resetFlag == false){
                MyMessage_Base * selfMsg = new MyMessage_Base("");
                selfMsg->setType(0);//self msg type 0 means send data to other node
                scheduleAt(simTime()+2, selfMsg);
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
        if(mmsg->getType() == 3){//init from hub to start transmission
            //todo may need to call reset() here again
            //get number of node to talk to
            peer = mmsg->getDst();
            //get my number
            nodeNumber = std::stoi(mmsg->getPayLoad());
            //create file to use
            Node::createFile();
            //intilaize by sending self message
            resetFlag = false;
            MyMessage_Base * msg = new MyMessage_Base("");
            msg->setType(0);//self msg type 0 means send data to other node
            scheduleAt(simTime()+2, msg);
        }
        else if(mmsg->getType() == 4){//from hub to stop transmission
            Node::reset();
        }
        else if(resetFlag == false){//msg from other node
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
    }
    if(started){//check for time out TODO may need to check for reset flag
        if(  ((simTime().dbl() - timers[ackExpected]) >   maxWaitTime) && timedOut == false){//if timed out
            MyMessage_Base * timeoutMsg = new MyMessage_Base("");
            timeoutMsg->setType(1);//self msg type 1 means time out on ack
            timedOut = true;
            scheduleAt(simTime()+1, timeoutMsg);
        }
    }
    if(nBuffered < maxSequenceNumber){//todo may need to check for reset flag
        capped = false;
    }
    else{
        capped = true;
    }

    EV<<" From node = "<<getName();
    EV<<" nextFrameToSend = "<<std::to_string(nextFrameToSend);
    EV<<" ackExpected = "<<std::to_string(ackExpected);
    EV<<" nBuffered = "<<std::to_string(nBuffered);
    EV<<" frameExpected = "<<std::to_string(frameExpected);

}



std::string Node::randomMsg()
{
    /*
        lazem n3ml check 3ala 7etet enena n-read and write from/to files w nzbtoha gwa el Node.cc beta3t TGF
    */
    //initialize msgSize randomly to be any number from 10 to 200
    int msgSize = uniform(10,200);
    //initialize msg variable to hold the message
    string msg = "";
    for(int i = 0;i < msgSize;i++)
        msg += char(uniform(33,126)); //any character from the message would be chosen randomly from ! to ~  character
    return msg;
}





std::string Node::readLine()
{
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    ifstream MyReadFile(fileName);

    getline(MyReadFile, myText);

    // Close the file
    MyReadFile.close();
    return myText;
}

void Node::createFile()
{
    // Create and open a text file whose name is node number
    fileName = "node" + std::to_string(nodeNumber) + ".txt";
    ofstream MyFile(fileName);

    //initialize the message number randomly
    msgNum = uniform(10,40); //to be defined in the node.h
    // Write to the file
    for(int i = 0;i < msgNum;i++)
        MyFile << Node::randomMsg()+"\n";

    // Close the file
    MyFile.close();
}


void Node::finish()
{
    if( remove( fileName.c_str() ) != 0 ) // TODO should be modified
        perror( "Error deleting file" );
    else
        puts( "File successfully deleted" );
}




