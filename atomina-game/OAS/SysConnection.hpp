#pragma once
#include <atomina.hpp>
#include "../GameEnums.hpp"
#include "AttrConnection.hpp"

class SysConnection: public ATMA::SysBase, public ATMA::NetworkMessageListener
{
public:
    // default constructor
    SysConnection(): SysBase(GameSystemType(GameSystemEnum::CONNECTION), "Connection") 
    {
        auto &ctx = ATMA::ATMAContext::getContext();
        m_req.push_back(std::bitset<ATConst::OBJECT_BIT_SIZE>{});
        m_req[0].set(GameAttributeType(GameAttributeEnum::CONNECTION));
    }

    // deconstructor
    virtual ~SysConnection() {}

    /**
    * update internal members on an engine tick basis
    * @param l_dt the time since last update
    */
    virtual void update(const long long &l_dt) override
    {
        auto &ctx = ATMA::ATMAContext::getContext();
        for(auto &idPair : m_conns)
        {
            auto connAttr = ctx.getAttribute<AttrConnection>(idPair.second, GameAttributeType(GameAttributeEnum::CONNECTION));
            std::lock_guard<std::mutex> lock{connAttr->m_msgMutex};
            for(auto &msg: connAttr->m_msgs)
            {
                ATMA_ENGINE_TRACE("Sending out object network message type {}", msg.type());
                ATMA::Props p = msg.values();
                p["connId"] = idPair.first;
                p["msgType"] = msg.type();
                ATMA::ObjectEventContext e{ATMA::ObjectEventType(ATMA::ObjectEvent::Network), p};
                ctx.dispatchObjectEvent(e);
            }
            connAttr->m_msgs.clear();
        }
    }

    /**
    * event callback function where the system will changes
    * its behaviour or perform an action based on the event details
    * @param l_e details of the event
    */
    virtual void notify(const ATMA::ObjectEventContext &l_e) override
    {}

    virtual void notify(const std::optional<const unsigned int> &l_id, const ATMA::NetworkMessage &l_e) override 
    {
        auto &ctx = ATMA::ATMAContext::getContext();
        ATMA_ENGINE_TRACE("Network System handling message with type {}", l_e.type());
        switch(l_e.type())
        {
        case static_cast<unsigned int>(ATMA::NetworkMessageEnum::CONNECTION_STARTED):
            {
                auto connObjId = ctx.createObject();
                ctx.addAttribute(connObjId, GameAttributeType(GameAttributeEnum::CONNECTION));
                auto connAttr = ctx.getAttribute<AttrConnection>(connObjId, GameAttributeType(GameAttributeEnum::CONNECTION));
                connAttr->m_id=l_id;
                m_conns[l_id] = connObjId;
                break;
            }
        default:
            if(auto itr = m_conns.find(l_id); itr != m_conns.end())
            {
                auto connAttr = ctx.getAttribute<AttrConnection>(itr->second, GameAttributeType(GameAttributeEnum::CONNECTION));
                std::lock_guard<std::mutex> lock{connAttr->m_msgMutex};
                connAttr->m_msgs.emplace_back(l_e);
            }
        }

    }

    std::unordered_map<std::optional<unsigned int>, unsigned int> m_conns{};
};