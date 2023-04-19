/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:11:17 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:37:07 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/clients.hpp"

using namespace irc;

// @brief Client Construtor, default fd -1
Clients::Clients() : m_fd( -1 ), m_address( "0.0.0.0" ) {

}

// @brief Client Constructor
// @param[in,out] fd FileDescriptor of open socket
Clients::Clients( const int& fd, const std::string& address ) : m_fd( fd ), m_address( address ) {
    m_isAuthenticated = false;
    m_isRegistered = false;
    m_markForDeletion = false;
    m_markForKill = false;
    m_clientAttempts = 0;
    m_mode = 0;
}

// @brief Client Destructor
Clients::~Clients() {
    m_msgQR.erase( m_msgQR.begin(), m_msgQR.end() );
    m_msgQS.erase( m_msgQS.begin(), m_msgQS.end() );
    m_channelJoined.erase( m_channelJoined.begin(), m_channelJoined.end() );
}

// @brief Receive data from socket
// @returns{out} size_t Bytes Received from Socket and stores in msgQ
// @exceptsafe no-throw
size_t Clients::receive( void ) {
    char buffer[BUFFSIZE];
    ssize_t byteRec;

    if ( ( byteRec = recv( m_fd, &buffer, BUFFSIZE, 0 ) ) <= 0 )
        return byteRec;

    m_rawBuffer.append( buffer, byteRec );
    std::queue<std::string>tmp = libft::splitStr( m_rawBuffer, "\r\n" );

    while ( tmp.size() ) {
        m_msgQR.push_back( libft::splitSpace( tmp.front() ) );
        tmp.pop();
    }

    return byteRec;
}

// @brief Gets the Clients File Descriptor [Server Operation]
// @returns[out] int Clients File Descriptor
// @exceptsafe no-throw
int Clients::getFD( void ) {
    return m_fd;
}

// @brief Gets the client receive message queue [Server Operation]
// @return[out,in] std::queue<std::vector<std::string> > client split message queue
// @exceptsafe no-throw
std::deque<std::vector<std::string> >& Clients::getMsgQR( void ) {
    return m_msgQR;
}

// @brief Gets the client send message queue [Server Operation]
// @return[out] std::deque<std::string> client send buffer
// @exceptsafe no-throw
std::deque<std::string>& Clients::getMsgQS( void ) {
    return m_msgQS;
}

// @brief Put a message into the client send message queue [Server Operation]
// @params[in] msg std::string raw message to send
// @exceptsafe no-throw
void Clients::setMsgQS( std::string msg ) {
    m_msgQS.push_back( msg );
}

// @brief Checks if client is authenticated
// @returns[out] bool returns true if authenticated
// @exceptsafe no-throw
bool Clients::isAuthenticated( void ) const {
    return m_isAuthenticated;
}

// @brief Checks if client is registered
// @returns[out] bool returns true if registered
// @exceptsafe no-throw
bool Clients::isRegistered( void ) const {
    return m_isRegistered;
}

// @brief Checks if client is operator
// @returns[out] bool returns true if client is oper
// @exceptsafe no-throw
bool Clients::isOperator( void ) const {
    return ( m_mode & 16 );
}

// @brief Gets the clients attempted logins
// @returns[out,in] int Reference to clientAttempts
// @exceptsafe no-throw
int& Clients::getClientAttempts( void ) {
    return m_clientAttempts;
}

// @brief Gets the clients nick name
// @returns[out] std::string Nick name of client
// @exceptsafe no-throw
const std::string Clients::getNickName( void ) const {
    return m_nickName;
}


// @brief Sets the clients nick name
// @params[in] nick
// @exceptsafe no-throw
void Clients::setNickName( const std::string& nick ) {
    m_nickName = nick;
}

// @brief Gets the clients user name
// @returns[out] std::string User name of client
// @exceptsafe no-throw
const std::string Clients::getUserName( void ) const {
    return m_userName;
}

// @brief Sets the clients User name
// @params[in] userName
// @exceptsafe no-throw
void Clients::setUserName( const std::string& userName ) {
    m_userName = userName;
}

// @brief get the last joined channel
// @returns[out] std::string Last joined channel Nnme
// @exceptsafe no-throw
const std::string Clients::getLastJoinChan( void ) const {
    if ( m_channelJoined.empty() )
        return std::string();

    return m_channelJoined.back();
}

// @brief gets a vector of std::string list of joined channels
// @returns[out] vector<string> sorted list of joined channels
// @exceptsafe no=throw
const std::vector<std::string> Clients::getJoinChan( void ) const {
    std::vector<std::string> ret;

    if ( m_channelJoined.empty() )
        return ret;

    std::list<std::string> tmp( m_channelJoined );
    tmp.sort();

    for ( std::list<std::string>::const_iterator it = tmp.begin(); it != tmp.end(); ++it )
        ret.push_back( *it );

    return ret;
}

// @brief Add a channel joined by client to the joinchan list
// @params[in] chanName Channel name to add
// @exceptsafe no-throw
void Clients::addJoinChan( const std::string& chanName ) {
    if ( chanName.empty() )
        return ;

    m_channelJoined.push_back( chanName );
}

// @brief Dell a channel joined by the client from the joinchan list
// @params[in] chanName Channel name to del
// @exceptsafe no-throw
void Clients::delJoinChan( const std::string& chanName ) {
    if ( chanName.empty() )
        return ;

    m_channelJoined.remove( chanName );
}

// @brief Gets the clients Real name
// @returns[out] std::string Real name of client
// @exceptsafe no-throw
const std::string Clients::getRealName( void ) const {
    return m_realName;
}

// @brief Sets the clients Real name
// @params[in] realName
// @exceptsafe no-throw
void Clients::setRealName( const std::string& realName ) {
    m_realName = realName;
}

// @brief Gets the hostname of clients connection
// @returns[out] std::string hostname of client connection
// @exceptsafe no-throw
const std::string Clients::getHostName( void ) const {
    return m_hostName;
}

// @brief Sets the hostname of clients connection
// @params[in] hostName
// @exceptsafe no-throw
void Clients::setHostName( const std::string& hostName ) {
    m_hostName = hostName;
}

// @brief Gets the hostname of clients connection
// @returns[out] std::string hostname of client connection
// @exceptsafe no-throw
const std::string Clients::getConnHostName( void ) const {
    return m_connectedHostName;
}

// @brief Sets the server hostname of clients connection
// @params[in] hostName
// @exceptsafe no-throw
void Clients::setConnHostName( const std::string& hostName ) {
    m_connectedHostName = hostName;
}

// @brief Set true if authenticate with irc password
// @params[in] isAuthenticated
// @exceptsafe no-throw
void Clients::setAuthenticated( bool isAuthenticated ) {
    m_isAuthenticated = isAuthenticated;
}

// @brief Set true if registered
// @params[in] isRegistered
// @exceptsafe no-throw
void Clients::setRegisterd( bool isRegistered ) {
    m_isRegistered = isRegistered;
}

// @brief Gets the user mode as a bitfield
//  [  x  s  O  o  r w i a ]
//  [ 128 64 32 16 8 4 2 1 ]
// @returns[out] unsigned char Bitfield of user modes
// @exceptsafe no-throw
unsigned char Clients::getMode( void ) const {
    return m_mode;
}

// @brief Sets the user mode as a bitfield
//  [  *  s  O  o  r w i a ]
//  [ 128 64 32 16 8 4 2 1 ]
// @params[in] mode unsigned char of bit field
// @exceptsafe no-throw
void Clients::setMode(unsigned char mode ) {
    //  To set a bit use | x (where x is the Mask with below)
    //      client->setMode( client->getMode() | 2 );
    //
    //  To clear a bit use & ~x (where x is the Mask with below)
    //      client->setMode( client->getMode() & ~2 );
    //
    // Implementd     Bit   Mask with   F   Description
    //      N          1        1       a - user is flagged as away;
    //      Y          2        2       i - marks a users as invisible;
    //      Y          3        4       w - user receives wallops;
    //      Y          4        8       r - restricted user connection;
    //     +Y          5        16      o - operator flag; (+ via OPER command)
    //      N          6        32      O - local operator flag;
    //      N          7        64      s - marks a user for receipt of server notices.
    //
    m_mode = mode;
}

// @brief gets the current # of channels the client is connected to
// @returns[out] int current channel count of the client
// @exceptsafe no-throw
int Clients::getChannelCount( void ) const {
    return m_channelJoined.size();
}

// @brief Mark Client for deletion and closing socket
// @params[in] fd the Client File Descriptor, it should be this->m_fd
// @exceptsafe no-throw
void Clients::markForDeletion( int fd ) {
    if ( fd == m_fd )
        m_markForDeletion = true;
}

void Clients::markForKill( int fd ) {
    if ( fd == m_fd )
        m_markForKill = true;
}

// @brief Check is client is marked for deletion and closing of socket
// @returns[out] bool true if marked for deletion
// @exceptsafe no-throw
bool Clients::isMarkedForDeletion( void ) const {
    return m_markForDeletion;
}

// @brief Check is client is marked for deletion and closing of socket Kill Event
// @returns[out] bool true if marked for deletion
// @exceptsafe no-throw
bool Clients::isMarkedForKill( void ) const {
    return m_markForKill;
}
