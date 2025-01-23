#ifndef ATMA_SERVER
#    include "GameApp.hpp"



GameApp::GameApp() {}

GameApp::~GameApp() {}

void GameApp::setup(ATMA::ATMAContext &l_ctx)
{
    l_ctx.registerAttributeType<AttrConnection>(GameAttributeType(GameAttributeEnum::CONNECTION));
    l_ctx.addSystemType<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    auto netSys = l_ctx.getSystem<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::CONNECTION_STARTED), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_JOIN), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST), netSys); 
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_RESPONSE), netSys); 
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::STATE_CHANGE), netSys);
    active = true;

    auto winID = l_ctx.createWindow();
    m_win = l_ctx.getWindow(winID);
    m_renderer = l_ctx.getRenderer();
    m_win->setSize({1920, 1080});

    m_win->show();
    // note that the context is reset when set to a new window, so any memory associated with
    // it will cause an error. So best to set window before anything else
    m_renderer->setWindow(m_win);
    auto vertShaderID = l_ctx.registerResource("vertex", 1u, "shader/defaultVertex.shader");
    auto fragShaderID = l_ctx.registerResource("frag", 1u, "shader/defaultFrag.shader");
    auto fontID = l_ctx.registerResource("font", 0u, "res/defaultFont.png");
    std::shared_ptr<MainMenuState> state = std::make_shared<MainMenuState>(m_win, vertShaderID, fragShaderID, fontID);
    l_ctx.addState(GameStateType(GameStateEnum::MAINMENU), std::move(state));
    std::shared_ptr<LobbyState> lobby = std::make_shared<LobbyState>(m_win, vertShaderID, fragShaderID, fontID);
    l_ctx.addObjectEventListener(ATMA::ObjectEventType(ATMA::ObjectEvent::Network), lobby);
    l_ctx.addState(GameStateType(GameStateEnum::LOBBY), std::move(lobby));
    std::shared_ptr<PlayState> play = std::make_shared<PlayState>(m_win, vertShaderID, fragShaderID, fontID);
    l_ctx.addObjectEventListener(ATMA::ObjectEventType(ATMA::ObjectEvent::Network), play);
    l_ctx.addState(GameStateType(GameStateEnum::PLAYSTATE), std::move(play));
    m_renderer->toggleBlend(true);
}

void GameApp::update(ATMA::ATMAContext &l_ctx)
{
    if(m_win->shouldClose())
    {
        active = false;
    }

    m_win->poll();
    m_renderer->startScene(ATMA::GLCamera{
        {  0.f,   0.f},
        {360.f, 360.f}
    });
    l_ctx.update();
    m_renderer->finishScene();
    m_win->swapBuffers();
}

void GameApp::shutdown(ATMA::ATMAContext &l_ctx)
{
    ATMA_ENGINE_INFO("Shutting down game");
}

std::unique_ptr<ATMA::Game> ATMA::CreateGame()
{
    return std::unique_ptr<ATMA::Game>{new GameApp()};
}
#endif