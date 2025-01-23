
#include "NetworkMessageTestSuite.hpp"

using namespace std::string_literals;

/**
 * Network message should serialize
 */
TEST_F(NetworkMessageFixture, CanSerializeNetworkMessage)
{
    ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST)};
    auto output = ATMA::NetworkSerde::serialize(nm);
    EXPECT_TRUE(output.size() > 0);
}

/**
 * Network message should correctly serialize and deserialize to the same value
 */
TEST_F(NetworkMessageFixture, CanDeserializeSerializedNetworkMessage)
{
    ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST)};
    auto bytes = ATMA::NetworkSerde::serialize(nm);
    size_t empty;
    auto output = ATMA::NetworkSerde::deserialize(bytes, empty);
    EXPECT_EQ(output.type(), nm.type());
}

TYPED_TEST_SUITE(TypedNetworkMessageFixture, NetworkMessageTypes);

TYPED_TEST(TypedNetworkMessageFixture, CanDeserializeSerializedNetworkMessageWithValues)
{
    ATMA::Props p{};
    ATMA::NetworkMessageValueEnum mvt = ATMA::NetworkMessageValueEnum::CHAR;
 
    std::pair<unsigned char, std::any> first, second;

    if (std::is_same_v<TypeParam, char>)
    {
        mvt = ATMA::NetworkMessageValueEnum::CHAR;
    }
    else if(std::is_same_v<TypeParam, unsigned char>)
    {
        mvt = ATMA::NetworkMessageValueEnum::UNSIGNEDCHAR;
    }
    else if (std::is_same_v<TypeParam, short>)
    {
        mvt = ATMA::NetworkMessageValueEnum::SHORT;
    }
    else if(std::is_same_v<TypeParam, unsigned short>)
    {
        mvt = ATMA::NetworkMessageValueEnum::UNSIGNEDSHORT;
    }
    else if(std::is_same_v<TypeParam, int>)
    {
        mvt = ATMA::NetworkMessageValueEnum::INT;
    }
    else if(std::is_same_v<TypeParam, unsigned int>)
    {
        mvt = ATMA::NetworkMessageValueEnum::UNSIGNEDINT;
    }
    else if(std::is_same_v<TypeParam, float>)
    {
        mvt = ATMA::NetworkMessageValueEnum::FLOAT;
    }
    else if(std::is_same_v<TypeParam, long>)
    {
        mvt = ATMA::NetworkMessageValueEnum::LONG;
    }
    else if(std::is_same_v<TypeParam, unsigned long>)
    {
        mvt = ATMA::NetworkMessageValueEnum::UNSIGNEDLONG;
    }
    else if(std::is_same_v<TypeParam, double>)
    {
        mvt = ATMA::NetworkMessageValueEnum::DOUBLE;
    }
    else if(std::is_same_v<TypeParam, long long>)
    {
        mvt = ATMA::NetworkMessageValueEnum::LONGLONG;
    }
    else if(std::is_same_v<TypeParam, unsigned long long>)
    {
        mvt = ATMA::NetworkMessageValueEnum::UNSIGNEDLONGLONG;
    }
    else if(std::is_same_v<TypeParam, std::string>)
    {
        mvt = ATMA::NetworkMessageValueEnum::STRING;
    }
    first = this->getSomeProperty(ATMA::NetworkMessageValueType(mvt));
    second = this->getSomeProperty(ATMA::NetworkMessageValueType(mvt));
    p["first"] = first;
    p["second"] = second;
    ATMA::NetworkMessage nm{ATMA::NetworkMessageType(ATMA::NetworkMessageEnum::PORT_REQUEST), p};
    auto bytes = ATMA::NetworkSerde::serialize(nm);
    size_t cursor;
    auto output = ATMA::NetworkSerde::deserialize(bytes, cursor);
    EXPECT_EQ(output.type(), nm.type());
    EXPECT_EQ(std::numeric_limits<TypeParam>::max(), output.values().getAs<TypeParam>("first"));
    EXPECT_EQ(std::numeric_limits<TypeParam>::max(), output.values().getAs<TypeParam>("second"));
}
