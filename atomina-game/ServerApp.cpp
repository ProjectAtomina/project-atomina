#ifdef ATMA_SERVER
#    include "ServerApp.hpp"
#    include "OAS/AttrConnection.hpp"
#    include "OAS/SysConnection.hpp"
#    include "state/ServerState.hpp"

ServerApp::ServerApp() {}

ServerApp::~ServerApp() {}

void ServerApp::setup(ATMA::ATMAContext &l_ctx)
{
    active = true;
    ATMA_ENGINE_INFO("Running Server");
    l_ctx.registerAttributeType<AttrConnection>(GameAttributeType(GameAttributeEnum::CONNECTION));
    l_ctx.addSystemType<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    auto netSys = l_ctx.getSystem<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::CONNECTION_STARTED), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::STATE_CHANGE), netSys);
    std::shared_ptr<ServerState> server = std::make_shared<ServerState>();
    l_ctx.addObjectEventListener(ATMA::ObjectEventType(ATMA::ObjectEvent::Network), server);
    l_ctx.addState(GameStateType(GameStateEnum::PLAYSTATE), server);
    l_ctx.netManager.startHosting(4734);
}

void ServerApp::update(ATMA::ATMAContext &l_ctx)
{
    l_ctx.update();
}

void ServerApp::shutdown(ATMA::ATMAContext &l_ctx)
{
    l_ctx.netManager.stopHosting();
    ATMA_ENGINE_INFO("Shutting down game");
}

std::unique_ptr<ATMA::Server> ATMA::CreateServer()
{
#    ifdef ATMA_OPENGL
#        error Cannot create server with OpenGL
#    endif
    return std::unique_ptr<ATMA::Server>{new ServerApp()};
}
#endif