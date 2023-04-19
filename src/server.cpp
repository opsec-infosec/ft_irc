/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:10:24 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 16:08:26 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

namespace {
    volatile std::sig_atomic_t gSignalStatus;

    void signalHandler( int signal ) {
        if ( signal )
            gSignalStatus = signal;
    }
}

using namespace irc;

// @brief Server Constructor, default port -1 with empty password
Server::Server( ) : m_host("0.0.0.0"), m_port( -1 ) {
    m_sData = new ServerData( m_clients, m_channels  );
    m_sData->m_startUpTime = libft::getCurTimeString();
    m_sData->m_password = std::string();

    m_clientCommand = new commandWrapper( *m_sData );

    m_log.save( "Server: Creating Instance host=" + m_host + " port=" + libft::to_string( m_port ) + " password=<BLANK> ", this );
    m_status = OFFLINE;
    sigInit();
}

// @brief Server Constructor
// @param[in] port Port Number for Server to listen on, int ports, vald ranges are >0 and <=65535
// @param[in] password Password to conncet to Server, std::string password
Server::Server( const std::string& host, const int& port, const std::string& password ) :  m_host( host ), m_port( port ) {
    m_sData = new ServerData( m_clients, m_channels );
    m_sData->m_startUpTime = libft::getCurTimeString();
    m_sData->m_password = password;

    m_clientCommand = new commandWrapper( *m_sData );

    m_log.save( "Server: Creating Instance host=" + host + " port=" + libft::to_string( port ) + " password=" + password, this );
    m_status = OFFLINE;
    sigInit();

}

// @brief Server Destructor
Server::~Server() {
    if ( BOT )
        delete m_marv;

    delete m_clientCommand;
    delete m_sData;
    this->stop();
    m_log.save( "Server: Terminated" );
}

// Server Mehods

// @brief Starts the irc server listener and binds to port
// @throws std::exception::runtime if error
// @exceptsafe basic
void Server::init( void ) {
    struct sockaddr_in address;

    std::memset( &address, 0, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( m_port );
    int addressLen = sizeof( address );

    if ( ( m_socket = socket( address.sin_family, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
        int err = errno;
        m_log.save( "Server: socket failure " + libft::to_string( err ), this );
        throw std::runtime_error( "Server: socket error" );
    }

    int optval = 1;
    if ( setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t)sizeof( optval ) ) < 0 ) {
        int err = errno;
        m_log.save( "Server: socket options error " + libft::to_string( err ), this );
        close( m_socket );
        throw std::runtime_error( "Server: socket options error" );
    }

    if ( setsockopt( m_socket, SOL_SOCKET, SO_REUSEPORT, &optval, (socklen_t)sizeof( optval ) ) < 0 ) {
        int err = errno;
        m_log.save( "Server: socket options error " + libft::to_string( err ), this );
        close( m_socket );
        throw std::runtime_error( "Server: socket options error" );
    }

    if ( fcntl( m_socket, F_SETFL, fcntl( m_socket, F_GETFL) | O_NONBLOCK ) < 0 ) {
        int err = errno;
        m_log.save( "Server: file ctl error " + libft::to_string( err ), this );
        close( m_socket );
        throw std::runtime_error( "Server: file ctl error");
    }

    if ( bind( m_socket, (struct sockaddr*)&address, addressLen ) < 0 ) {
        int err = errno;
        m_log.save( "Server: bind error " + libft::to_string( err ), this );
        close( m_socket );
        throw std::runtime_error("Server: bind error");
    }

    if ( listen( m_socket, std::numeric_limits<int>().max() ) < 0 ) {
        int err = errno;
        m_log.save( "Server: listen error " + libft::to_string( err ), this );
        close( m_socket );
        throw std::runtime_error( "Server: listen error" );
    }

    m_status = ONLINE;
    m_log.save( "Server: Listening on address=" + m_host + " port=" + libft::to_string( m_port ) );
}

// @brief Starts the irc server polling
// @throws std::exception::runtime if error
// @exceptsafe basic
void Server::start( void ) {
    if ( !m_status )
        throw std::runtime_error( "Server: Offline, must init first" );

    setPFD( m_socket );

    if ( BOT )
        m_marv = new Marvin( "Marvin", "127.0.0.1", m_port );

    while ( isOnline() ) {
        if ( poll( m_pFD.data(), m_pFD.size(), -1 ) < 0 ) {
            int err = errno;
            if ( err != EINTR ) {
                m_log.save( "Server: poll error " + libft::to_string( err ), this );
                throw std::runtime_error( "Server: poll error" );
            }
        }

        for ( size_t i = 0; i < m_pFD.size(); i++ ) {
            // Something in the OUT Queue
            if ( m_pFD[i].revents & POLLOUT && m_pFD[i].fd != m_socket ) {
                for ( std::deque<std::string>::iterator it = m_clients[i - 1]->getMsgQS().begin(); it != m_clients[i - 1]->getMsgQS().end(); ++it ) {
                    #ifdef __APPLE__
                        send( m_pFD[i].fd, it->c_str(), it->size(), SO_NOSIGPIPE );
                    #else
                        send( m_pFD[i].fd, it->c_str(), it->size(), MSG_NOSIGNAL );
                    #endif
                    m_log.save( "Client " + libft::to_string( m_clients[i - 1]->getFD() ) + ": sent " + libft::to_string( it->size() ) + " bytes => msg: " + it->substr( 0, it->size() - 2 ) );

                    if ( !m_clients[i - 1]->getMsgQS().empty() )
                        m_clients[i - 1]->getMsgQS().pop_front();

                    if ( BOT ) {
                        RuntimeData rtd = RuntimeData( *m_sData );
                        m_marv->talk( &rtd );
                    }
                }
            }

            // Clean up clients that are marked for deletion
            if ( m_pFD[i].fd != m_socket &&  m_clients[i - 1]->isMarkedForDeletion() ) {
                removeClient( m_pFD[i].fd );
                break;
            }

            // Something is in the IN Queue
            if ( m_pFD[i].revents & POLLIN ) {
                // We have a new connection, create a new socket for comms
                if ( m_pFD[i].fd == m_socket ) {
                    int fd = -1;
                    int addressLen = 0;
                    struct sockaddr_in address;
                    std::memset( &address, 0, sizeof( address ) );

                    if ( ( fd = accept( m_socket, (sockaddr *)&address, (socklen_t *)&addressLen ) ) < 0 ) {
                        int err = errno;
                        m_log.save( "Server: client connection error " + libft::to_string( err ), this );
                        throw std::runtime_error( "Server: client connection error");
                    }

                    m_log.save( "Client " + libft::to_string( fd ) + ": connected from IP: " + libft::to_string( inet_ntoa( address.sin_addr ) ) );
                    m_clients.push_back( new Clients( fd, inet_ntoa( address.sin_addr ) ) );
                    setPFD( fd );
                }

                // We are receiving a raw message on the client socket
                if ( m_pFD[i].fd != m_socket ) {
                    int bytesRec = m_clients[i - 1]->receive();

                    if ( bytesRec <= 0 ) {
                        (m_clients[i - 1])->markForDeletion( m_pFD[i].fd );
                        break ;
                    }

                    m_log.save( "Client " + libft::to_string( m_clients[i - 1]->getFD() ) + ": received " + libft::to_string( bytesRec ) + " bytes <= msg: " + printMsgQ( m_clients[i - 1]->getMsgQR() ) );
                    m_clientCommand->cmdExecute( m_clients[i - 1] );

                }
            }
        }

        // Clean up Channels that are empty
        if ( m_channels.size() )
            removeChannel();
    }
}

// Public Methods

// @brief Stops the irc server
// @exceptsafe no-throw
void Server::stop( void ) {
    if ( m_status ) {
        m_status = OFFLINE;
        m_log.save( "Server: Stopped socked closed" );
        close( m_socket );

        for ( std::vector<Clients *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it ) // Remove all clients
            delete ( *it.base() );

        for ( std::vector<Channels *>::iterator it = m_channels.begin(); it != m_channels.end(); ++it ) // Remove all channels
            delete ( *it.base() );

        m_channels.erase( m_channels.begin(), m_channels.end() );
        m_clients.erase( m_clients.begin(), m_clients.end() );
        m_pFD.erase( m_pFD.begin(), m_pFD.end() );
    }
}

// @brief Gets the irc server client connections
// @returns[out] const std::vector<Clients *> &
// @exceptsafe no-throw
std::vector<Clients *>& Server::getClients( void ) {
    return m_clients;
}

// @brief Set the poll fd in the struct pollfd vector
// @params[in] fd File Descriptor of socket connection
// @exceptsafe no-throw
void Server::setPFD( int fd ) {
    struct pollfd pFD = { .fd = fd, .events = POLLIN | POLLOUT, .revents = 0 };
    m_pFD.push_back( pFD );
}

// Private Methods

// @brief Removes the client from the m_clients vector
// @params[in] fd File Descriptior
// @exceptsafe no-throw
void Server::removeClient( int fd ) {
    for ( std::vector<Clients *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it ) { // Search CLient List
        if ( (*it.base())->getFD() == fd ) {  // If this is the client being removed
            for ( std::vector<Channels *>::iterator cit = m_channels.begin(); cit != m_channels.end(); ++cit ) { // Search through all the channels

                if ( (*cit.base())->isClientInChannel( (*it.base()) ) ) { // If the client is in a channel
                    m_log.save( "Client " + libft::to_string( fd ) + ": Removed from channel list " + (*cit.base())->getName() );

                    try { (*cit.base())->delClient( (*it.base()) ); } // delete the client from the channel
                    catch ( Channels::NotFoundException& ex ) { }
                }
            }

            delete (*it.base()); // Remove the client from the server
            m_clients.erase( it );
            m_log.save( "Client " + libft::to_string( fd ) + ": Removed from server list" );
            break;
        }
    }

    for ( std::vector<pollfd>::iterator it = m_pFD.begin(); it != m_pFD.end(); ++it ) { // Remove the client from the poll list
        if ( it->fd == fd ) {
            m_pFD.erase( it );
            m_log.save( "Client " + libft::to_string( fd ) + ": Disconnected" );
            close( fd );
            break;
        }
    }
}

// @brief Checks channels for clients, and removes if empty
// @exceptsafe no-throw
void Server::removeChannel( void ) {
    for ( size_t i = 0; i < m_channels.size(); i++ ) {
        if ( m_channels[i]->getClientCount() == 0 ) {
            m_log.save( "Channel " + m_channels[i]->getName() + ": Empty Removing channel -" );

            delete m_channels[i];
            m_channels.erase( m_channels.begin() + i );
        }
    }
}

// @brief Returns the status of the irc server
// @returns[out] bool true if server is online, false otherwise
// @exceptsafe no-throw
bool Server::isOnline( void ) {
    if ( gSignalStatus && m_status == ONLINE )
        m_log.save( "Server: Received signal " + libft::to_string( gSignalStatus ) );

    if ( gSignalStatus == SIGTERM || gSignalStatus == SIGINT \
        || gSignalStatus == SIGQUIT || gSignalStatus == SIGHUP ) {
        this->stop();
    }

    return ( m_status ? true : false );
}

// @brief sigInit
// @exceptsafe no-throw
void Server::sigInit( void ) {
    std::signal( SIGINT, signalHandler );
    std::signal( SIGTERM, signalHandler );
    std::signal( SIGQUIT, signalHandler );
    std::signal( SIGHUP, signalHandler );
    std::signal( SIGPIPE, SIG_IGN );
}

std::string Server::printMsgQ( std::deque<std::vector<std::string> > msgQ ) const {
    std::string ret;

    for ( std::deque<std::vector<std::string> >::iterator it = msgQ.begin(); it != msgQ.end(); ++it ) {
        for ( size_t i = 0; i < it->size(); i++) {
            ret.append( it->data()[i] + " ");
        }

        if ( it != msgQ.end() - 1 )
            ret.append( "| ");
    }

    return ret;
}
