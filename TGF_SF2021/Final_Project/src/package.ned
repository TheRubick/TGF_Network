package final_project;

@license(LGPL);

import ned.IdealChannel;

network Star
{
    parameters:
        int n = default(5);
    submodules:
        nodes[n]: Node;
        hubNode: Hub ;
        
    connections: 
       for i=0..sizeof(nodes)-1
       {
           nodes[i].out --> IdealChannel --> hubNode.ins++;
           nodes[i].in <-- IdealChannel <-- hubNode.outs++;
       }  
}