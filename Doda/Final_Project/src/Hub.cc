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

int node1, node2;
int counter = 0;
int timerInterval;

void Hub::initialize()
{
    //MyMessage_Base* timer = new MyMessage_Base("timer");
    double interval = exponential(1 / par("lambda").doubleValue());
    scheduleAt(simTime() + interval , new cMessage(""));
    //scheduleAt(simTime() + 30, timer);
    timerInterval = uniform(5, 10);
}

void Hub::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Uniform:"<< uniform(0, par("n").intValue()-1);
        node1 = uniform(0, par("n").intValue()-1);

        do { //Avoid choosing node1 again
            node2 = uniform(0, par("n").intValue()-1);
        } while(node2 == node1);
        MyMessage_Base* start_msg1 = new MyMessage_Base("start sender");
        MyMessage_Base* start_msg2 = new MyMessage_Base("start receiver");
        start_msg1->setDst(node2);
        start_msg1->setType(3);
        start_msg2->setDst(node1);
        start_msg2->setType(3);
        send(start_msg1, "outs", node1); // send to the first node to ask it to start sending
        send(start_msg2, "outs", node2); // send to the second node to ask it to start receiving
    }else{
            counter++;
            EV << "Counter:"<< counter;
            if (counter == timerInterval) {

                MyMessage_Base* end_msg = new MyMessage_Base("stop");
                MyMessage_Base* end_msg0 = new MyMessage_Base("stop");
                end_msg->setType(4);
                end_msg0->setType(4);
                send(end_msg, "outs", node1); // send to the first node to ask it to stop sending
                send(end_msg0, "outs", node2); // send to the second node to ask it to stop sending
                int oldn1 = node1;
                int oldn2 = node2;
                do { //Avoid choosing node1 again
                    node1 = uniform(0, par("n").intValue()-1);
                } while(node1 == oldn1 || node1 == oldn2);

                do { //Avoid choosing node1 again
                     node2 = uniform(0, par("n").intValue()-1);
                } while(node2 == node1 || node2 == oldn1 || node2 == oldn2);

                MyMessage_Base* start_msg1 = new MyMessage_Base("start sender");
                MyMessage_Base* start_msg2 = new MyMessage_Base("start receiver");
                start_msg1->setDst(node2);
                start_msg1->setType(3);
                start_msg2->setDst(node1);
                start_msg2->setType(3);
                send(start_msg1, "outs", node1); // send to the first node to ask it to start sending
                send(start_msg2, "outs", node2); // send to the second node to ask it to start receiving
                timerInterval = uniform(5, 10);
                //MyMessage_Base* timer = new MyMessage_Base("timer");
                counter = 0;

            }else{
                // message from one of the nodes
                MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
                int dest = mmsg->getDst();
                if(dest == node1 || dest == node2){
                    EV << "Hub dst:::::::::"<< mmsg->getDst();
                    send(mmsg, "outs", mmsg->getDst());
                }
            }
    }

}
