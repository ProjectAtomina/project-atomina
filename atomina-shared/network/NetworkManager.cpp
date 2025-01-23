#include "pch.hpp"
#include "NetworkManager.hpp"

namespace ATMA
{

    void NetworkConnection::run()
    {

        ATMA_ENGINE_INFO(
            "Starting connection thread for id {} with handle {}",
            m_id.value_or(std::numeric_limits<unsigned int>::max()),
            m_conn
        );

        m_conn->setBlocking(false);
        do
        {
            size_t recvBytes;
            unsigned char buf[NETWORKMESSAGEBUFFERSIZE] = {0};
            std::span<unsigned char> bufSpan{buf};

            if(auto res = m_conn->receiveBytes(bufSpan, ATMA::NETWORKMESSAGEBUFFERSIZE, recvBytes); res == 0)
            {
                continue;
            }
            else if(res < 0)
            {
                ATMA_ENGINE_WARN("socket handle {} received error", m_conn);
                break;
            }

            std::vector<unsigned char> wholeMessage{};
            size_t totalBytes = recvBytes;
            unsigned short messageSize;

            for(int i = 0; i < recvBytes; i++)
            {
                wholeMessage.emplace_back(bufSpan[i]);
            }
            std::copy(
                bufSpan.begin(),
                bufSpan.begin() + sizeof(unsigned short),
                reinterpret_cast<unsigned char *>(&messageSize)
            );
            while(totalBytes < messageSize && recvBytes > 0)
            {
                m_conn->receiveBytes(bufSpan, NETWORKMESSAGEBUFFERSIZE, recvBytes);
                totalBytes += recvBytes;
                for(int i = 0; i < recvBytes; i++)
                {
                    wholeMessage.emplace_back(bufSpan[i]);
                }
            }
            ATMA_ENGINE_TRACE(
                "Got Network message of {} bytes on handle {} id {}",
                totalBytes,
                m_conn,
                m_id.value_or(std::numeric_limits<unsigned int>::max())
            );
            // its possible that multiple messages can be received at once from
            // a connection, so we need to use the same byte stream to deserialize
            // multiple messages
            size_t offset = 0;
            while(offset < wholeMessage.size())
            {
                size_t cursor;
                NetworkMessage msg = NetworkSerde::deserialize(
                    std::vector<unsigned char>{wholeMessage.begin() + offset, wholeMessage.end()}, cursor
                );
                offset += cursor;
                if(auto itr = m_subscribers->find(msg.type()); itr != m_subscribers->end())
                {
                    ATMA_ENGINE_TRACE("Dispatching message type {} to {} subscribers", msg.type(), itr->second.size());
                    NetworkMessageListener::dispatch(m_id, msg, itr->second);
                }
                //if message becomes invalid, we have no idea where the start of the message is so we need
                //to abandon it
                if(msg.type() == NetworkMessageType(NetworkMessageEnum::INVALID))
                    break;
            }

        } while(*m_connected);

        ATMA_ENGINE_INFO(
            "Terminating connection thread for id {} m_connected={}",
            m_id.value_or(std::numeric_limits<unsigned int>::max()),
            *m_connected
        );
        m_termSignal.release();
    }

    void NetworkListener::run()
    {
        ATMA_ENGINE_INFO("Starting listening thread");
        while(*m_listening)
        {
            if(auto sock = m_listener->acceptConnection(); sock != nullptr)
            {
                const ConnId id = (*m_lastId)++;
                ATMA_ENGINE_INFO("Accepting connection with id {}", id);
                sock->setBlocking(false);
                std::optional<const ConnId> idOpt{id};
                std::shared_ptr<bool> connected = std::make_shared<bool>(true);
                std::shared_ptr<NetworkConnection> conn =
                    std::make_shared<NetworkConnection>(idOpt, sock, connected, m_subscribers);
                (*m_connections)[id] = std::make_shared<Conn>(std::make_pair(connected, conn));
                NetworkMessage msg{NetworkMessageType(NetworkMessageEnum::CONNECTION_STARTED)};
                if(auto itr = m_subscribers->find(msg.type()); itr != m_subscribers->end())
                    NetworkMessageListener::dispatch(idOpt, msg, itr->second);
            }
        }
        ATMA_ENGINE_INFO("Terminating listening thread");
    }

    NetworkManager::NetworkManager() {}

    NetworkManager::~NetworkManager()
    {
        ATMA_ENGINE_INFO("Shutting down Network Manager");
        *m_listening = false;
        *m_connected = false;
        m_listener = nullptr;
        m_listenerSocket = nullptr;

        m_conn = nullptr;
        m_connSocket = nullptr;
        for(auto &c: *m_connections)
        {
            *(c.second->first) = false;
        }
        m_connections->clear();
    }

    void NetworkManager::startHosting(const unsigned int &l_port)
    {
        m_listenerSocket = SocketListener::makeSocketListener(l_port);
        if(*m_listening)
        {
            ATMA_ENGINE_WARN("Already listening");
            return;
        }
        *m_listening = true;
        ATMA_ENGINE_INFO("Creating Network Listener");
        m_listener = std::make_unique<NetworkListener>(
            m_listenerSocket, m_listening, m_connections, m_lastConnId, m_subscribers
        );
    }

    void NetworkManager::broadcastMessage(const NetworkMessage &l_msg)
    {
        std::vector<unsigned char> msg = NetworkSerde::serialize(l_msg);
        for(auto &conn: *m_connections)
        {
            ATMA_ENGINE_TRACE("Send message type {} to conn id {}", l_msg.type(), conn.first);
            conn.second->second->send(l_msg);
        }
    }

    void NetworkManager::stopHosting()
    {
        ATMA_ENGINE_INFO("notifying listening thread to stop");
        *m_listening = false;
        m_listener = nullptr;
        m_listenerSocket = nullptr;
    }

    void NetworkManager::startConnection(const URL &l_url, const unsigned short &l_port)
    {
        if(*m_connected)
        {
            ATMA_ENGINE_WARN("Already connected");
            return;
        }
        m_connSocket = Socket::makeSocket(l_url, l_port);
        m_connSocket->setBlocking(false);
        *m_connected = true;
        ATMA_ENGINE_INFO("Creating Network Connection");
        m_conn = std::make_unique<NetworkConnection>(std::nullopt, m_connSocket, m_connected, m_subscribers);
        NetworkMessage msg{NetworkMessageType(NetworkMessageEnum::CONNECTION_STARTED)};
        if(auto itr = m_subscribers->find(msg.type()); itr != m_subscribers->end())
            NetworkMessageListener::dispatch(std::nullopt, msg, itr->second);
    }

    void NetworkManager::sendMessage(const NetworkMessage &l_msg, const std::optional<const ConnId> &l_id)
    {
        std::vector<unsigned char> msg = NetworkSerde::serialize(l_msg);
        if(l_id.has_value())
        {
            ATMA_ENGINE_TRACE("Send message type {} to conn id {}", l_msg.type(), l_id.value());
            (*m_connections)[l_id.value()]->second->send(l_msg);
        }
        else
        {
            m_connSocket->sendBytes(msg, msg.size());
        }
    }

    void NetworkManager::stopConnection(const std::optional<const ConnId> &l_id)
    {
        ATMA_ENGINE_INFO(
            "notifying connection thread to stop {}", l_id.value_or(std::numeric_limits<const ConnId>::max())
        );
        if(l_id.has_value())
        {
            if(auto c = m_connections->find(l_id.value()); c != m_connections->end())
            {
                // ATMA_ENGINE_TRACE("removing connection with id {}", l_id.value());
                *(c->second->first) = false;
                m_connections->erase(l_id.value());
            }
        }
        else
        {
            *m_connected = false;
            m_conn = nullptr;
            m_connSocket = nullptr;
        }
    }

    void
    NetworkManager::addMessageListener(const unsigned int &l_type, std::shared_ptr<NetworkMessageListener> l_listener)
    {
        auto itr = m_subscribers->find(l_type);
        if(itr == m_subscribers->end())
        {
            std::vector<std::shared_ptr<NetworkMessageListener>> newVec{l_listener};
            (*m_subscribers)[l_type] = newVec;
            ATMA_ENGINE_INFO("Network Event Listener for Network Event Type: {0:d} has been added", l_type);
        }
        else
        {
            (*m_subscribers)[l_type].emplace_back(l_listener);
            ATMA_ENGINE_INFO("Network Event Listener for Network Event Type: {0:d} has been added", l_type);
        }
    }

    void NetworkManager::purgeConnections()
    {
        ATMA_ENGINE_INFO("purging Network Manager");
        *m_connected = false;

        m_conn = nullptr;
        m_connSocket = nullptr;
        for(auto &c: *m_connections)
        {
            *(c.second->first) = false;
        }
        m_connections->clear();
        *m_lastConnId = 0;
    }

    void NetworkManager::purgeListeners()
    {
        m_subscribers->clear();
    }

}