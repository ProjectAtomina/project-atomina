#include "pch.hpp"
#include "ATMAContext.hpp"
#include "util/AtominaException.hpp"
#include "util/Log.hpp"
#include <memory>

namespace ATMA
{

    void ATMAContext::objectUpdated(
        const unsigned int &l_objectID,
        const std::bitset<ATConst::OBJECT_BIT_SIZE> &l_bits
    )
    {
        for(auto &system: m_systems)
        {
            if(system.second->match(l_bits) && !system.second->hasObject(l_objectID))
            {
                system.second->addObject(l_objectID);
            }
            else if(!system.second->match(l_bits) && system.second->hasObject(l_objectID))
            {
                system.second->removeObject(l_objectID);
            }
        }
    }

    void ATMAContext::systemUpdated(const unsigned int &l_systemID)
    {
        for(auto &obj: m_objects)
        {
            if(m_systems[l_systemID]->match(obj.second.first))
            {
                m_systems[l_systemID]->addObject(obj.first);
            }
        }
    }

    void ATMAContext::pushWindowEvent(const WindowEvent &l_e)
    {
        ATMA_ENGINE_INFO("Pushed window event of type {0:d}", int(l_e.m_event.type));
        for(auto &state: m_states)
        {
            state.second->handleEvent(l_e);
        }
    }

    unsigned int ATMAContext::createObject()
    {
        std::lock_guard<std::mutex> lock{m_mtx};
        auto id = m_lastObjectID++;
        ATMA_ENGINE_INFO("Created Object ID {0:d}", id);
        return id;
    }

    unsigned int ATMAContext::createObject(const std::bitset<ATConst::OBJECT_BIT_SIZE> &l_bits)
    {
        std::lock_guard<std::mutex> lock{m_mtx};
        auto id = m_lastObjectID++;
        ATMA_ENGINE_INFO("Created Object ID {0:d}", id);
        for(unsigned int i = 0; i < ATConst::OBJECT_BIT_SIZE; i++)
        {
            if(!l_bits.test(i))
                continue;
            addAttribute(id, i);
        }
        objectUpdated(id, l_bits);
        return id;
    }

    void ATMAContext::addAttribute(const unsigned int &l_objectID, const unsigned int &l_attrType)
    {
        if(l_objectID >= m_lastObjectID)
            throw ValueNotFoundException(
                "Object id: " + std::to_string(l_objectID) + " was not found"
            );

        auto attrItr = m_attrFactory.find(l_attrType);
        if(attrItr == m_attrFactory.end())
            throw ValueNotFoundException(
                "Attribute type: " + std::to_string(l_attrType)
                + " has not been registered in factory"
            );

        auto itr = m_objects.find(l_objectID);
        if(itr == m_objects.end())
        {
            // create new attribute container
            std::bitset<ATConst::OBJECT_BIT_SIZE> bits{};
            bits.set(l_attrType);
            std::shared_ptr<AttrBase> attr = m_attrFactory[l_attrType]();
            std::unordered_map<AttrTypeID, std::shared_ptr<AttrBase>> attrMap{};
            attrMap[l_attrType] = attr;
            std::pair<
                std::bitset<ATConst::OBJECT_BIT_SIZE>,
                std::unordered_map<AttrTypeID, std::shared_ptr<AttrBase>>>
                pair{bits, attrMap};
            ObjectAttributes attrs{pair};
            m_objects[l_objectID] = attrs;
            ATMA_ENGINE_INFO(
                "Added attribute type {0:d} to object id {1:d}", l_attrType, l_objectID
            );
            objectUpdated(l_objectID, bits);
        }
        else
        {
            // update attribute container
            itr->second.first.set(l_attrType);
            itr->second.second[l_attrType] = m_attrFactory[l_attrType]();
            ATMA_ENGINE_INFO(
                "Added attribute type {0:d} to object id {1:d}", l_attrType, l_objectID
            );
            objectUpdated(l_objectID, itr->second.first);
        }
    }

    void
    ATMAContext::removeAttribute(const unsigned int &l_objectID, const unsigned int &l_attrType)
    {
        auto itr = m_objects.find(l_objectID);
        if(itr == m_objects.end())
        {
            throw ValueNotFoundException(
                "object id: " + std::to_string(l_objectID)
                + " does not contain any attributes or does not exist"
            );
        }
        else
        {
            auto innerItr = itr->second.second.find(l_attrType);
            if(innerItr == itr->second.second.end())
            {
                throw ValueNotFoundException(
                    "object id: " + std::to_string(l_objectID)
                    + " does not contain attribute type: " + std::to_string(l_attrType)
                );
            }
            else
            {
                itr->second.first.reset(innerItr->first);
                itr->second.second.erase(innerItr);
                ATMA_ENGINE_INFO(
                    "Removed attribute type {0:d} to object id {1:d}", l_attrType, l_objectID
                );
                objectUpdated(l_objectID, itr->second.first);
            }
        }
    }

    bool ATMAContext::hasAttribute(const unsigned int &l_objectID, const unsigned int &l_attrType)
    {
        auto itr = m_objects.find(l_objectID);
        if(itr == m_objects.end())
        {
            return false;
        }
        else
        {
            return itr->second.first.test(l_attrType);
        }
    }

    void ATMAContext::enableSystem(const unsigned int &l_systemID)
    {
        if(m_systems.count(l_systemID) == 0)
            throw ValueNotFoundException(
                "System type: " + std::to_string(l_systemID)
                + " has not been registered in ATMA Context"
            );
        m_systems[l_systemID]->m_enabled = true;
        ATMA_ENGINE_INFO("Enabled system type: {0:d}", l_systemID);
    }

    void ATMAContext::disableSystem(const unsigned int &l_systemID)
    {
        if(m_systems.count(l_systemID) == 0)
            throw ValueNotFoundException(
                "System type: " + std::to_string(l_systemID)
                + " has not been registered in ATMA Context"
            );
        m_systems[l_systemID]->m_enabled = false;
        ATMA_ENGINE_INFO("Disabled system type: {0:d}", l_systemID);
    }

    bool ATMAContext::hasSystem(const unsigned int &l_systemID)
    {
        return m_systems.count(l_systemID) > 0;
    }

    void ATMAContext::removeSystem(const unsigned int &l_systemID)
    {
        auto itr = m_systems.find(l_systemID);
        if(itr == m_systems.end())
        {
            throw ValueNotFoundException(
                "System type: " + std::to_string(l_systemID) + " does not exist in ATMA context"
            );
        }
        else
        {
            ATMA_ENGINE_INFO("Removed System type: {0:d}", itr->second->getType());
            m_systems.erase(itr);
        }
    }

    void ATMAContext::addState(const unsigned int &l_stateType, std::unique_ptr<BaseState> l_state)
    {
        bool initState = m_states.empty();
        if(m_states.count(l_stateType) == 0)
        {
            m_states[l_stateType] = std::move(l_state);
            if(initState)
                switchToState(l_stateType);
        }
        else
            throw RegistrationException(
                "State of type:" + std::to_string(l_stateType)
                + " has already been registered to the context"
            );
        ATMA_ENGINE_INFO("Registered State type: {0:d}", l_stateType);
    }

    void ATMAContext::removeState(const unsigned int &l_stateType)
    {
        auto itr = m_states.find(l_stateType);
        if(itr == m_states.end())
        {
            throw ValueNotFoundException(
                "State of type: " + std::to_string(l_stateType)
                + " has not been registered to the context"
            );
        }
        else
        {
            m_states.erase(itr);
            ATMA_ENGINE_INFO("Removed State type: {0:d}", l_stateType);
        }
    }

    void ATMAContext::switchToState(const unsigned int &l_stateType)
    {
        if(m_states.count(l_stateType) == 0)
        {
            throw ValueNotFoundException(
                "State of type: " + std::to_string(l_stateType)
                + " has not been registered to the context"
            );
        }
        if(m_states.count(m_currentStateID) != 0)
        {
            ATMA_ENGINE_INFO("Deactivating State: {0:d}", m_currentStateID);
            m_states[m_currentStateID]->deactivate();
        }
        ATMA_ENGINE_INFO(
            "Switching from state Type: {0:d} to state Type: {1:d}", m_currentStateID, l_stateType
        );
        m_currentStateID = l_stateType;
        ATMA_ENGINE_INFO("Activating State: {0:d}", m_currentStateID);
        m_states[m_currentStateID]->activate();
    }

    bool ATMAContext::hasState(const unsigned int &l_stateType)
    {
        return m_states.count(l_stateType) > 0;
    }

    unsigned int ATMAContext::registerResource(
        const unsigned int &l_resourceType,
        const std::optional<std::string> &l_filename
    )
    {
        auto id = m_lastResourceId++;
        m_resources[id] = std::make_pair(l_resourceType, l_filename);
        return id;
    }

    bool ATMAContext::hasResource(const unsigned int &l_resourceID)
    {
        return m_resources.count(l_resourceID) > 0;
    }

    void ATMAContext::unloadResource(const unsigned int &l_resourceID)
    {
        auto itr = m_loadedResources.find(l_resourceID);
        if(itr == m_loadedResources.end())
        {
            throw ValueNotFoundException(
                "Resource of id: " + std::to_string(l_resourceID)
                + " has not been loaded to the context"
            );
        }
        else
        {
            m_loadedResources.erase(itr);
            ATMA_ENGINE_INFO("Unloaded Resource id: {0:d}", l_resourceID);
        }
    }

    bool ATMAContext::hasLoadedResource(const unsigned int &l_resourceID)
    {
        return m_loadedResources.count(l_resourceID) > 0;
    }

    void ATMAContext::removeResource(const unsigned int &l_resourceID)
    {
        auto itr = m_resources.find(l_resourceID);
        if(itr == m_resources.end())
        {
            throw ValueNotFoundException(
                "Resource of id: " + std::to_string(l_resourceID)
                + " has not been registered to the context"
            );
        }
        else
        {
            try
            {
                unloadResource(l_resourceID);
            }
            catch(ValueNotFoundException e)
            {
                ATMA_ENGINE_INFO("Resource being removed: {0}", e.what());
            }
            m_resources.erase(itr);
            ATMA_ENGINE_INFO("removed Resource id: {0:d}", l_resourceID);
        }
    }

    unsigned int ATMAContext::createWindow()
    {
        m_windows[m_lastWindowID] = std::make_shared<Window>(Window{});
        ATMA_ENGINE_INFO("Created window with id: {0:d}", m_lastWindowID);
        unsigned int id = m_lastWindowID++;
        return id;
    }

    std::shared_ptr<Window> ATMAContext::getWindow(const unsigned int &l_windowID)
    {
        auto itr = m_windows.find(l_windowID);
        if(itr == m_windows.end())
        {
            throw ValueNotFoundException(
                "Window of id: " + std::to_string(l_windowID) + " does not exist"
            );
        }
        else
        {
            return itr->second;
        }
    }

    void ATMAContext::deleteWindow(const unsigned int &l_windowID)
    {
        auto itr = m_windows.find(l_windowID);
        if(itr == m_windows.end())
        {
            throw ValueNotFoundException(
                "Window of id: " + std::to_string(l_windowID) + " does not exist"
            );
        }
        else
        {
            m_windows.erase(itr);
            ATMA_ENGINE_INFO("Deleted Window with id: {0:d}", l_windowID);
        }
    }

    void ATMAContext::dispatchObjectEvent(const ObjectEventContext &l_e)
    {
        for(auto &listenerVec: m_listeners)
        {
            if(l_e.m_objectEventType == listenerVec.first)
            {
                for(auto &listener: listenerVec.second)
                {
                    if(listener->isEnabled()){
                        ATMA_ENGINE_INFO("Listener for event type {0:d} notified", l_e.m_objectEventType);
                        listener->notify(l_e);
                    }
                }
            }
        }
    }

    void ATMAContext::addObjectEventListener(
        const ObjectEventID &l_id,
        std::shared_ptr<ObjectEventListener> l_listener
    )
    {
        auto itr = m_listeners.find(l_id);
        if(itr == m_listeners.end())
        {
            std::vector<std::shared_ptr<ObjectEventListener>> newVec{l_listener};
            m_listeners[l_id] = newVec;
        }
        else
        {
            m_listeners[l_id].emplace_back(l_listener);
        }
    }

    void ATMAContext::update()
    {

        for(auto &win: m_windows)
        {
            sf::Event e{};
            while(win.second->poll(e))
            {
                pushWindowEvent(WindowEvent{e});
            }
        }

        for(auto &state: m_states)
        {
            state.second->update(sf::Time{});
        }

        for(auto &sys: m_systems)
        {
            if(sys.second->m_enabled)
                sys.second->update(0.0f);
        }
    }

    void ATMAContext::purgeObjects()
    {
        m_objects.clear();
        std::bitset<ATConst::OBJECT_BIT_SIZE> emptybits{};
        for(auto &system: m_systems)
        {
            system.second->purge();
        }
        m_lastObjectID = 0;
    }

    void ATMAContext::purgeSystems()
    {
        m_systems.clear();
    }

    void ATMAContext::purgeStates()
    {
        m_states.clear();
        m_currentStateID = 0;
    }

    void ATMAContext::purgeResources()
    {
        m_resources.clear();
        m_loadedResources.clear();
    }

    void ATMAContext::purgeWindows()
    {
        m_windows.clear();
    }

    void ATMAContext::purgeListeners()
    {
        m_listeners.clear();
    }

    void ATMAContext::purge()
    {
        ATMA_ENGINE_INFO("Purging ATMA Context");
        m_objects.clear();
        m_attrFactory.clear();
        m_systems.clear();
        m_states.clear();
        m_resources.clear();
        m_loadedResources.clear();
        m_windows.clear();
        m_listeners.clear();
        m_lastObjectID = 0;
        m_lastWindowID = 0;
        m_currentStateID = 0;
    }

}