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

#include <fstream>

string Node::readLine()
{
    string line;
    return line;
}

void Node::createFile()
{
    // Create and open a text file whose name is node number
    ofstream MyFile("node.txt");

    //initialize the message number randomly
    int msgNum = uniform(10,40);
    // Write to the file
    for(int i = 0;i < msgNum;i++)
        MyFile << Node::randomMsg();

    // Close the file
    MyFile.close();
}