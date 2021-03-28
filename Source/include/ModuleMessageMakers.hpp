#pragma once
#include "ServiceModule.pb.h"
#include <cstdint>

namespace Module {

class MessageMakers {
public:
    static ServiceModule::Message makeRequestMessage(google::protobuf::Any*, uint32_t transactionCode);
    static ServiceModule::Message makeSubscriptionRequestMessage(std::string, uint32_t transactionCode);
};

} // namespace Module