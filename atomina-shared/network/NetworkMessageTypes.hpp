#pragma once
#include "pch.hpp"

namespace ATMA
{
    /**
     * defines basic message value types
     */
    enum class NetworkMessageValueEnum
    {
        NONE = 0,
        CHAR,
        UNSIGNEDCHAR,
        SHORT,
        UNSIGNEDSHORT,
        INT,
        UNSIGNEDINT,
        FLOAT,
        LONG,
        UNSIGNEDLONG,
        DOUBLE,
        LONGLONG,
        UNSIGNEDLONGLONG,
        STRING
    };

    /**
     * wrapper class for MessageValue Enum that allows for implicit
     * coversion to unsigned char
     */
    class NetworkMessageValueType
    {
    public:
        // normal constructor
        NetworkMessageValueType(const NetworkMessageValueEnum &l_type): m_type(l_type) {}

        // conversion constructor
        NetworkMessageValueType(const unsigned char &l_char): m_type(static_cast<NetworkMessageValueEnum>(l_char)) {}

        // conversion operator
        operator unsigned char()
        {
            return static_cast<unsigned char>(m_type);
        }
    private:
        const NetworkMessageValueEnum m_type;
    };

    /**
     * defines all network message types
     */
    enum class NetworkMessageEnum
    {
        NONE = 0,
        INVALID,
        CONNECTION_STARTED,
        DISCONNECT,
        PORT_REQUEST,
        PORT_JOIN,
        PORT_RESPONSE,
        STATE_CHANGE,
        COUNT
    };

    /**
     * wrapper class for Network Message Enum that allows for implicit
     * coversion to unsigned int
     */
    class NetworkMessageType
    {
    public:
        // normal constructor
        NetworkMessageType(const NetworkMessageEnum &l_type): m_type(l_type) {}

        // conversion constructor
        NetworkMessageType(const unsigned int &l_int): m_type(static_cast<NetworkMessageEnum>(l_int)) {}

        // conversion operator
        operator unsigned int()
        {
            return static_cast<unsigned int>(m_type);
        }
    private:
        const NetworkMessageEnum m_type;
    };
}
