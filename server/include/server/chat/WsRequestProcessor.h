#pragma once

#include <server/chat/WsData.h>
#include <server/utils/utils.h>
#include <server/chat/MessageHandlerService.h>
#include <server/chat/DrogonRoomService.h>

class WsRequestProcessor {
public:
    static drogon::Task<> handleIncomingMessage(const drogon::WebSocketConnectionPtr& conn, const std::string& bytes) {
        try {
            chat::Envelope env;
            if(!env.ParseFromString(bytes)) {
                sendEnvelope(conn, makeGenericErrorEnvelope("Malformed protobuf message"));
                co_return;
            }
            DrogonRoomService room_service{conn};
            sendEnvelope(conn, co_await MessageHandlerService::processMessage(conn->getContext<WsData>(), env, room_service));
        } catch(const std::exception& e) {
            LOG_ERROR << "Critical error in WsRequestProcessor::handleIncomingMessage: " << e.what();
            sendEnvelope(conn, makeGenericErrorEnvelope("Critical server error during message handling."));
            co_return;
        }
    }
};
