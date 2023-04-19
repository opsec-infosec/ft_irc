/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   marvin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 21:46:48 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/14 19:12:20 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/marvin.hpp"

using namespace irc;

// @brief Marvin the depressed robot from HHG
// @exceptsafe no-throw
Marvin::Marvin( const std::string& nickName, const std::string& ipAddress, const int& port ) :  m_nickName( nickName ), m_registered( OFFILINE ) {
    try { connectBot( ipAddress, port ); }
    catch (MarvinFellDown& ex) { return ; }
}

// @brief Marvins destructor, cleans up Marvins mess
// @exceptsafe no-throw
Marvin::~Marvin( ) {
    disconnectBot();

    if ( !m_msgQR.empty() )
        m_msgQR.erase( m_msgQR.begin(), m_msgQR.end() );

    m_log.save( "Marvin Client " + libft::to_string( m_fd ) + ": Cleanup" );
 }

// Public Methods

// @brief talk - entrypoint into Marvin the Bot
// @exceptsafe no-throw
void Marvin::talk( RuntimeData* rtd ) {
    m_rtd = rtd;

    recvMsg();

    std::vector<std::string> rec;

    if ( !m_registered )
        return ;

    // Register the Client with the irc server
    if ( m_registered == REGISTER || m_registered == CONNECT ) {
        if ( m_registered == CONNECT ) {
            std::string conn = "CAP LS\r\n";
            sendMsg( conn );
            m_registered = REGISTER;
        }

        if ( m_registered == REGISTER && m_msgQR.size() && getSource() == libft::to_string( SERVER ) && getCmd() ==  "CAP" && getTarget() ==  "*" ) {
            std::string conn = "PASS " + m_rtd->getPassword() + "\r\nNICK Marvin\r\nUSER Marvin Marvin 127.0.0.1 :Marvin\r\nCAP END\r\n";
            sendMsg( conn );
        }

        if ( m_registered == REGISTER && m_msgQR.size() && getCmd() == "NICK" && getTarget() == m_nickName ) {
            try {
                m_client = m_rtd->getClientFromNick( m_nickName );
                m_registered = ONLINE;
            }
            catch (RuntimeData::NotFoundException& ex ) { m_registered = CONNECT; }
        }

        if ( !m_msgQR.empty() )
            m_msgQR.pop_front();

        return ;
    }

    // Start executing the messages
    while ( m_msgQR.size() && m_registered == ONLINE ) {
        executeCmd( getSource(), getTarget(), getCmd(), getParams(), getTrailing() );
        m_msgQR.pop_front();
    }
}

// Private Methods

// @brief Execute Commands - where we can parse messages / commands and send replies back to targets
// @params[in] source Source of message
// @params[in] target Target of the message, usually Marvin
// @params[in] command Command sent from server
// @params[in] params Vector of std::string parameters
// @params[in] trailing trailing message
// @exceptsafe no-throw
void Marvin::executeCmd( const std::string& source, const std::string& target, const std::string& command, const std::vector<std::string>& params, const std::string& trailing ) {

    // Check channels already joined and make sure marvin is not the last one in the channel
    if ( ( command == "PART" || command == "QUIT" ) && !m_prevChannels.empty() ) {
        for ( std::deque<std::pair<std::string, std::string> >::iterator it = m_prevChannels.begin(); it != m_prevChannels.end(); ++it ) {
            if ( it->first == target ) {
                Channels* chan;
                ICommand* icmd = NULL;

                try {
                    chan = m_rtd->getChannel( it->first );
                    std::string chanName = chan->getName();

                    if ( chan->getClientCount() <= 1 ) {
                        icmd = new Part();
                        icmd->loadRunTimeData( m_rtd );
                        icmd->setClient( m_client );
                        ((Part *)icmd)->part( m_client, chanName );
                        m_prevChannels.erase( it );
                    }
                }
                catch ( Channels::NotFoundException& ex ) { }
                catch ( std::runtime_error& ex ) { }
                catch ( std::invalid_argument& ex ) { }

                if ( icmd )
                    delete icmd;

                break ;
            }
        }
    }

    // The Intro message... there is a clue here.
    if ( command == "PRIVMSG" && target == m_nickName && trailing != "Help" && trailing != "Thanks" ) {
        return sendMsg( command + " " + source + " :Hello " + source + " how can I Help you? Not that you need help... I hate talking to Humans...\r\n" );
    }

    // Ok.. now that we are thanked... we eject out of the channel
    if ( command == "PRIVMSG" && trailing == "Thanks" ) {
        if ( m_prevChannels.empty() )
            return ;

        for ( std::deque<std::pair<std::string, std::string> >::iterator it = m_prevChannels.begin(); it != m_prevChannels.end(); ++it ) {
            if ( it->first == target ) {
                ICommand* icmd = NULL;

                try {
                    icmd = new Part();
                    icmd->loadRunTimeData( m_rtd );
                    icmd->setClient( m_client );
                    ((Part *)icmd)->part( m_client, it->first );
                    m_prevChannels.erase( it );
                }
                catch ( std::runtime_error& ex ) { }
                catch ( std::invalid_argument& ex ) { }

                if ( icmd )
                    delete icmd;

                break ;
            }
        }
    }

    // We join the channel and set the operator to the source of the help privmsg
    if ( command == "PRIVMSG" && target == m_nickName && trailing == "Help") {
        std::deque<Channels *> opsMissing = m_rtd->getMissingChannelOps( 0, m_client->getNickName() );

        if ( opsMissing.empty() )
            sendMsg( "PRIVMSG " + source + " :Here I am, brain the size of a planet, and they ask me to pick up a piece of paper\r\n" );
        else
            sendMsg( "PRIVMSG " + source + " :Don’t pretend you want to talk to me, I know you hate me\r\n" );

        for ( std::deque<Channels *>::const_iterator it = opsMissing.begin(); it != opsMissing.end(); ++it ) {
            Clients* fClient;
            Channels* channel = *it;
            std::string chanName = channel->getName();

            ICommand* icmd = NULL;

            try { fClient = m_rtd->getClientFromNick( source ); }
            catch ( RuntimeData::NotFoundException& ex ) { return ; }

            if ( channel->isClientInChannel( fClient ) ) {
                try {
                    icmd = new Join();
                    icmd->loadRunTimeData( m_rtd );
                    icmd->setClient( m_client );
                    ((Join *)icmd)->join(m_client, chanName, channel->getKey() );
                    delete icmd;

                    icmd = new Mode();
                    icmd->loadRunTimeData( m_rtd );
                    icmd->setClient( m_client );

                    if ( !( channel->getChanOpMode( m_client ) & 3 ) )
                        ((Mode *)icmd)->mode( m_client, m_nickName, chanName, 3, '+' );

                    ((Mode *)icmd)->mode( m_client, fClient->getNickName(), chanName, 2, '+' );
                    sendMsg( "PRIVMSG " + chanName + " :I interfaced with the main computer, and was able to get " + fClient->getNickName() + " channel ops!!!\r\n" );
                }
                catch ( Channels::NotFoundException& ex ) { }
                catch ( Channels::ModeViolationException& ex) {
                    sendMsg( "PRIVMSG " + source + " :Well you certainly are in a pickel now... with the channel " + chanName + " arent you!!!\r\n" );
                    channel->setChanOpMode( fClient, 2 );
                    sendMsg( "PRIVMSG " + source + " :Well I interfaced with the main computer, and was able to get you channel ops... it hates me by the way\r\n" );
                }
                catch ( std::runtime_error& ex ) {
                    if ( icmd )
                        delete icmd;

                    return;
                }

                m_prevChannels.push_back( std::make_pair( chanName, source ) );
                delete icmd;
            }
            else {
                sendMsg( "PRIVMSG " + source + " :Well I can't really help you.\r\n" );
            }
        }
    }

    (void)params;
}


// @brief Setup connection to the irc server to connect Marvin
// @params[in] ipAddress IP Address of the server, defaults to 127.0.0.1 localhost
// @params[in] port to connect to the server, defaults to port 6667
// @throws MarvinFellDown on invalid port, socket, or connection
// @excepsafe strong
void Marvin::connectBot( const std::string& ipAddress, const int& port ) {
    if ( ipAddress.empty() || ( port < 0 || port >= 65536 ) )
            throw MarvinFellDown( "invalid address or port" );

    struct sockaddr_in address;

    if ( ( m_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0)
        throw MarvinFellDown( "socket error " );

    int optval = 1;
    if ( setsockopt( m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, (socklen_t)sizeof( optval ) ) < 0 ) {
        int err = errno;
        m_log.save( "Server: socket options error " + libft::to_string( err ), this );
        close( m_fd );
        throw std::runtime_error( "Server: socket options error" );
    }

    if ( setsockopt( m_fd, SOL_SOCKET, SO_REUSEPORT, &optval, (socklen_t)sizeof( optval ) ) < 0 ) {
        int err = errno;
        m_log.save( "Server: socket options error " + libft::to_string( err ), this );
        close( m_fd );
        throw std::runtime_error( "Server: socket options error" );
    }

    std::memset( &address, 0, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_port = htons( port );

    if ( inet_pton( AF_INET, ipAddress.c_str(), &address.sin_addr ) <= 0 ) {
        close( m_fd );
        throw MarvinFellDown( "conversion to sockaddr failed" );
    }

    if ( ( connect( m_fd, (struct sockaddr*)&address, sizeof( address ) ) ) < 0 ) {
        close( m_fd );
        throw MarvinFellDown( "failed to connect to " + ipAddress + " on port " + libft::to_string( port ) );
    }

    m_registered = CONNECT;
}

// @brief Disconnect the bot from irc
// @exceptsafe no-throw
void Marvin::disconnectBot( void ) {
    close( m_fd );
    m_registered = OFFILINE;
    m_log.save( "Marvin Client " + libft::to_string(m_fd ) + ": Disconnected" );
}

// @brief Send Messages to the irc server
// @params[in] reply std::string raw message, don't forget to include the /r/n
// @exceptsafe no-throw
void Marvin::sendMsg( const std::string& reply ) {
    if ( reply.empty() )
        return ;

    #ifdef __APPLE__
     send( m_fd, reply.c_str(), reply.size(), SO_NOSIGPIPE );
    #else
     send( m_fd, reply.c_str(), reply.size(), MSG_NOSIGNAL );
    #endif
}

// @brief Receive Messages from the irc server and queue them in a buffer.  We split up the messages the same as the server
// @exeptsafe no-throw
void Marvin::recvMsg( void ) {
    char buffer [BUFFSIZE];
    ssize_t byteRec;

    if ( ( byteRec = recv( m_fd, &buffer, BUFFSIZE, MSG_DONTWAIT ) ) <= 0 ) {
        if ( errno != EWOULDBLOCK )
            disconnectBot();

        return ;
    }

    m_rawBuffer.append( buffer, byteRec );
    std::queue<std::string>tmp = libft::splitStr( m_rawBuffer, "\r\n" );

    while ( tmp.size() ) {
        m_msgQR.push_back( splitMessage( tmp.front() ) );
        tmp.pop();
    }

    return ;
}

// @brief Get Source of message
// @returns[out] std::string of the source nickname that sent the message
// @exceptsafe no-throw
std::string Marvin::getSource( void ) const {
    std::string ret;

    if ( m_msgQR.empty() && m_msgQR.front().empty() )
        return ret;

    ret = m_msgQR.front().front().substr( 1 );
    return ret;
}

// @brief Get the Command sent by the irc server, can also be Reply error numbers
// @returns[out] std::string of the command received
// @exceptsafe no-throw
std::string Marvin::getCmd( void ) const {
    if ( m_msgQR.empty() || m_msgQR.front().size() < 2 )
        return std::string();

    return m_msgQR.front()[1];
}

// @brief Get Target for the message, usually Marvin
// @returns[out] std::string Target nickname of the message
// @exceptsafe no-throw
std::string Marvin::getTarget( void ) const {
    std::string ret;

    if ( m_msgQR.empty() || ( m_msgQR.front().size() < 3 || m_msgQR.front().size() > 4 ) )
        return std::string();

    ret = ( m_msgQR.front().end() - 2 )->data();


    return ret;
}

// @brief Get Trailing part of the message
// @returns[out] std::string Traiing message
// @exceptsafe no-throw
std::string Marvin::getTrailing( void ) const {
    std::string ret;

    if ( m_msgQR.empty() || !m_msgQR.front().size() )
        return ret;

    std::string tmp = m_msgQR.front().back();
    if ( !tmp.empty() && tmp[0] == ':' )
        ret = m_msgQR.front().back().substr( 1, m_msgQR.front().back().size() );

    return ret;
}

// @brief Gets the Parameters of the message as a vector of strings
// @returns[out] std::vector<std::string> of parameters
// @exceptsafe no-throw
std::vector<std::string> Marvin::getParams( void ) const {
    std::vector<std::string> ret;

    if ( m_msgQR.empty() || m_msgQR.front().size() < 4 )
        return ret;

    for ( std::vector<std::string>::const_iterator it = m_msgQR.front().begin() + 3; it != m_msgQR.front().end() - 1; ++it )
        ret.push_back( *it );


    return ret;
}

std::vector<std::string> Marvin::splitMessage( const std::string& val ) {
        std::vector<std::string> words;
        std::string tmp;
        std::string trailing;

        if ( val.size() < 2 )
            return words;

        size_t pos = val.find_first_of(':', 1 );

        if ( pos != std::string::npos )
            trailing = val.substr( pos );

        std::stringstream sst( val.substr( 0, pos ) );
        while ( sst>>tmp )
            words.push_back( tmp );

        if ( !trailing.empty() )
            words.push_back( trailing );

        return words;
}
