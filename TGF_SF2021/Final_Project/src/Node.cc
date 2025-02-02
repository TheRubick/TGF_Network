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
using namespace std;
Define_Module(Node);

bool Node::between(int a, int b, int c) {
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return (true);
    else
        return (false);
}

int Node::generateRandom(int lowerRange, int UpperRange) {
    // return random integer between the given range
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(lowerRange, UpperRange); // define the range
    return distr(gen);
}



void Node::send_data(int frameNum, int frameAck) {
    MyMessage_Base *msg = new MyMessage_Base("");

    if (frameNum == -1) {
        msg->setType(1); //type 1 meens ack only without data
        msg->setPayLoad("");
    } else {
        msg->setType(0); //type 0 means data and ack
        //msg->setPayLoad(buffer[frameNum].c_str());
        string payload = buffer[frameNum];
        //apply framing
        payload = framingMsg(payload);
        //hamming code
        payload = hammingCode(payload);
        msg->setPayLoad(payload.c_str());
        EV << " Sending Msg = " << buffer[frameNum];
        float t = simTime().dbl();
        timers[frameNum] = t;
    }
    msg->setSeq(frameNum);
    int ack = (frameAck + maxSequenceNumber) % (maxSequenceNumber + 1);
    msg->setAck(ack);
    msg->setDst(peer);
    //applying errors and sending msg
    noiseModelling(msg);
    //send(msg, "out");

    EV << " seq no = " << std::to_string(frameNum);
    EV << " with ack = " << std::to_string(ack)<<"\n";

}

int Node::inc_circular(int x) {
    x = x + 1;
    if (x > maxSequenceNumber)
        x = 0;
    return x;
}

void Node::reset() {
    nextFrameToSend = 0;
    ackExpected = 0;
    frameExpected = 0;
    nBuffered = 0;
    //maxSequenceNumber = par("maxSequenceNumber").intValue();
    //buffer = new std::string[maxSequenceNumber + 1];
    capped = false;
    //timers = new float[maxSequenceNumber + 1];
    started = false;
    timedOut = false;
    //maxWaitTime = par("timeOut").doubleValue();
    //selfMsgDelay = par("selfMsgDelay").doubleValue();
    //selfTimeOutEventDelay = par("selfTimeOutEventDelay").doubleValue();
    peer = -1;
    resetFlag = true;
    msgNum = 0;
    currentMsg = 0;
    /*
    if (remove(fileName.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");
    */
   MyReadFile.close();
}

void Node::initialize() {
    nextFrameToSend = 0;
    ackExpected = 0;
    frameExpected = 0;
    nBuffered = 0;
    maxSequenceNumber = par("maxSequenceNumber").intValue();
    buffer = new std::string[maxSequenceNumber + 1];
    capped = false;
    timers = new float[maxSequenceNumber + 1];
    started = false;
    timedOut = false;
    maxWaitTime = par("timeOut").doubleValue();
    selfMsgDelay = par("selfMsgDelay").doubleValue();
    selfTimeOutEventDelay = par("selfTimeOutEventDelay").doubleValue();
    resetFlag = false;
    msgNum = 0;
    nodeNumber = 0;
    fileName = "";
    currentMsg = 0;
    generatedFrames = 0;
    droppedFrames = 0;
    retransmittedFrames = 0;
    duplicatedFrames = 0;
    freopen ("logs.txt","a",stdout);
}

void Node::handleMessage(cMessage *msg) {
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base*>(msg);
    // TODO - Generated method body
    //self msg either to cont working or time out
    if (mmsg->isSelfMessage() && resetFlag == false) {    //self message
        if (mmsg->getType() == 0) {    ///self msg to send data
            if (capped == false) { //sender buffer have available space
            //std::string c = "seq number = "+std::to_string(nextFrameToSend);//msg to be sent
                if(isFileFinished()){
                    MyMessage_Base *msgToHub = new MyMessage_Base("");
                    msgToHub->setType(7);
                    send(msgToHub,"out");
                    EV<<"\n"<<"/////////////////////////////////////////////"<<endl;
                    EV<<"      File have no more data"<<endl;
                    EV<<"/////////////////////////////////////////////////"<<endl;
                }
                else{
                    std::string c = Node::readLine();
                    cout << "node number = " << nodeNumber + 1 << endl;
                    cout << "sending message = " << c << endl;
                    buffer[nextFrameToSend] = c;
                    nBuffered++;
                    Node::send_data(nextFrameToSend, frameExpected);
                    generatedFrames++;
                    nextFrameToSend = Node::inc_circular(nextFrameToSend);
                    started = true;
                }
            }

            MyMessage_Base *selfMsg = new MyMessage_Base("");
            selfMsg->setType(0); //self msg type 0 means send data to other node
            scheduleAt(simTime() + selfMsgDelay, selfMsg);

        } else if (mmsg->getType() == 1) {    //self message to indicate timeout
            EV << " From node = " << getName() << " timed out\n";
            timedOut = false;
            nextFrameToSend = ackExpected;
            cout << "node number = " << nodeNumber + 1 << endl;
            for (int i = 0; i < nBuffered; i++) {
                Node::send_data(nextFrameToSend, frameExpected);
                retransmittedFrames++;
                generatedFrames++;
                cout << "retransmitting message payload = " << buffer[nextFrameToSend] << endl;
                nextFrameToSend = Node::inc_circular(nextFrameToSend);
            }
        }
    } else {                //if not self message
        if (mmsg->getType() == 3) {        //init from hub to start transmission
            //todo may need to call reset() here again
            //get number of node to talk to
            peer = mmsg->getDst();
            //get my number
            nodeNumber = std::stoi(mmsg->getPayLoad());
            //create file to use
            //Node::createFile();
            fileName = "node" + std::to_string(nodeNumber + 1) + ".txt";
            MyReadFile.open(fileName);
            //intilaize by sending self message
            resetFlag = false;
            MyMessage_Base *msg = new MyMessage_Base("");
            msg->setType(0);     //self msg type 0 means send data to other node
            scheduleAt(simTime() + selfMsgDelay, msg);
        } else if (mmsg->getType() == 4) {       //from hub to stop transmission
            Node::reset();
        } else if (resetFlag == false) {            //msg from other node
            //first handle the received ack
            EV << " From node = " << getName() << " recevied actual message\n";
            int receivedAck = mmsg->getAck();
            EV << " ackExpected before while= " << std::to_string(ackExpected)
                      << "\n";
            EV << " receivedAck = " << std::to_string(receivedAck);
            EV << " nextFrameToSend = " << std::to_string(nextFrameToSend);
            while (Node::between(ackExpected, receivedAck, nextFrameToSend)) {
                nBuffered--;
                ackExpected = Node::inc_circular(ackExpected);
                EV << " Inside While loop ";
            }
            EV << " ackExpected after while= " << std::to_string(ackExpected)
                      << "\n";
            cout << "node = " << nodeNumber + 1 << " received message " <<endl;
            cout << "seq number = " << mmsg->getSeq() << endl;
            //check for type to know if it contained valid data or not
            if (mmsg->getType() == 0) {
                std::string m = mmsg->getPayLoad();
                EV << "before hamming decode " << m.c_str() << endl;
                m = hammingDecode(m);
                EV << "before deframming " << deframingMsg(m) << endl;
                m = deframingMsg(m);
                cout << "message type is piggybacked \n";
                cout << "message payload = " << m << endl;
                if (mmsg->getSeq() == frameExpected) {
                    frameExpected = Node::inc_circular(frameExpected);
                    bubble(m.c_str());
                }
                else{
                    droppedFrames++;
                }
            }else{
                cout << "message type is ack \n";
            }

        }
    }
    if (started && resetFlag == false) {    //check for time out TODO may need to check for reset flag
        if (((simTime().dbl() - timers[ackExpected]) > maxWaitTime && nBuffered > 0)
                && timedOut == false) {            //if timed out
            MyMessage_Base *timeoutMsg = new MyMessage_Base("");
            timeoutMsg->setType(1);      //self msg type 1 means time out on ack
            timedOut = true;
            EV<<"Time out event"<<endl;
            cout << "time out for frame number "<< ackExpected <<" at node number = " << nodeNumber + 1 << endl;
            scheduleAt(simTime() + selfTimeOutEventDelay, timeoutMsg);
        }
    }
    if(resetFlag == false){
        if (nBuffered < maxSequenceNumber) { //todo may need to check for reset flag
            capped = false;
        } else {
            capped = true;
        }
    }
    EV << " From node = " << getName();
    EV << " nextFrameToSend = " << std::to_string(nextFrameToSend);
    EV << " ackExpected = " << std::to_string(ackExpected);
    EV << " nBuffered = " << std::to_string(nBuffered);
    EV << " frameExpected = " << std::to_string(frameExpected);

}

std::string Node::randomMsg() {
    //initialize msgSize randomly to be any number from 10 to 90
    //int msgSize = uniform(10, 90);
    //initialize msg variable to hold the message
    int msgSize = generateRandom(10,50);
    string msg = "";
    for (int i = 0; i < msgSize; i++)
        msg += char(generateRandom(65,90));
        //msg += char(uniform(65, 90)); //any character from the message would be chosen randomly from A to Z  character
    return msg;
}

std::string Node::readLine() {
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    //ifstream MyReadFile(fileName);

    getline(MyReadFile, myText);
    //for (int i = 0; i < currentMsg; i++)
       // getline(MyReadFile, myText);
    //currentMsg++;
    // Close the file
    return myText;
}

bool Node::isFileFinished() {
    return MyReadFile.eof();
}

void Node::createFile() {
    // Create and open a text file whose name is node number
    fileName = "node" + std::to_string(nodeNumber + 1) + ".txt";
    ofstream MyFile(fileName);

    //initialize the message number randomly
    //cout << 5+(10*nodeNumber)<< " " << 40+(10*nodeNumber) << endl;
    //msgNum = uniform(5 + (10 * nodeNumber), 40 + (10 * nodeNumber)); //to be defined in the node.h
    msgNum = generateRandom(10,30);
    // Write to the file
    for (int i = 0; i < msgNum; i++)
        MyFile << Node::randomMsg() + "\n";

    // Close the file
    MyFile.close();
}

void Node::noiseModelling(MyMessage_Base *message) {
    int errorType = generateRandom(1, 4);
    //errorType = 4;
    std::string payload = message->getPayLoad();

    int rand = generateRandom(1,10);

    if (errorType == 1) {
        // Modification

        EV << "\n rand =  " << std::to_string(rand) << endl;

        int modRate = par("modRate").intValue();

        
        cout << "node = " << nodeNumber + 1 + 1<< endl;
        if (rand > modRate) // prob to corrupt a certain bit
                {
            EV << "before modification = " << payload << endl;
            // choose which bit to modify
            int bitNumber = generateRandom(0, payload.size() - 1);
            EV << "\n sender will corrupt a bit at  "
                      << std::to_string(bitNumber) << endl;
            
            cout << "sender will corrupt a bit at " << bitNumber << endl;
            cout << "payload before = " << payload << endl;
            if (payload[bitNumber] == '0') {
                payload[bitNumber] = '1';
            } else {
                payload[bitNumber] = '0';
            }
            cout << "payload after =  " << payload << endl;
            EV << "after modification =  " << payload << endl;

        }

        
        message->setPayLoad(payload.c_str());

        send(message, "out");

    } else if (errorType == 2) {
        // loss

        int lossRate = par("lossRate").intValue();

        if (rand > lossRate) {
            EV << "\n msg is lost  ..\n";
            cout << "msg is lost " << endl;
        } else {
            send(message, "out");
        }

    }

    else if (errorType == 3) {
        // duplication

        send(message, "out");
        MyMessage_Base *msgSender2 = message->dup();
        int dupRate = par("dupRate").intValue();

        if (rand > dupRate) {
            send(msgSender2, "out");
            EV << "\n msg is sent twice   ..\n";
            cout << "msg is sent twice  .." << endl;
            duplicatedFrames++;
        }

    }

    else if (errorType == 4) {
        // delayed

        int delayRate = par("delayRate").intValue();

        if (rand > delayRate) {
            double delay = generateRandom(0, 2);
            double inputDelay = par("delay").doubleValue();
            if (inputDelay != 0.0) {
                delay = inputDelay;
            }
            EV << "\n msg is delayed with " << delay << " secs ..\n";
            cout << "msg is delayed with " << delay << " secs " << endl;
            sendDelayed(message, delay, "out");
        } else {
            send(message, "out");
        }

    }

}
//////////////////////////////////////////////////////////////////////
string Node::hammingCode(string binMsg) {
    //initialize r "redundant bits count" with 0
    int r = 0;
    //loop till we get the satisfies the following equation : 2**r >= (m + r + 1)
    while (!(pow(2, r) >= (binMsg.length() + r + 1))) {
        r++;
        if (pow(2, r) - 1 >= INT_MAX) // in case it reached the overflow
            return ""; //means the hamming code failed
    }

    //initialize n to be the message bits + redundant bits
    int n = binMsg.length() + r;

    string temp = "", //initialize temp variable
            codedBinMsg = ""; //initialize codedBinMsg which will hold the result
    for (int i = 1, m = 0, ri = 0; i <= n; i++) {
        /*
         in case that i is base of 2 i.e. i = 2,4,8,16,... then initialize it with zero
         otherwise initialize the temp variable with the message bit
         */
        if (pow(2, ri) == i) {
            //increment the redundant bits counter
            ri++;
            //initialize redundant bits with zero
            temp += '0';
        } else {
            temp += binMsg[m];
            //increment the message bits counter
            m++;
        }
    }
    /*
     for each bit from the redundant bits , recalculate it with the message bits that corresponds to it by xoring them
     */
    for (int ri = 0; ri < r; ri++) {
        //riIndex variable to indicate the current redundant bit
        int riIndex = int(pow(2, ri)) - 1;
        for (int i = 1; i <= n; i++) {
            //if the iterator i is the same as the current redundant bit then skip it
            if (int(pow(2, ri)) == i)
                continue;
            if ((int(pow(2, ri)) & i) == int(pow(2, ri))) {
                int riValue = bitset<1>(temp[riIndex]).to_ulong()
                        ^ bitset<1>(temp[i - 1]).to_ulong();
                temp[int(pow(2, ri)) - 1] = riValue == 0 ? '0' : '1';
            }
        }
    }

    //initialize the flag
    string flag = "01111110";
    //assign the codedBinMsg with the temp
    codedBinMsg = flag + temp + flag;
    return codedBinMsg;
}


string Node::hammingDecode(string codedBinMsg) {
    string bitToInvert = "", //bitToInvert will hold the index of the single bit error
            binMsg = "", //will hold the binMsg to be returned "after removing the hamming code"
            temp = ""; //initialize temp variable with codedBinMsg
    //remove the flags from the message
    for (int i = 8; i < codedBinMsg.length() - 8; i++)
        temp += codedBinMsg[i];

    codedBinMsg = temp;
    int r = int(ceil(log2(codedBinMsg.length() + 1))), //initialize r "redundant bits count"
    n = codedBinMsg.length(); //initialize n with the codedBinMsg length
    /*
     for each bit from the redundant bits , recalculate it with the message bits that corresponds to it by xoring them
     */
    
    for (int ri = 0; ri < r; ri++) {
        //riIndex variable to indicate the current redundant bit
        int riIndex = int(pow(2, ri)) - 1;
        for (int i = 1; i <= n; i++) {
            //if the iterator i is the same as the current redundant bit then skip it
            if (int(pow(2, ri)) == i)
                continue;
            if ((int(pow(2, ri)) & i) == int(pow(2, ri))) {
                int riValue = bitset<1>(temp[riIndex]).to_ulong()
                        ^ bitset<1>(temp[i - 1]).to_ulong();
                temp[int(pow(2, ri)) - 1] = riValue == 0 ? '0' : '1';
            }
        }
        //add the calculated values of the redundant bits to bitToInvert
        bitToInvert += temp[riIndex];
    }

    //convert string bitToInvert to invertIndex
    int invertIndex = 0;
    for (int i = 0; i < bitToInvert.length(); i++)
        invertIndex += int(pow(2, i) * bitset<1>(bitToInvert[i]).to_ulong());


    //if invertIndex > 0 means there is a single bit should be inverted
    if (invertIndex > 0) {
        /*
         if the bits is '1' flip it to zero and vice versa
         */
        if (temp[invertIndex - 1] == '1')
            temp[invertIndex - 1] = '0';
        else
            temp[invertIndex - 1] = '1';
    
    }

    //assign binMsg with the message bits only i.e. without the redundant bits
    for (int i = 1, ri = 0; i <= n; i++) {
        if (int(pow(2, ri)) == i) {
            ri++;
        } else {
            binMsg += temp[i - 1];
        }
    }

    if(invertIndex > 0)
    {
        cout << "node number = " << nodeNumber + 1 << endl;
        cout << "error is corrected where bit number = " << invertIndex << " is inverted \n";
        cout << "message after error correction = " << Node::deframingMsg(binMsg) << endl;
    }

    return binMsg;
}



string Node::framingMsg(string Msg) {
    string binMsg = "";
    //convert the string to binary
    for (int i = 0; i < Msg.length(); i++)
        binMsg += bitset<8>(Msg[i]).to_string();

    //before framing
    cout << "node = " << nodeNumber + 1<< endl;
    cout << "before framing = " << binMsg << endl;
    //initialize counter to indicate any sequence of five 1s
    int counter = 0;
    //initialize a temp variable
    string temp = "";
    for (int i = 0; i < binMsg.length(); i++) {
        temp += binMsg[i];
        if (binMsg[i] == '1')
            counter++;
        else if (binMsg[i] == '0')
            counter = 0;
        if (counter == 5) {
            /*
             in case there is five sequence of 1s then add zero
             */
            //reset the counter
            counter = 0;
            //add zero charachter to the temp string
            temp += '0';
        }

    }
    //assign the binMsg with the temp
    binMsg = temp;
    cout << "after framing =  " << binMsg << endl;
    return binMsg;
}

string Node::deframingMsg(string binMsg) {
    //initialize counter to indicatedummy any sequence of five 1s
    int counter = 0;
    //initialize the Msg that would hold the string converted from binMsg
    string Msg = "";
    for (int i = 0; i < binMsg.length(); i++) {
        Msg += binMsg[i];
        if (binMsg[i] == '1')
            counter++;
        else if (binMsg[i] == '0')
            counter = 0;
        if (counter == 5) {
            /*
             in case there is five sequence of 1s then skip adding the next charachter
             */
            //reset the counter
            counter = 0;
            //increment the iterator to make it pass the zero charachter
            i++;
        }
    }
    //initialize temp variable to store the characters of the message
    string temp = "";
    for (int i = 0; i < Msg.length(); i += 8) {
        string letter = "";
        for (int j = i; j < i + 8; j++)
            letter += Msg[j];
        //convert each 8 bits "1 byte" sequence into character and add it to temp variable
        temp += char(bitset<8>(letter).to_ulong());
    }
    //assign the message with temp
    Msg = temp;
    return Msg;
}

///////////////////////////////////////////////////

void Node::finish() {
    delete[] timers;
    delete[] buffer;
    /*
    if (remove(fileName.c_str()) != 0) // TODO should be modified
        perror("Error deleting file");
    else
        puts("File successfully deleted");
    */
    
    MyReadFile.close();
    recordScalar("Number of generated frames     = ",generatedFrames);
    recordScalar("Number of dropped frames          = ",droppedFrames);
    recordScalar("Number of retransmitted frames = ",retransmittedFrames);
    cout << "node number = " << nodeNumber << endl;
    cout << "Number of generated frames     = " << generatedFrames << endl;
    cout << "Number of dropped frames       = " << droppedFrames << endl;
    cout << "Number of retransmitted frames = " << retransmittedFrames << endl;
    
    double numerator = (double)(generatedFrames-retransmittedFrames);
    double denominator = (double)(generatedFrames);
    double percentage = 0.0;
    if (denominator != 0){
        percentage = numerator/denominator;
    }
    recordScalar("Percentage of Useful Frames    = ",percentage);
    recordScalar("/////////////////////////////////////////////////////////////////////////",0);
    cout << "Percentage of Useful Frames    = " << percentage << endl;
    //fclose(stdout);
}

