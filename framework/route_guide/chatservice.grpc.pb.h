// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: chatservice.proto
// Original file comments:
// See README.txt for information and build instructions.
//
// Note: START and END tags are used in comments to define sections used in
// tutorials.  They are not part of the syntax for Protocol Buffers.
//
// To get an in-depth walkthrough of this file and the related examples, see:
// https://developers.google.com/protocol-buffers/docs/tutorials
//
// [START declaration]
#ifndef GRPC_chatservice_2eproto__INCLUDED
#define GRPC_chatservice_2eproto__INCLUDED

#include "chatservice.pb.h"

#include <grpc++/impl/codegen/async_stream.h>
#include <grpc++/impl/codegen/async_unary_call.h>
#include <grpc++/impl/codegen/proto_utils.h>
#include <grpc++/impl/codegen/rpc_method.h>
#include <grpc++/impl/codegen/service_type.h>
#include <grpc++/impl/codegen/status.h>
#include <grpc++/impl/codegen/stub_options.h>
#include <grpc++/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class RpcService;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace chatserver {

class commandService GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status commandRequest(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::chatserver::Stats* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Stats>> AsynccommandRequest(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Stats>>(AsynccommandRequestRaw(context, request, cq));
    }
    virtual ::grpc::Status Join(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>> AsyncJoin(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>>(AsyncJoinRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>> chat(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>>(chatRaw(context));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>> Asyncchat(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>>(AsyncchatRaw(context, cq, tag));
    }
    virtual ::grpc::Status User(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>> AsyncUser(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>>(AsyncUserRaw(context, request, cq));
    }
    virtual ::grpc::Status List(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>> AsyncList(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>>(AsyncListRaw(context, request, cq));
    }
    virtual ::grpc::Status Leave(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>> AsyncLeave(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>>(AsyncLeaveRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Stats>* AsynccommandRequestRaw(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>* AsyncJoinRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>* chatRaw(::grpc::ClientContext* context) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::chatserver::Stats, ::chatserver::Stats>* AsyncchatRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>* AsyncUserRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>* AsyncListRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::chatserver::Requests>* AsyncLeaveRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status commandRequest(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::chatserver::Stats* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Stats>> AsynccommandRequest(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Stats>>(AsynccommandRequestRaw(context, request, cq));
    }
    ::grpc::Status Join(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>> AsyncJoin(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>>(AsyncJoinRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderWriter< ::chatserver::Stats, ::chatserver::Stats>> chat(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriter< ::chatserver::Stats, ::chatserver::Stats>>(chatRaw(context));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::chatserver::Stats, ::chatserver::Stats>> Asyncchat(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::chatserver::Stats, ::chatserver::Stats>>(AsyncchatRaw(context, cq, tag));
    }
    ::grpc::Status User(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>> AsyncUser(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>>(AsyncUserRaw(context, request, cq));
    }
    ::grpc::Status List(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>> AsyncList(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>>(AsyncListRaw(context, request, cq));
    }
    ::grpc::Status Leave(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::chatserver::Requests* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>> AsyncLeave(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>>(AsyncLeaveRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::chatserver::Stats>* AsynccommandRequestRaw(::grpc::ClientContext* context, const ::chatserver::Commands& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>* AsyncJoinRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* chatRaw(::grpc::ClientContext* context) GRPC_OVERRIDE;
    ::grpc::ClientAsyncReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* AsyncchatRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>* AsyncUserRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>* AsyncListRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::chatserver::Requests>* AsyncLeaveRaw(::grpc::ClientContext* context, const ::chatserver::Requests& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_commandRequest_;
    const ::grpc::RpcMethod rpcmethod_Join_;
    const ::grpc::RpcMethod rpcmethod_chat_;
    const ::grpc::RpcMethod rpcmethod_User_;
    const ::grpc::RpcMethod rpcmethod_List_;
    const ::grpc::RpcMethod rpcmethod_Leave_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status commandRequest(::grpc::ServerContext* context, const ::chatserver::Commands* request, ::chatserver::Stats* response);
    virtual ::grpc::Status Join(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response);
    virtual ::grpc::Status chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* stream);
    virtual ::grpc::Status User(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response);
    virtual ::grpc::Status List(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response);
    virtual ::grpc::Status Leave(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_commandRequest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_commandRequest() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_commandRequest() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commandRequest(::grpc::ServerContext* context, const ::chatserver::Commands* request, ::chatserver::Stats* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestcommandRequest(::grpc::ServerContext* context, ::chatserver::Commands* request, ::grpc::ServerAsyncResponseWriter< ::chatserver::Stats>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Join : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Join() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_Join() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Join(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestJoin(::grpc::ServerContext* context, ::chatserver::Requests* request, ::grpc::ServerAsyncResponseWriter< ::chatserver::Requests>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_chat : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_chat() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_chat() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* stream) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requestchat(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(2, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_User : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_User() {
      ::grpc::Service::MarkMethodAsync(3);
    }
    ~WithAsyncMethod_User() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status User(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestUser(::grpc::ServerContext* context, ::chatserver::Requests* request, ::grpc::ServerAsyncResponseWriter< ::chatserver::Requests>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(3, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_List : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_List() {
      ::grpc::Service::MarkMethodAsync(4);
    }
    ~WithAsyncMethod_List() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status List(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestList(::grpc::ServerContext* context, ::chatserver::Requests* request, ::grpc::ServerAsyncResponseWriter< ::chatserver::Requests>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(4, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Leave : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Leave() {
      ::grpc::Service::MarkMethodAsync(5);
    }
    ~WithAsyncMethod_Leave() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Leave(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestLeave(::grpc::ServerContext* context, ::chatserver::Requests* request, ::grpc::ServerAsyncResponseWriter< ::chatserver::Requests>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(5, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_commandRequest<WithAsyncMethod_Join<WithAsyncMethod_chat<WithAsyncMethod_User<WithAsyncMethod_List<WithAsyncMethod_Leave<Service > > > > > > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_commandRequest : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_commandRequest() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_commandRequest() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status commandRequest(::grpc::ServerContext* context, const ::chatserver::Commands* request, ::chatserver::Stats* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Join : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Join() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_Join() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Join(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_chat : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_chat() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_chat() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status chat(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::chatserver::Stats, ::chatserver::Stats>* stream) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_User : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_User() {
      ::grpc::Service::MarkMethodGeneric(3);
    }
    ~WithGenericMethod_User() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status User(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_List : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_List() {
      ::grpc::Service::MarkMethodGeneric(4);
    }
    ~WithGenericMethod_List() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status List(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Leave : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Leave() {
      ::grpc::Service::MarkMethodGeneric(5);
    }
    ~WithGenericMethod_Leave() GRPC_OVERRIDE {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Leave(::grpc::ServerContext* context, const ::chatserver::Requests* request, ::chatserver::Requests* response) GRPC_FINAL GRPC_OVERRIDE {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
};

}  // namespace chatserver


#endif  // GRPC_chatservice_2eproto__INCLUDED
