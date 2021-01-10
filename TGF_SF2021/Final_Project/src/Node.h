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

#ifndef __FINAL_PROJECT_NODE_H_
#define __FINAL_PROJECT_NODE_H_

#include <omnetpp.h>
#include "MyMessage_m.h"
#include <fstream>
#include <stdio.h>
using namespace omnetpp;
#include <iostream>
#include <bitset>
#include <string>
#include <math.h>
#include <climits>
#include <random>
using namespace std;
/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  int peer = -1;
  protected:
  //go back N
  int nextFrameToSend;//receiver window pointer
  int ackExpected;  //sender buffer window start
  int frameExpected;//sender buffer window end
  int nBuffered;//number of unacknowledged frames
  int maxSequenceNumber;
  std::string *buffer;
  bool capped; //flag check if buffer is full
  float *timers;//array for storing message sent time (used with time out)
  bool started;//flag used with timeout
  float selfMsgDelay;//delay added when using schedule
  float selfTimeOutEventDelay;//delay added when using schedule for time out event
  float maxWaitTime;//timer for time out
  bool timedOut;//flag if time out event is created
  bool resetFlag;//flag if reset received from hub
  ifstream MyReadFile;
  //////
  //create files
  int msgNum;//total number of messages in the file
  int currentMsg;//current message in the file
  int nodeNumber;//number of current node
  std::string fileName;//name of the node's file
  ///////statistics
  int generatedFrames;//total number of generated frames
  int droppedFrames;//total number of dropped frames
  int retransmittedFrames;//total number of retransmittedframes
  int duplicatedFrames;//total number of duplicated frames
  ///////
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  //model channel noise
  void noiseModelling( MyMessage_Base * message);
  void finish();
  //called when reset is received from hub
  void reset();
  //to check if the received ack number is in the sender buffer
  bool between(int a, int b, int c);
  //used to send frames
  void send_data(int frameNum, int frameExpected);
  //increment in circular way
  int inc_circular(int x);
  ////files
  //check if there is no more messages in the file
  bool isFileFinished();
  //create file of messages to send
  void createFile();
  //read a message from the file to send it
  std::string readLine();
  //create random message (utility for create file method)
  std::string randomMsg();
  ///framing and hamming code
  string hammingCode(string binMsg);
  string hammingDecode(string codedBinMsg);
  string framingMsg(string Msg);
  string deframingMsg(string binMsg);
  int generateRandom(int lowerRange, int UpperRange);

};

#endif
