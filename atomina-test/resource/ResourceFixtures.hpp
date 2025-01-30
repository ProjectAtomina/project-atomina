#pragma once
#include "AtominaTest.hpp"

/**
 * Fixture for testing Resource and helping with adding them
 * to the ATMA context
 */
template<class T>
class ResourceFixture: public ::testing::Test
{
public:
    ATMA::ATMAContext &ctx = ATMA::ATMAContext::getContext();
protected:
    /**
     * Adds resource to the ATMA contain with type of the template
     * class
     * @param l_resourceType id of the resource type
     * @param l_filename full qualified path to the resource
     * @returns unique identifier of the resource
     */
    unsigned int registerResource(
        const unsigned int &l_resourceType,
        const std::optional<std::string> &l_filename = std::nullopt
    )
    {
        return ctx.registerResource("", l_resourceType, l_filename);
    }

    /**
     * loads resource from file into memory
     * @param l_resourceID unique identifier of the resource
     * @returns pointer to loaded resource
     */
    std::shared_ptr<T> loadResource(const unsigned int &l_resourceID)
    {
        return ctx.loadResource<T>(l_resourceID);
    }

    /**
     * Cleans up context after each test
     */
    void TearDown() override
    {
        
        ctx.purge();
    }
};

/**
 * Fixture for type specific implementations of resources
 */
class UnTypedResourceFixture: public ::testing::Test
{
public:
    ATMA::ATMAContext &ctx = ATMA::ATMAContext::getContext();

protected:

    /**
     * Cleans up the context after each test
     */
    void TearDown() override
    {
        ctx.purge();
    }
};