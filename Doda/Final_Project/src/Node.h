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

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  int peer = -1;
  protected:
  int nextFrameToSend;
  int ackExpected;
  int frameExpected;
  int nBuffered;
  int maxSequenceNumber;
  std::string *buffer;
  bool capped;
  float *timers;
  bool started;
  float maxWaitTime;
  bool timedOut;
  bool resetFlag;
  int msgNum;
  int currentMsg;
  int nodeNumber;
  std::string fileName;
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
  void noiseModelling( MyMessage_Base * message);
  void finish();
  void reset();
  bool between(int a, int b, int c);
  void send_data(int frameNum, int frameExpected);
  int inc_circular(int x);
  ////files
  bool isFileFinished();
  void createFile();
  std::string readLine();
  std::string randomMsg();

};

#endif
