#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <google/protobuf/any.pb.h>

class EventInterface {
protected:
public:
    EventInterface() = default;
    virtual ~EventInterface() = default;

    virtual bool validateMessage(const google::protobuf::Any& any) = 0;
    virtual void handleReceivedMessage(const google::protobuf::Any& any) = 0;
};

template <typename T, std::enable_if_t<!std::is_pointer_v<T>, bool> = true> class MessageReceiveEvent : public EventInterface {
protected:
    T callback{};

public:
    MessageReceiveEvent() = default;
    explicit MessageReceiveEvent(T callback) { this->callback = callback; }
    ~MessageReceiveEvent() override = default;

    bool validateMessage(const google::protobuf::Any& any) override {
        bool messageValidated{false};
        if (this->callback) {
            messageValidated = this->callback->validate(any);
        }
        return messageValidated;
    }

    void handleReceivedMessage(const google::protobuf::Any& any) override { this->callback->run(any); }
};