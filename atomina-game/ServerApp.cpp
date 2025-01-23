#ifdef ATMA_SERVER
#    include "ServerApp.hpp"
#    include "OAS/AttrConnection.hpp"
#    include "OAS/SysConnection.hpp"

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
    l_ctx.netManager.startHosting(4734);
}

void ServerApp::update(ATMA::ATMAContext &l_ctx)
{
    auto &ctx = ATMA::ATMAContext::getContext();
    auto sysConn = ctx.getSystem<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    for(auto connId : sysConn->m_conns)
    {
        if(!connId.first.has_value())
        {
            ATMA_ENGINE_WARN("found connection without id handle {}", connId.second);
            continue;
        }
        auto connAttr = ctx.getAttribute<AttrConnection>(connId.second, GameAttributeType(GameAttributeEnum::CONNECTION));
        std::vector<ATMA::NetworkMessage> msgQueue;
        {
            std::lock_guard<std::mutex> lock{connAttr->m_msgMutex};
            msgQueue = connAttr->m_msgs;
            connAttr->m_msgs.clear();
        }
        for(auto &msg : msgQueue)
        {
            switch(static_cast<ATMA::NetworkMessageEnum>(msg.type()))
            {
            case ATMA::NetworkMessageEnum::PORT_REQUEST:
                {
                    int i = 0;
                    std::map<std::string, std::any> msgEntries{};
                    for(;i < m_ports.size(); i++)
                    {
                        msgEntries["port"+std::to_string(i)] = std::pair<unsigned char, std::any>{
                             ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::SHORT), (short)-1  //occupied
                        };
                        std::pair<unsigned char, std::any> pair{
                            ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::SHORT), (short)m_ports.size()  //empty
                        };
                        ATMA::Props p{{{"port", pair}}};
                        ATMA::NetworkMessage broadcast{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_JOIN), p};
                        ctx.netManager.sendMessage(broadcast, m_ports[i]);
                    }
                    m_ports.emplace_back(connId.first);
                    msgEntries["port"+std::to_string(i)] = std::pair<unsigned char, std::any>{
                        ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::SHORT), (short)1  //port to be filled
                    };
                    ++i;
                    for(;i < MAX_PORTS; i++)
                    {
                        msgEntries["port"+std::to_string(i)] = std::pair<unsigned char, std::any>{
                            ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::SHORT), (short)0  //empty
                        };
                    }              
                    ATMA::Props p{msgEntries};
                    ATMA_ENGINE_INFO("User has requested port data");
                    ATMA::NetworkMessage resp{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_RESPONSE), p};
                    ctx.netManager.sendMessage(
                        resp, connId.first
                    );
                }
                break;
            case ATMA::NetworkMessageEnum::STATE_CHANGE:
                {
                    ATMA::Props p{
                        {{"state",
                          std::pair<unsigned char, std::any>{
                              ATMA::NetworkMessageValueType(ATMA::NetworkMessageValueEnum::UNSIGNEDINT),
                              msg.values().getAs<unsigned int>("state")
                          }}}
                    };
                    ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::STATE_CHANGE), p};
                    ctx.netManager.broadcastMessage(nm);
                }
                break;
            default:
                ATMA_ENGINE_INFO("skipped network message type: {}", msg.type());
                break;
            }
        }
    }
    

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