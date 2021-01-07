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

#include "Hub.h"
#include "MyMessage_m.h"

Define_Module(Hub);

int node1[10];
int node2[10];
int tableSize = 10;
int indx = 0;
int timerInterval = 15;
double prevSimTime;

void Hub::initialize()
{
    double interval = exponential(1 / par("lambda").doubleValue());
    scheduleAt(simTime() + interval , new cMessage(""));
    int oldn1 = -1;
    int oldn2 = -1;
    prevSimTime = simTime().dbl();
    for(int i =0; i<tableSize; i++){
        do { //Avoid choosing node1 again
            node1[i] = uniform(0, par("n").intValue()-1);
        } while(node1[i] == oldn1 || node1[i] == oldn2);

        do { //Avoid choosing node1 again
             node2[i] = uniform(0, par("n").intValue()-1);
        } while(node2[i] == node1[i] || node2[i] == oldn1 || node2[i] == oldn2);
        oldn1 = node1[i];
        oldn2 = node2[i];
    }
}

void Hub::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {

        MyMessage_Base* start_msg1 = new MyMessage_Base("start sender");
        MyMessage_Base* start_msg2 = new MyMessage_Base("start receiver");
        start_msg1->setDst(node2[indx]);
        start_msg1->setType(3);
        start_msg2->setDst(node1[indx]);
        start_msg2->setType(3);
        send(start_msg1, "outs", node1[indx]); // send to the first node to ask it to start sending
        send(start_msg2, "outs", node2[indx]); // send to the second node to ask it to start receiving
    }else{
            if (simTime().dbl() - prevSimTime >= timerInterval) {
                prevSimTime = simTime().dbl();
                MyMessage_Base* end_msg = new MyMessage_Base("stop");
                MyMessage_Base* end_msg0 = new MyMessage_Base("stop");
                end_msg->setType(4);
                end_msg0->setType(4);
                send(end_msg, "outs", node1[indx]); // send to the first node to ask it to stop sending
                send(end_msg0, "outs", node2[indx]); // send to the second node to ask it to stop sending
                indx++;
                if(indx == tableSize) indx = 0;
                MyMessage_Base* start_msg1 = new MyMessage_Base("start sender");
                MyMessage_Base* start_msg2 = new MyMessage_Base("start receiver");
                start_msg1->setDst(node2[indx]);
                start_msg1->setType(3);
                start_msg2->setDst(node1[indx]);
                start_msg2->setType(3);
                send(start_msg1, "outs", node1[indx]); // send to the first node to ask it to start sending
                send(start_msg2, "outs", node2[indx]); // send to the second node to ask it to start receiving


            }else{
                // message from one of the nodes
                MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
                int dest = mmsg->getDst();
                if(dest == node1[indx] || dest == node2[indx]){
                    EV << "Hub dst:::::::::"<< mmsg->getDst();
                    send(mmsg, "outs", mmsg->getDst());
                }
            }
    }

}
