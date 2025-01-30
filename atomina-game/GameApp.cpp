#ifndef ATMA_SERVER
#    include "GameApp.hpp"

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
    static unsigned short count = 0;
    if(count > 1) //needs to play twice because second time gets cut off
        return 1;
    auto &ctx = ATMA::ATMAContext::getContext();
    unsigned int *id = (unsigned int *) userData;
    auto res = ctx.loadResource<ATMA::AudioWave>(*id);
    unsigned short *buffer = (unsigned short *) outputBuffer;
 
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
 
    ATMA_ENGINE_TRACE("buffer size is {} and wave form size is {} ", nBufferFrames, res->m_wave.m_data.size());
    // Write interleaved audio data.
    unsigned short *data = (unsigned short *)res->m_wave.m_data.data();
    for (unsigned int i = 0; i<nBufferFrames; i++ ) {
        for(unsigned short j = 0; j < res->m_wave.m_channels; j++)
        {
            buffer[i*res->m_wave.m_channels + j] = data[i*res->m_wave.m_channels + j];
        }
    }
    count++;
    return 0;
}

GameApp::GameApp() {}

GameApp::~GameApp() {}

void GameApp::setup(ATMA::ATMAContext &l_ctx)
{
    l_ctx.registerAttributeType<AttrConnection>(GameAttributeType(GameAttributeEnum::CONNECTION));
    l_ctx.addSystemType<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    l_ctx.registerAttributeType<AttrTimer>(GameAttributeType(GameAttributeEnum::TIMER));
    l_ctx.addSystemType<SysTime>(GameSystemType(GameSystemEnum::TIME));
    auto netSys = l_ctx.getSystem<SysConnection>(GameSystemType(GameSystemEnum::CONNECTION));
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::CONNECTION_STARTED), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_JOIN), netSys);
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST), netSys); 
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_RESPONSE), netSys); 
    l_ctx.netManager.addMessageListener(ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::STATE_CHANGE), netSys);
    l_ctx.netManager.addMessageListener(GameNetMessageType(GameNetMessageEnum::PLAYER_WIN), netSys);
    l_ctx.netManager.addMessageListener(GameNetMessageType(GameNetMessageEnum::PLAYER_TIED), netSys);
    l_ctx.netManager.addMessageListener(GameNetMessageType(GameNetMessageEnum::PLAYER_LOSE), netSys);
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
    l_ctx.addObjectEventListener(GameEventType(GameEventEnum::TIMER_COMPLETE), play);
    l_ctx.addState(GameStateType(GameStateEnum::PLAYSTATE), std::move(play));
    m_renderer->toggleBlend(true);
    auto id = l_ctx.registerResource("testWave", 0u, std::optional<std::string>{"res/test.wav"});
    auto res = l_ctx.loadResource<ATMA::AudioWave>(id);
    RtAudio dac;
    std::vector<unsigned int> deviceIds = dac.getDeviceIds();
    if ( deviceIds.size() < 1 ) 
    {
        ATMA_ENGINE_ERROR("no devices found");
        throw ATMA::AtominaException{"no audio devices found"};
    }
 
    RtAudio::StreamParameters params;
    params.deviceId = dac.getDefaultOutputDevice();
    params.nChannels = res->m_wave.m_channels;
    params.firstChannel = 0;
    unsigned int bufferFrames = res->m_wave.m_data.size()/res->m_wave.m_channels/res->m_wave.m_sampleSize;
    unsigned int data = id;
    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;
    if(dac.openStream(&params, nullptr, RTAUDIO_SINT16, res->m_wave.m_sampleRate, &bufferFrames, &saw, (void *)&data))
    {
        ATMA_ENGINE_ERROR("failed to open stream");
        throw ATMA::AtominaException{"unable to open audio stream"};
    }

    // Stream is open ... now start it.
    if ( dac.startStream() ) 
    {
        ATMA_ENGINE_ERROR("failed to start stream: {}",  dac.getErrorText());
        throw ATMA::AtominaException{"unable to start stream"};
    }
 
    while(dac.isStreamRunning()){}
 
    ATMA_ENGINE_INFO("stream finished playing");
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