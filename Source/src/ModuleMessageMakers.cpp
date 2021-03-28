#include "ModuleMessageMakers.hpp"
#include "ModuleGlobals.hpp"

namespace Module {

ServiceModule::Message MessageMakers::makeRequestMessage(google::protobuf::Any* any, uint32_t transactionCode) {
    auto* requestHeader = new ServiceModule::Header{};
    requestHeader->set_senderidentifier(Module::Globals::moduleIdentifier);
    requestHeader->set_operationcode(ServiceModule::OperationCode::ModuleRequest);
    requestHeader->set_transactioncode(transactionCode);
    auto* moduleRequest = new ServiceModule::Request{};
    moduleRequest->set_allocated_request(any);
    ServiceModule::Message messageToSend{};
    messageToSend.set_allocated_header(requestHeader);
    messageToSend.set_allocated_request(moduleRequest);
    return messageToSend;
}

ServiceModule::Message MessageMakers::makeSubscriptionRequestMessage(std::string subscribedType, uint32_t transactionCode) {
    auto* requestHeader = new ServiceModule::Header{};
    requestHeader->set_senderidentifier(Module::Globals::moduleIdentifier);
    requestHeader->set_operationcode(ServiceModule::OperationCode::ModuleRequest);
    requestHeader->set_transactioncode(transactionCode);
    auto* subscriptionRequest = new ServiceModule::SubscriptionRequest{};
    subscriptionRequest->set_subscribedtype(subscribedType);
    ServiceModule::Message messageToSend{};
    messageToSend.set_allocated_header(requestHeader);
    messageToSend.set_allocated_subscriptionrequest(subscriptionRequest);
    return messageToSend;
}

} // namespace Module