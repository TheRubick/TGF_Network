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


MyMessage_Base* timer = new MyMessage_Base("timer");
double timerInterval = 5.0; // five seconds
int node1, node2;
MyMessage_Base* start_msg1 = new MyMessage_Base("start sender");
MyMessage_Base* start_msg2 = new MyMessage_Base("start receiver");
MyMessage_Base* end_msg = new MyMessage_Base("stop");


void Hub::initialize()
{
    double interval = exponential(1 / par("lambda").doubleValue());
    scheduleAt(simTime() + interval, new cMessage(""));
    scheduleAt(simTime() + timerInterval, timer);
}

void Hub::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        node1 = uniform(0, par("n").intValue()-1);

        do { //Avoid choosing node1 again
            node2 = uniform(0, par("n").intValue()-1);
        } while(node2 == node1);

        start_msg1->setPeer(node2);
        start_msg2->setPeer(node1);
        send(start_msg1, "outs", node1); // send to the first node to ask it to start sending
        send(start_msg2, "outs", node2); // send to the second node to ask it to start receiving
    }else{
        if (strcmp(getName(), "timer") == 0) {

               send(end_msg, "outs", node1); // send to the first node to ask it to stop sending
               send(end_msg, "outs", node2); // send to the second node to ask it to stop sending

               node1 = uniform(0, par("n").intValue()-1);

               do { //Avoid choosing node1 again
                    node2 = uniform(0, par("n").intValue()-1);
               } while(node2 == node1);

               start_msg1->setPeer(node2);
               start_msg2->setPeer(node1);
               send(start_msg1, "outs", node1); // send to the first node to ask it to start sending
               send(start_msg2, "outs", node2); // send to the second node to ask it to start receiving

               scheduleAt(simTime() + timerInterval, timer);  // rescheduling
        }else{
            // message from one of the nodes
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            send(mmsg, "outs", mmsg->getDst());
        }
    }


}
