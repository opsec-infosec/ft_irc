/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 11:17:32 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/07 11:39:02 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/channels.hpp"

using namespace irc;

// @brief Channel Constructor, defaults channel mode to +nt
// @params[in] channelName Name of Channel to create
// @throws invalid_argument if channelName is incorrect, empty or does not contain # or &
//  as first character
Channels::Channels( const std::string& channelName ) {
    if ( channelName.empty() )
        throw std::invalid_argument( "Channel or Topic empty" );

    if ( channelName[0] != '#' && channelName[0] != '&' )
        throw std::invalid_argument( "Channel name is invalid" );

    m_name = channelName;
    m_mode = DEFAULT_CHAN_MODE;
    m_log.save( "Channel " + channelName + ": Created +" );
}

Channels::~Channels() {

    m_channelClients.erase( m_channelClients.begin(), m_channelClients.end() );
    m_inviteList.erase( m_inviteList.begin(), m_inviteList.end() );
}

// Public Methods

// @brief gets the channel name
// @returns[out] std::string channelName
// @exceptsafe no-throw
const std::string& Channels::getName( void ) const {
    return m_name;
}

// @brief gets the channel topic
// @returns[out] std::string Channel Topic
// @exceptsafe no-throw
const std::string& Channels::getTopic( void ) const {
    return m_topic;
}

// @brief get the topicwhotime which includes nickname and time of topic change
// @returns[out] std::pair<std::string, std::string> ->first = nickname ->second = unix timestamp
// @exceptsafe no-throw
std::pair<std::string, std::string> Channels::getTopicWho( void ) const {
    std::pair<std::string, std::string> ret;

    ret = std::make_pair( m_whoTopic, m_timestamp );

    return ret;
}

// @brief gets the channel key
// @returns[out] std::string Channel Key
// @exceptsafe no-throw
const std::string& Channels::getKey( void ) const {
    return m_key;
}

// @brief sets the topic of the channel.  Checks whether the client has Chan Op mode
// @params[in] client Client requsting to change the topic
// @params[in] topic Topic string to se on the channel
// @throws ModeViolationException "t" if the client does not have Chan Op +o and chanmode is +t
// @throws NotFoundException if the client does not exist on the channel
// @exceptsafe strong
void Channels::setTopic( const Clients* client, const std::string& topic ) {
    if ( topic.empty() )
        return ;

    unsigned char chanOpMode;
    try { chanOpMode = getChanOpMode( client ); }
    catch ( NotFoundException& ex ) { throw ; }

    if ( getChanMode() & 256 && !( chanOpMode & 2 ) )
        throw ModeViolationException( "t" );

    m_topic = topic;
    m_whoTopic = client->getNickName();
    m_timestamp = libft::getCurUnixTimeString();
}

// @brief sets the Channel Key, automatically sets the mode to +k
// @params[in] key Channel Key
// @throws std::runtime_error if key contains any invalid characters like spaces
// @exceptsafe strong
void Channels::setKey( std::string key ) {
    if ( !( libft::isalnumStr( key ) ) )
        throw std::runtime_error( "Invalid Key" );

    m_key = key;
    setChanMode( getChanMode() | 32 );
}

// [  n   t   s    m   k   I   i  l  e  b ]
// [ 512 256  128  64  32  16  8  4  2  1 ]
// @brief get the channel mode as a bitfield
// @returns unsigned short int (2 bytes) Mode of Channel
// @exceptsafe no-throw
unsigned short int Channels::getChanMode( void ) const {
    return m_mode;
}

// [           q ~   & a   % h   @ o   + v ]
// [ 8  7  6    5     4     3     2     1  ]
// @brief get the channel operator mode as a bitfield
// @returns unsigned char (1 byte) Mode of Client Operator
// @throws NotFoundException if client not found
// @exceptsafe strong
unsigned char Channels::getChanOpMode( const Clients* client ) const {
    if ( !client )
        return 0;

    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        if ( client == it->first )
            return it->second;
    }

    throw NotFoundException( "Client Not Found" );
}

// @brief Sets the channel mode as a bitfield
// [  n   t   s    m   k   I   i  l  e  b ]
// [ 512 256  128  64  32  16  8  4  2  1 ]
// @params[in] mode unsigned short int  of bit field
// @exceptsafe no-throw
void Channels::setChanMode( unsigned short int mode ) {
    //  To set a bit use | x (where x is the Mask with below)
    //      channel->setChanMode( channel->getChanMode() | 2 );
    //
    //  To clear a bit use & ~x (where x is the Mask with below)
    //      channel->setChanMode( channel->getChanMode() & ~2 );
    //
    // Implementd     Bit   Mask with   F   Description
    //      N          1        1       b - Ban Channel Mask
    //      N          2        2       e - Ban Channel Except Mask
    //      Y          3        4       l - Channel User limit
    //      Y          4        8       i - Invite Only
    //      N          5        16      I - Invite Only Except Mask
    //      Y          6        32      k - Key Mode
    //      Y          7        64      m - Moderated Mode
    //      Y          8       128      s - Secret Mode
    //      Y          9       256      t - Protected Topic Mode
    //      Y         10       512      n - No Extern Messages Mode
    //
    m_mode = mode;
}

// [           q ~   & a   % h   @ o   + v ]
// [ 8  7  6    5     4     3     2     1  ]
// @brief get the channel operator mode as a bitfield
// @params[in] client Client record to set their mode
// @params[in] mode Mode to set for the client
// @exceptsafe no-throw
void Channels::setChanOpMode( Clients *client, unsigned char mode ) {
    //  To set a bit use | x (where x is the Mask with below)
    //      channel->setChanOpMode( channel->getChanOpMode() | 2 );
    //
    //  To clear a bit use & ~x (where x is the Mask with below)
    //      channel->setChanOpMode( channel->getChanOpMode() & ~2 );
    //
    // Implementd     Bit   Mask with   F   Description
    //      Y          1        1       v - (+) Voice Mode
    //      Y          2        2       o - (@) Channel Ops Mode
    //      N          3        4       h - (%) Halfpops Mode
    //      N          4        8       a - (&) Protected Mode
    //      N          5        16      q - (~) Founders Mode
    //      N          6        32        - Not Used
    //      N          7        64        - Not Used
    //      N          8       128        - Not Used
    //

    if ( mode > 31 )
        return ;

    std::vector<std::pair<Clients *, unsigned char> >::iterator it;

    try { it = searchClient( client ); }
    catch ( std::invalid_argument& ex ) { return ; }
    catch ( NotFoundException& ex ) { return ; }

    it->second = mode;
}

// @brief set Channel Limit
// @params[in] size the limit on the number of user allowed to join the channel
// @exceptsafe no-throw
void Channels::setChanLimit( size_t size ) {
    if ( size )
        m_channelLimit = size;
}

// @brief Add a Client to the inviation list
// @params[in] client the client to add to the invitation list
// @exceptsafe no-throw
void Channels::addClientInvite( Clients* client ) {
    if ( !client )
        return ;

    // Check invite list, only add the invite once
    for ( std::vector<std::string>::const_iterator it = m_inviteList.begin(); it != m_inviteList.end(); ++it ) {
        if ( *it == client->getNickName() )
            return ;
    }

    m_inviteList.push_back( client->getNickName() );
    m_log.save( "Channel " + m_name + ": Invite Client " + libft::to_string( client->getFD() ) + " " + client->getNickName() + " +" );
}

// @brief Add client to channel, no checks on Invite only
// @params[in] client Client *
// @throws std::runtime_error if client is already found in the channel
// @throws ModeViolationException if client can't join the channel mode +l +i
// @exceptsafe strong
void Channels::addClient( Clients* client ) {
    if ( !client )
        return ;

    if ( client->getChannelCount() >= 10 )
        return ;

    // Search for client in channel, if not there we add.. if the client is already there we throw runtime_error
    std::vector<std::pair<Clients *, unsigned char> >::const_iterator it;

    try { it = searchClient( client ); }
    catch ( std::invalid_argument& ex ) { return ; }
    catch ( NotFoundException& ex ) {

        if ( m_mode & 4 && m_channelClients.size() == m_channelLimit )
            throw ModeViolationException( "l" );

        if ( m_mode & 8 ) {
            if ( m_inviteList.empty() )
                throw ModeViolationException( "i" );

            std::vector<std::string>::iterator it = m_inviteList.begin();

            for ( ; it != m_inviteList.end(); ++it ) {
                if ( client->getNickName() == *it ) {
                    m_log.save( "Channel " + m_name + ": Invite Founmd Client " + libft::to_string( client->getFD() ) + " "  + *it + " +" );
                    break;
                }
            }

            if ( it == m_inviteList.end() )
                throw ModeViolationException( "i" );
        }

        // When Client is added to channel, we remove them from the invitation list
        if ( !m_inviteList.empty() ) {
            for ( std::vector<std::string>::iterator it = m_inviteList.begin(); it != m_inviteList.end(); ++it ) {
                if ( client->getNickName() == *it ) {
                    m_inviteList.erase( it );
                    m_log.save( "Channel " + m_name + ": Remove Invite Client " + libft::to_string( client->getFD() ) + " " + client->getNickName() + " -");
                    break ;
                }
            }
        }

        m_channelClients.push_back( std::make_pair( client, DEFAULT_OP_MODE ) );

        //Add this channel to the clients joined chan list
        client->addJoinChan( m_name );
        m_log.save( "Channel " + m_name + ": Add Client " + libft::to_string( client->getFD() ) + " " + client->getNickName() + " +" );

        return ;
    }

    throw std::runtime_error( "Client Already Added" );
}

// @brief Deletes client from channel
// @params[in] client nick name of client
// @throws NotFoundException if client not found
// @exceptsafe strong
void Channels::delClient( Clients* client ) {
    if ( !client )
        throw NotFoundException( "Client Not Found" );

    if ( !client->isMarkedForKill() && client->isMarkedForDeletion() )
        sendQuitMsg( client );

    std::vector<std::pair<Clients *, unsigned char> >::iterator it;

    try { it = searchClient( client ); }
    catch ( std::invalid_argument& ex ) { return ; }
    catch ( NotFoundException& ex ) { throw ; }

    m_channelClients.erase( it );

    // Delete this channel for clients chan list
    client->delJoinChan( m_name );
    m_log.save( "Channel " + m_name + ": Removed Client " + libft::to_string( client->getFD() ) + " " + client->getNickName() + " -");
}

// @brief Send Private Messages to Channel Clients
// @params[in] client Client sending the message
// @params[in] command Message to send to Channel
// @params[in] trailing trailing params
// @params[in] target target of the message
// @throws ModeViolationException if unable to send message to channel ( check the ex return n or m )
// @exceptsafe strong
void Channels::chanMsg( Clients* client, std::string source,  std::string command, std::string trailing, std::string target, std::string operation ) {
    if ( !client || command.empty() || m_channelClients.empty() )
        return ;

    if ( command == "PRIVMSG" ) {
        if ( getChanMode() & 512 && !isClientInChannel( getClient( source ) ) )
            throw ModeViolationException( "n" ) ;

        if ( getChanMode() & 64 ) {
            unsigned char chanopmode;
            try { chanopmode = getChanOpMode( getClient( source ) ); }
            catch ( NotFoundException& ex ) { throw ModeViolationException( "m" ); }

            if ( !(chanopmode & ( 1 | 2 ) ) )
                throw ModeViolationException( "m" );
        }
    }
    if ( !operation.empty() )
        client->setMsgQS( std::string( ":" + source + " " + command + " " + target + " " + operation + ( trailing.empty() ?  "" : " :" + trailing ) + "\r\n" ) );
    else
        client->setMsgQS( std::string( ":" + source + " " + command + " " + target + ( trailing.empty() ?  "" : " :" + trailing ) + "\r\n" ) );
}

// @brief Send Quit Message to Channel Clients
// @params[in] client Client sending the quit message
// @exceptsafe no-throw
void Channels::sendQuitMsg( Clients *client, std::string reason ) const {
    if ( !client || m_channelClients.empty() )
        return ;

    for ( std::vector<std::pair<Clients*, unsigned char> >::const_iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        if ( client->getNickName() != (it->first)->getNickName() )
            (it->first)->setMsgQS( std::string( ":" + client->getNickName() + " QUIT " + ":QUIT: " + reason + " \r\n" ) );
    }
}

// @brief Get the Number of clients in the channel
// @returns[out] size_t Number of connected clients in the channel
// @exceptsafe no-throw
size_t Channels::getClientCount( void ) const {
    return m_channelClients.size();
}

// @brief Is Client in the current channel
// @params[in] client Client to check if in this channel
// @returns[out] bool if client is in this channel returns true
// @exceptsafe no-throw
bool Channels::isClientInChannel( const Clients *client ) const {
    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        if ( it->first->getNickName() == client->getNickName() )
            return true;
    }
    return false;
}

// @brief gets the list of clients connected in the channel
// @returns[out] const std::vector<std::pair<Clients*,unsigned char>>& const Ref List of all clients in channel
// @exceptsafe no-throw
const std::vector<std::pair<Clients*, unsigned char> > Channels::getChannelList( void ) const {
    return m_channelClients;
}

// @brief gets the list of clients in the channel
// @returns[out] std::vector<Clients *> vector of clients in the channel
// @exceptsafe no-throw
const std::vector<Clients *> Channels::getClientList( void ) const {
    std::vector<Clients *> ret;

    if ( m_channelClients.empty() )
        return ret;


    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        ret.push_back( it->first );
    }

    return ret;
}

// Private Methods

// @brief gets a client record by nickname that is in this channel
// @params[in] nickName gets a client record by nickname
// @returns[out] Clients* record of the client
// @throws NotFoundException if nickname not found
// @exceptsafe strong
Clients* Channels::getClient( std::string nickName ) const {
    if ( nickName.empty() )
        throw NotFoundException( "Nickname Not Found" );

    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        if ( (*it.base()).first->getNickName() == nickName )
            return it->first;
    }

    throw NotFoundException( "Nickname Not Found" );
}


// @brief Searches for a client in the channel list and returns an iterator pointing to the record
// @params[in] client Client record to search for
// @returns[out] std::vector<std::pair<Clients *, unsigned char>> iterator pointing to the client record
// @throws NotFoundException if the client is not found in the channel list
// @exceptsafe strong
std::vector<std::pair<Clients *, unsigned char> >::iterator Channels::searchClient( Clients* client ) {
    if ( !client )
        throw std::invalid_argument( "Client Empty" );

    for ( std::vector<std::pair<Clients *, unsigned char> >::iterator it = m_channelClients.begin(); it != m_channelClients.end(); ++it ) {
        if ( client == it->first )
            return it;
    }

    throw NotFoundException( "Client Not Found" );
}
