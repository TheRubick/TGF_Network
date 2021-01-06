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

void Node::initialize()
{
}

void Node::handleMessage(cMessage *msg)
{
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
    if(strcmp(mmsg->getName(), "start sender") == 0){
        peer = mmsg->getDst();
        std::stringstream ss;
        ss << peer;
        EV << "sender peer:::::::::"<< ss.str()<< "\n";
        scheduleAt(simTime(), new MyMessage_Base(""));

    }else{
        if(strcmp(mmsg->getName(), "start receiver") == 0){
            peer = mmsg->getDst();
            EV << "receiver peer:::::::::"<< peer;
        }else{
                //msg from another node
                bubble("Message received");
                MyMessage_Base *nmsg = new MyMessage_Base("Data");
                nmsg->setDst(peer);
                std::stringstream ss;
                ss << peer;
                EV << "node peer:::::::::"<< ss.str()<< "\n";
                send(nmsg, "out");
        }
    }
}
