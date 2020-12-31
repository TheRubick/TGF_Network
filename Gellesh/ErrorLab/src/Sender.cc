#include "Sender.h"
#include "MyMessage_m.h"

Define_Module(Sender);

std::string Sender::randomMsg(int lower, int upper) {
    //initialize msgSize randomly to be any number from 10 to 200
    int msgSize = uniform(lower, upper);
    //initialize msg variable to hold the message
    std::string msg = "";
    for (int i = 0; i < msgSize; i++)
        msg += char(uniform(33, 126)); //any character from the message would be chosen randomly from ! to ~  character
    return msg;
}

std::vector<std::bitset<M> > Sender::noiseModelling(
        std::vector<std::bitset<M> > message, int errorType) {

    std::string inputMsg = "";
    for (std::vector<std::bitset<M> >::const_iterator i = message.begin();
            i != message.end(); ++i) {
        inputMsg = inputMsg + (char) i->to_ulong();
    }

    if (errorType == 1) {
        // Modification

        // corrupt 1 bit with 30 % percentage
        int rand = uniform(0, 1) * 10;
        EV << " rand =  " << std::to_string(rand) << endl;

        int modRate = par("modRate").intValue();

        if (rand > modRate) // prob to corrupt a certain bit
                {
            // choose which character to modify
            int modCh = uniform(0, message.size() - 1);
            int bitNumber = uniform(0, M - 1);
            EV << "sender will corrupt a bit at  " << std::to_string(modCh)
                      << "  character bit number = " << bitNumber << endl;
            message[modCh].flip(bitNumber);

        }

        std::string final = "";
        for (std::vector<std::bitset<M> >::const_iterator i = message.begin();
                i != message.end(); ++i) {
            final = final + (char) i->to_ulong();
        }

        EV << inputMsg << "  =-=  " << final << endl;

        MyMessage_Base *msgSender = new MyMessage_Base("Hello");
        msgSender->setM_Payload(final.c_str());
        msgSender->setM_Type(0); // 0 is data and 1 is achknowledgment
        msgSender->setSeq_Num(12);
        //    std::bitset<8> temp('A');
        //    msgSender->setMycheckbits(parityCheck);
        send(msgSender, "out");

    } else if (errorType == 2) {
        // loss

        EV << " msg is lost  ..\n";
        scheduleAt(simTime() + 2, new cMessage("loss message"));

    }

    else if (errorType == 3) {
        // duplication

        MyMessage_Base *msgSender = new MyMessage_Base("Hello");
        msgSender->setM_Payload(inputMsg.c_str());
        msgSender->setM_Type(0); // 0 is data and 1 is achknowledgment
        msgSender->setSeq_Num(12);
        //    std::bitset<8> temp('A');
        //    msgSender->setMycheckbits(parityCheck);
        send(msgSender, "out");
        MyMessage_Base *msgSender2 = msgSender->dup();
        send(msgSender2, "out");
        EV << " msg is sent twice   ..\n";

    }

    else if (errorType == 4) {
        // delayed

        MyMessage_Base *msgSender = new MyMessage_Base("Hello");
        msgSender->setM_Payload(inputMsg.c_str());
        msgSender->setM_Type(0); // 0 is data and 1 is achknowledgment
        msgSender->setSeq_Num(12);

        double delay = uniform(0, 1);
        double inputDelay = par("delay").doubleValue();
        if (inputDelay != 0.0) {
            EV << "HERE" << endl;
            delay = inputDelay;
        }
        EV << " msg is delayed with " << delay << " secs ..\n";

        sendDelayed(msgSender, delay, "out");

    }

    return message;

}

void Sender::initialize() {
    scheduleAt(simTime() + 5, new cMessage(""));
}

void Sender::handleMessage(cMessage *msg) {


    cancelAndDelete(msg);



    //1) Generate random string
    std::string inputMsg = randomMsg(5, 10);
    EV << " Generated Msg is -> " << inputMsg;

    //2) calculate the char count
    int charCount = inputMsg.size();

    //3) define the vector of the bitsets contain binary representation of the character
    std::vector<std::bitset<M> > vec;

    for (int var = 0; var < charCount; ++var) {
        std::bitset<M> character(inputMsg[var]);
        vec.push_back(character);
    }

    // assume here framing and error detection bits are added

    //4) select the type of error on the channel

    int errorType = uniform(1, 4);

    switch (errorType) {
    case 1:
        printf("Choice is 1  Modification(*)");
        break;
    case 2:
        printf("Choice is 2  Loss(*)");
        break;
    case 3:
        printf("Choice is 3  duplication(*)");
        break;
    case 4:
        printf("Choice is 4  delayed(*)");
        break;
    default:
        break;
    }

    //6) loop on the vector print the message and convert every bitset to char
    //and append them to the final string message

    vec = noiseModelling(vec, errorType);

//    std::string final = "";
//    for (std::vector<std::bitset<M> >::const_iterator i = vec.begin();
//            i != vec.end(); ++i) {
//        final = final + (char) i->to_ulong();
//    }
//
//    EV << inputMsg << "  =-=  " << final << endl;


//    MyMessage_Base *msgSender = new MyMessage_Base("Hello");
//    msgSender->setM_Payload(final.c_str());
//    msgSender->setM_Type(0); // 0 is data and 1 is achknowledgment
//    msgSender->setSeq_Num(12);
//    //    std::bitset<8> temp('A');
////    msgSender->setMycheckbits(parityCheck);
//    send(msgSender, "out");

}

