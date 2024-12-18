#ifdef _WINDOWS
#    include "pch.hpp"
#    include "SocketWinImpl.hpp"

namespace ATMA
{

    SocketWinImpl::SocketWinImpl(): Socket() {}

    SocketWinImpl::~SocketWinImpl() {}

    bool SocketWinImpl::connectSocket(const URL &l_addr, const unsigned short &l_port)
    {

        ZeroMemory(&m_hints, sizeof(m_hints));
        m_hints.ai_family = AF_UNSPEC;
        m_hints.ai_socktype = SOCK_STREAM;
        m_hints.ai_protocol = IPPROTO_TCP;

        bool addr =
            getaddrinfo(
                l_addr.getIP().c_str(), std::to_string(l_port).c_str(), &m_hints, &m_addrinfo
            )
            == 0;
        if(!addr)
        {
            ATMA_ENGINE_WARN("Socket failed to connect: {0}", WSAGetLastError());
            return false;
        }
        m_socket = socket(m_addrinfo->ai_family, m_addrinfo->ai_socktype, m_addrinfo->ai_protocol);

        if(m_socket == INVALID_SOCKET)
        {
            ATMA_ENGINE_WARN("Socket failed to connect: {0}", WSAGetLastError());
            return false;
        }

        auto connected =
            connect(m_socket, m_addrinfo->ai_addr, static_cast<int>(m_addrinfo->ai_addrlen));

        if(connected == SOCKET_ERROR)
        {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

        freeaddrinfo(m_addrinfo);

        if(m_socket == INVALID_SOCKET)
        {
            ATMA_ENGINE_WARN("Socket failed to connect to server");
            return false;
        }

        ATMA_ENGINE_TRACE("Created WinSock with handle {}", m_socket);
        return true;
    }

    bool SocketWinImpl::closeSocket()
    {
        shutdown(m_socket, SD_SEND);
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return true;
    }

    void SocketWinImpl::setBlocking(const bool &l_bool)
    {
        ATMA_ENGINE_TRACE("Setting WinSock with handle {} to blocking={}", m_socket, l_bool);
        if(!l_bool)
        {
            unsigned long ul = 1;
            ioctlsocket(m_socket, FIONBIO, (unsigned long *)&ul);
        }
        else
        {
            unsigned long ul = 0;
            ioctlsocket(m_socket, FIONBIO, (unsigned long *)&ul);
        }
  
    }

    bool SocketWinImpl::sendBytes(const std::span<unsigned char> &l_buffer, const size_t &l_size)
    {
        ATMA_ENGINE_TRACE("Sending bytes over WinSock with handle {}", m_socket);
        int result = send(m_socket, reinterpret_cast<char*>(l_buffer.data()), l_size, 0);
        if(result == SOCKET_ERROR)
        {
            ATMA_ENGINE_WARN("Socket failed to send bytes: {0}", WSAGetLastError());
            closesocket(m_socket);
            return false;
        }
        return true;
    }

    bool SocketWinImpl::receiveBytes(
        std::span<unsigned char> &l_buffer,
        const size_t &l_size,
        size_t &l_receivedBytes
    )
    {
        ATMA_ENGINE_TRACE("Receiving bytes over WinSock with handle {}", m_socket);
        int result = recv(m_socket, reinterpret_cast<char *>(l_buffer.data()), l_size, 0);
        if(result > 0)
        {
            l_receivedBytes = result;
        }
        else if(result == 0)
        {
            return false;
        }
        else
        {
            ATMA_ENGINE_WARN("Socket failed to received bytes: {0}", WSAGetLastError());
            return false;
        }
        return true;
    }

}

#else
#    error Windows implementation included in non-Windows target
#endif