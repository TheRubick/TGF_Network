#include <fstream>
#include <stdio.h>

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

bool Node::isFileFinished()
{
    ifstream MyReadFile("node.txt");
    return (MyReadFile.peek() != EOF);
}

std::string Node::readLine()
{
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    ifstream MyReadFile("node.txt");
    
    getline(MyReadFile, myText);
  
    // Close the file
    MyReadFile.close(); 
    return myText;
}

void Node::createFile()
{
    // Create and open a text file whose name is node number
    ofstream MyFile("node.txt");

    //initialize the message number randomly
    msgNum = uniform(10,40); //to be defined in the node.h
    // Write to the file
    for(int i = 0;i < msgNum;i++)
        MyFile << Node::randomMsg()+"\n";

    // Close the file
    MyFile.close();
}

void Node::final()
{
    if( remove( "node.txt" ) != 0 ) // TODO should be modified
        perror( "Error deleting file" );
    else
        puts( "File successfully deleted" );
}
