#include "ResourceTestSuite.hpp"
#include <gtest/gtest.h>

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
}