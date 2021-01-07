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

package final_project;

//
// TODO auto-generated module
//
simple Node
{
    parameters:
        int modRate = default(7);
        int lossRate = default(6);
        int delayRate = default(6);
        int dupRate = default(6);
        double selfMsgDelay = default(2.0);
        double selfTimeOutEventDelay = default(1.0);
        double timeOut = default(10.0);
        double delay = default(0.0);
        int n = default(4);
    gates:
        input in;
        output out;
}
