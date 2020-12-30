//
// Generated file, do not edit! Created by nedtool 5.6 from MyMessage.msg.
//

#ifndef __MYMESSAGE_M_H
#define __MYMESSAGE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include <bitset>
typedef std::bitset<8> bits;
// }}

/**
 * Class generated from <tt>MyMessage.msg:27</tt> by nedtool.
 * <pre>
 * packet MyMessage
 * {
 *     \@customize(true);  // see the generated C++ header for more info
 *     int Seq_Num;
 *     int M_Type;
 *     int Peer;
 *     int dst;
 *     string M_Payload;
 *     bits mycheckbits;
 * }
 * </pre>
 *
 * MyMessage_Base is only useful if it gets subclassed, and MyMessage is derived from it.
 * The minimum code to be written for MyMessage is the following:
 *
 * <pre>
 * class MyMessage : public MyMessage_Base
 * {
 *   private:
 *     void copy(const MyMessage& other) { ... }

 *   public:
 *     MyMessage(const char *name=nullptr, short kind=0) : MyMessage_Base(name,kind) {}
 *     MyMessage(const MyMessage& other) : MyMessage_Base(other) {copy(other);}
 *     MyMessage& operator=(const MyMessage& other) {if (this==&other) return *this; MyMessage_Base::operator=(other); copy(other); return *this;}
 *     virtual MyMessage *dup() const override {return new MyMessage(*this);}
 *     // ADD CODE HERE to redefine and implement pure virtual functions from MyMessage_Base
 * };
 * </pre>
 *
 * The following should go into a .cc (.cpp) file:
 *
 * <pre>
 * Register_Class(MyMessage)
 * </pre>
 */
class MyMessage_Base : public ::omnetpp::cPacket
{
  protected:
    int Seq_Num;
    int M_Type;
    int Peer;
    int dst;
    ::omnetpp::opp_string M_Payload;
    bits mycheckbits;

  private:
    void copy(const MyMessage_Base& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MyMessage_Base&);
    // make constructors protected to avoid instantiation

    MyMessage_Base(const MyMessage_Base& other);
    // make assignment operator protected to force the user override it
    MyMessage_Base& operator=(const MyMessage_Base& other);

  public:
    MyMessage_Base(const char *name=nullptr, short kind=0);
    virtual ~MyMessage_Base();
    virtual MyMessage_Base *dup() const override {throw omnetpp::cRuntimeError("You forgot to manually add a dup() function to class MyMessage");}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSeq_Num() const;
    virtual void setSeq_Num(int Seq_Num);
    virtual int getM_Type() const;
    virtual void setM_Type(int M_Type);
    virtual int getPeer() const;
    virtual void setPeer(int Peer);
    virtual int getDst() const;
    virtual void setDst(int dst);
    virtual const char * getM_Payload() const;
    virtual void setM_Payload(const char * M_Payload);
    virtual bits& getMycheckbits();
    virtual const bits& getMycheckbits() const {return const_cast<MyMessage_Base*>(this)->getMycheckbits();}
    virtual void setMycheckbits(const bits& mycheckbits);
};


#endif // ifndef __MYMESSAGE_M_H

