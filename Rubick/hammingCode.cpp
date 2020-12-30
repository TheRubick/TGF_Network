#include <iostream>
#include <bitset>
#include <string>
#include <math.h>
#include <climits>
using namespace std;


string hammingCode(string binMsg)
{
	//initialize r "redundant bits count" with 0
	int r = 0;
	//loop till we get the satisfies the following equation : 2**r >= (m + r + 1)
	while(!(pow(2,r) >= (binMsg.length() + r + 1)))
	{
		r++;
		if(pow(2,r) - 1 >= INT_MAX) // in case it reached the overflow
			return ""; //means the hamming code failed
	}
	
	//initialize n to be the message bits + redundant bits
	int n = binMsg.length() + r;
	
	string temp = "",//initialize temp variable
	codedBinMsg = "";//initialize codedBinMsg which will hold the result
	for(int i = 1,m = 0,ri = 0;i <= n;i++)
	{
		/*
			in case that i is base of 2 i.e. i = 2,4,8,16,... then initialize it with zero
			otherwise initialize the temp variable with the message bit
		*/
		if(pow(2,ri) == i)
		{
			//increment the redundant bits counter
			ri++;
			//initialize redundant bits with zero
			temp += '0';
		}
		else{
			temp += binMsg[m];
			//increment the message bits counter
			m++;
		}
	}
	/*
		for each bit from the redundant bits , recalculate it with the message bits that corresponds to it by xoring them
	*/
	for(int ri = 0;ri < r;ri++)
	{
		//riIndex variable to indicate the current redundant bit
		int riIndex = int(pow(2,ri)) - 1;
		for(int i = 1;i <= n;i++)
		{
			//if the iterator i is the same as the current redundant bit then skip it
			if(int(pow(2,ri)) == i)
				continue;
			if((int(pow(2,ri)) & i) == int(pow(2,ri)))
			{
				int riValue = bitset<1>(temp[riIndex]).to_ulong() ^ bitset<1>(temp[i-1]).to_ulong(); 
				temp[int(pow(2,ri)) - 1] = riValue == 0 ? '0' : '1';
			}
		}
	}
	
	//assign the codedBinMsg with the temp
	codedBinMsg = temp;
	return codedBinMsg;
}

string hammingDecode(string codedBinMsg)
{
	int r = int(ceil(log2(codedBinMsg.length()+1))), //initialize r "redundant bits count"
	n = codedBinMsg.length(); //initialize n with the codedBinMsg length
	string bitToInvert = "", //bitToInvert will hold the index of the single bit error
	binMsg = "", //will hold the binMsg to be returned "after removing the hamming code"
	temp = codedBinMsg; //initialize temp variable with codedBinMsg
	/*
		for each bit from the redundant bits , recalculate it with the message bits that corresponds to it by xoring them
	*/
	for(int ri = 0;ri < r;ri++)
	{
		//riIndex variable to indicate the current redundant bit
		int riIndex = int(pow(2,ri)) - 1;
		for(int i = 1;i <= n;i++)
		{	
			//if the iterator i is the same as the current redundant bit then skip it
			if(int(pow(2,ri)) == i)
				continue;
			if((int(pow(2,ri)) & i) == int(pow(2,ri)))
			{
				int riValue = bitset<1>(temp[riIndex]).to_ulong() ^ bitset<1>(temp[i-1]).to_ulong(); 
				temp[int(pow(2,ri)) - 1] = riValue == 0 ? '0' : '1';
			} 
		}
		//xor the redundant bit with itself
		int riValue = bitset<1>(temp[riIndex]).to_ulong() ^ bitset<1>(temp[riIndex]).to_ulong(); 
		temp[riIndex] = riValue == 0 ? '0' : '1';
		//add the calculated values of the redundant bits to bitToInvert
		bitToInvert += temp[riIndex];
	}
	
	//convert string bitToInvert to invertIndex
	int invertIndex = 0;
	for(int i = 0;i < bitToInvert.length();i++)
		invertIndex += int(pow(2,i)*bitset<1>(bitToInvert[i]).to_ulong()); 

	//if invertIndex > 0 means there is a single bit should be inverted
	if( invertIndex > 0)
	{
		/*
			if the bits is '1' flip it to zero and vice versa
		*/
		if(temp[invertIndex - 1] == '1')
			temp[invertIndex - 1] = '0';
		else
			temp[invertIndex - 1] = '1';
	}
	
	//assign binMsg with the message bits only i.e. without the redundant bits
	for(int i = 1,ri = 0;i <= n;i++)
	{
		if(int(pow(2,ri)) == i)
		{
			ri++;	
		}
		else
		{
			binMsg += temp[i - 1];
		}
	}
	return binMsg;
}

