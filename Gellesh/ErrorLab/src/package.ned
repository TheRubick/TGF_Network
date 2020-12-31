package errorlab;

@license(LGPL);

network Network
{
    @display("bgb=326,159");
    submodules:
        sender: Sender {
            @display("p=138,38;i=block/source");
        }
        receiver: Receiver {
            @display("p=138,116;i=block/buffer2");
        }
    connections:
        sender.out --> receiver.in;
        receiver.out --> sender.in;
}
