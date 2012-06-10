#ifndef VNOCMESSAGEHANDLER_HPP
#define VNOCMESSAGEHANDLER_HPP
#include <stdint.h>
#include <memory>

#include "AsioTcpConnection.hpp"
#include "SocketHandler.hpp"
#include "../../Message/MsgDef.h"
#include <ezlogger_headers.hpp>

class IMessageHandler;
class VnocProtocol;

class IVnocMessageProtocolHandler
{
public:
    virtual void SendVnocMessage(const CMessage *msg) = 0;
};

struct MessageContext
{
    IVnocMessageProtocolHandler *connection; 
    std::string userName;
};

template <typename ConnectionT>
class VnocMessageSocketHandler : public SocketHandler, IVnocMessageProtocolHandler
{
public:
    VnocMessageSocketHandler(ConnectionT *connection);
    ~VnocMessageSocketHandler();

    //start handler the message on the connection
    virtual void start();
    void setProtocol(VnocProtocol* protocol){protocol_= protocol;}
    virtual void SendVnocMessage(const CMessage *msg);

private:
    void ReadHeaderHandler(const asio::error_code& error, size_t bytes_transferred);
    void ReadBodyHandler(char *messageBuffer, const asio::error_code& error, size_t bytes_transferred);
    void SendHandler(char* buffer, const asio::error_code& error, size_t bytes_transferred);

    //post a read operation to wait for message header.
    void readHeader();
    int HandleRVCMessage(MSG_RVC *rvcMessage);

    const static size_t HEADER_LEN = 30;
    char headerData_[HEADER_LEN];
    ConnectionT *connection_;
    MessageContext *ctx_;
    VnocProtocol* protocol_;

};
class VnocMessageHandlerFactory: public SocketHandlerFactory
{
public:
    virtual SocketHandler* CreateHandler(AsioTcpConnection *connection)
    {
        auto handler = new VnocMessageSocketHandler<AsioTcpConnection>(connection);
        handler->setProtocol(protocol);
        return handler;
    }
        VnocProtocol *protocol;
};
#include "VnocMessageSocketHandler.inl"
#endif //VNOCMESSAGEHANDLER_HPP
