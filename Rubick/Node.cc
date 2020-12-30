std::string Node::randomMsg()
{
    //initialize msgSize randomly to be any number from 10 to 200
    int msgSize = uniform(10,200);
    //initialize msg variable to hold the message
    string msg = "";
    for(int i = 0;i < msgSize;i++)
        msg += char(uniform(33,126)); //any character from the message would be chosen randomly from ! to ~  character
    return msg;
}
