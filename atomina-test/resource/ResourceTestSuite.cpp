#include "ResourceTestSuite.hpp"
#include <gtest/gtest.h>
//#define SCALE  32767.0

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
    static unsigned short count = 0;
    if(count > 1)
        return 1;
    auto &ctx = ATMA::ATMAContext::getContext();
    unsigned int *id = (unsigned int *) userData;
    auto res = ctx.loadResource<ATMA::AudioWave>(*id);
    unsigned short *buffer = (unsigned short *) outputBuffer;
 
    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
 
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

TYPED_TEST_SUITE(ResourceFixture, ResourceTypes);

/**
 * ATMA Context should keep all resources
 * and assign them unique identifiers
 */
TYPED_TEST(ResourceFixture, CanRegisterResource)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    EXPECT_TRUE(ctx.hasResource(id));
}

/**
 * Resources should be able to be loaded on call
 * using their id
 */
TYPED_TEST(ResourceFixture, CanLoadResource)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    auto res = this->loadResource(id);
    EXPECT_NE(res, nullptr);
}

/**
 * Resource should no longer be kept in context cache
 * when unloaded
 */
TYPED_TEST(ResourceFixture, CanUnloadResource)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    auto res = this->loadResource(id);
    ctx.unloadResource(id);
    EXPECT_FALSE(ctx.hasLoadedResource(id));
}

/**
 * Loaded resources should always be the same if they have the
 * same id
 */
TYPED_TEST(ResourceFixture, reloadingALoadedResourceReturnsResource)
{

    auto &ctx = this->ctx;
    ATMA_ENGINE_INFO("loaded context");
    auto id = this->registerResource(0u);
    ATMA_ENGINE_INFO("resource registered");
    auto res = this->loadResource(id);
    ATMA_ENGINE_INFO("resource loaded");
    auto res2 = this->loadResource(id);
    ATMA_ENGINE_INFO("resource loaded a second time");
    EXPECT_EQ(res, res2);
}

/**
 * Resource should no longer be registered in context
 * once removed
 */
TYPED_TEST(ResourceFixture, CanRemoveResource)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    ctx.removeResource(id);
    EXPECT_FALSE(ctx.hasResource(id));
}

/**
 * Resources that have been removed should throw an exception
 * when trying to get them
 */
TYPED_TEST(ResourceFixture, GetRemovedResource)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    auto res = this->loadResource(id);
    ctx.removeResource(id);
    EXPECT_THROW(this->loadResource(id), ATMA::ValueNotFoundException);
}

/**
 * Removing a resource from the context should also
 * remove it from loaded cache in the context
 */
TYPED_TEST(ResourceFixture, RemovingResourceUnloadsIt)
{
    auto &ctx = this->ctx;
    auto id = this->registerResource(0u);
    auto res = this->loadResource(id);
    ctx.removeResource(id);
    EXPECT_FALSE(ctx.hasLoadedResource(id));
}

TEST_F(UnTypedResourceFixture, LoadWaveform)
{
    auto &ctx = this->ctx;
    auto id = ctx.registerResource("testWave", 0u, std::optional<std::string>{"res/flick.wav"});
    auto res = ctx.loadResource<ATMA::AudioWave>(id);
    EXPECT_NE(res,nullptr);
    RtAudio dac;
    std::vector<unsigned int> deviceIds = dac.getDeviceIds();
    if ( deviceIds.size() < 1 ) 
    {
        ATMA_ENGINE_ERROR("no devices found");
        ADD_FAILURE();
    }
 
    RtAudio::StreamParameters params;
    params.deviceId = dac.getDefaultOutputDevice();
    params.nChannels = res->m_wave.m_channels;
    params.firstChannel = 0;
    unsigned int bufferFrames = res->m_wave.m_data.size();
    unsigned int data = id;
    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;
    if(dac.openStream(&params, nullptr, RTAUDIO_SINT16, res->m_wave.m_sampleRate, &bufferFrames, &saw, (void *)&data))
    {
        ATMA_ENGINE_ERROR("failed to open stream");
        ADD_FAILURE();
    }

    // Stream is open ... now start it.
    if ( dac.startStream() ) 
    {
        ATMA_ENGINE_ERROR("failed to start stream: {}",  dac.getErrorText());
        ADD_FAILURE();
    }
 
    while(dac.isStreamRunning()){}
 
    ATMA_ENGINE_INFO("stream finished playing");
 

}