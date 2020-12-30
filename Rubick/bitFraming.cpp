#include <iostream>
#include <bitset>
#include <string>
#include <math.h>
#include <climits>
using namespace std;

string framingMsg(string Msg)
{
	string binMsg = "";
	//convert the string to binary
	for(int i = 0;i < Msg.length();i++)
		binMsg += bitset<8>(Msg[i]).to_string();
	
	//initialize counter to indicate any sequence of five 1s
	int counter = 0;
	//initialize a temp variable
	string temp = "";
	for(int i = 0;i < binMsg.length();i++)
	{
		temp += binMsg[i];
		if(binMsg[i] == '1')
			counter++;
		else if(binMsg[i] == '0')
			counter = 0;
		if (counter == 5)
		{
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
	return binMsg;
}

string deframingMsg(string binMsg)
{
	//initialize counter to indicate any sequence of five 1s
	int counter = 0;
	//initialize the Msg that would hold the string converted from binMsg
	string Msg = "";
	for(int i = 0;i < binMsg.length();i++)
	{
		Msg += binMsg[i];
		if(binMsg[i] == '1')
			counter++;
		else if(binMsg[i] == '0')
			counter = 0;
		if (counter == 5)
		{
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
	for(int i = 0;i < Msg.length();i+=8)
	{
		string letter = "";
		for(int j = i;j < i+8;j++)
			letter += Msg[j];
		//convert each 8 bits "1 byte" sequence into character and add it to temp variable
		temp += char(bitset<8>(letter).to_ulong());
	}
	//assign the message with temp
	Msg = temp;
	return Msg;
}
