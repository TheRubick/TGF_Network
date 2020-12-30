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

int peer_node = -1;


void Node::initialize()
{
}

void Node::handleMessage(cMessage *msg)
{
    if(strcmp(getName(), "start sender") == 0){
        MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
        peer_node = mmsg->getPeer();
        MyMessage_Base *nmsg = new MyMessage_Base("hello!!");
        nmsg->setDst(peer_node);
        send(nmsg, "out");

    }else{
        if(strcmp(getName(), "start receiver") == 0){
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            peer_node = mmsg->getPeer();
        }else{
            if(strcmp(getName(), "stop") == 0){
                peer_node = -1;
            }else{
                //msg from another node
                MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
                bubble("Message received");
                MyMessage_Base *nmsg = new MyMessage_Base("hello!!");
                nmsg->setDst(peer_node);
                send(nmsg, "out");
            }
        }
    }
}
