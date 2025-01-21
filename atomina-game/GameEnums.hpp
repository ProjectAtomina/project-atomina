#pragma once
#include <atomina.hpp>

/**
 * defines all possible events within the game
 */
enum class GameEventEnum
{
    COUNT = static_cast<unsigned int>(ATMA::ObjectEvent::COUNT),
    CONNECT,
    DISCONNECT,
    SYNC,
};

/**
 * defines all possible states within the shared library
 */
enum class GameStateEnum
{
    COUNT = static_cast<unsigned int>(ATMA::State::COUNT),
    MAINMENU,
    LOBBY,
    PLAYSTATE,
};

/**
* defines all possible systems within the game
*/
enum class GameSystemEnum
{
    COUNT = static_cast<unsigned int>(ATMA::System::COUNT),
    CONNECTION,
};

/**
* defines all possible systems within the game
*/
enum class GameAttributeEnum
{
    COUNT = static_cast<unsigned int>(ATMA::Attribute::COUNT),
    CONNECTION,
};

enum class GameNetMessageEnum
{
    COUNT = static_cast<unsigned int>(ATMA::NetworkMessageEnum::COUNT),
    PLAYER_CHOICE

};

/**
 * wrapper class for State Enum that allows for implicit
 * coversion to unsigned int
 */
class GameEventType
{
public:
    // normal constructor
    GameEventType(const GameEventEnum &l_type): m_type(l_type) {}

    // conversion constructor
    GameEventType(const unsigned int &l_int): m_type(static_cast<GameEventEnum>(l_int)) {}

    // conversion operator
    operator unsigned int()
    {
        return static_cast<unsigned int>(m_type);
    }
private:
    const GameEventEnum m_type;
};



/**
 * wrapper class for State Enum that allows for implicit
 * coversion to unsigned int
 */
class GameStateType
{
public:
    // normal constructor
    GameStateType(const GameStateEnum &l_type): m_type(l_type) {}

    // conversion constructor
    GameStateType(const unsigned int &l_int): m_type(static_cast<GameStateEnum>(l_int)) {}

    // conversion operator
    operator unsigned int()
    {
        return static_cast<unsigned int>(m_type);
    }
private:
    const GameStateEnum m_type;
};

/**
 * wrapper class for State Enum that allows for implicit
 * coversion to unsigned int
 */
class GameSystemType
{
public:
    // normal constructor
    GameSystemType(const GameSystemEnum &l_type): m_type(l_type) {}

    // conversion constructor
    GameSystemType(const unsigned int &l_int): m_type(static_cast<GameSystemEnum>(l_int)) {}

    // conversion operator
    operator unsigned int()
    {
        return static_cast<unsigned int>(m_type);
    }
private:
    const GameSystemEnum m_type;
};

/**
 * wrapper class for State Enum that allows for implicit
 * coversion to unsigned int
 */
class GameAttributeType
{
public:
    // normal constructor
    GameAttributeType(const GameAttributeEnum &l_type): m_type(l_type) {}

    // conversion constructor
    GameAttributeType(const unsigned int &l_int): m_type(static_cast<GameAttributeEnum>(l_int)) {}

    // conversion operator
    operator unsigned int()
    {
        return static_cast<unsigned int>(m_type);
    }
private:
    const GameAttributeEnum m_type;
};