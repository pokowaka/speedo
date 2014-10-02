#pragma once
class Message {
  public:
    virtual ~Message() = 0;
    virtual void execute() = 0;

};


// Ye olde dummy..
class DummyMessage : public Message {
  public:
    ~DummyMessage() { }
    void execute() { }
};
