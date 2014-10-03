#pragma once
class Message {
  public:
    virtual ~Message() {};
    virtual void execute() = 0;

};


// Ye olde dummy..
class DummyMessage : public Message {
  public:
    ~DummyMessage() { }
    void execute() { }
};
