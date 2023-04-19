/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runtime.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:48 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/10 19:55:09 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/runtime.hpp"

using namespace irc;

// @brief RuntimeData Constructor, provides runtime data of channels, connected clients etc.
RuntimeData::RuntimeData( ServerData& sData ) : m_sData( sData ) {
    m_nickNameMaxLength = 12;
}

RuntimeData::~RuntimeData() {

}

// Runtime Data Methods

// @brief gets the nick name list from server
// @returns[in,out] std::map<std::string nickName, int fd>
// @throws std::runtime_error if RunTimeData not loaded
// @exceptsafe strong
std::vector<Clients *> RuntimeData::getClientList( void ) const {
    if ( !m_sData.m_clients.empty() )
        return m_sData.m_clients;

    throw std::logic_error( "RunTimeData Not Loaded" );
}

// @brief gets the client that matches the nickName
// @params[in] nickName The client nickname to lookup
// @returns[in,out] Clients Record of the found nickname, as a reference
// @throws std::runtime_error Throws on not found
// @throws std::logic_error on RunTimeData not loaded.
// @exceptsafe strong
Clients* RuntimeData::getClientFromNick( const std::string& nickName ) const {
    if ( m_sData.m_clients.empty() )
        throw std::logic_error( "RunTImeData Not Loaded" );

    for ( std::vector<Clients *>::iterator it = m_sData.m_clients.begin(); it != m_sData.m_clients.end(); ++it ) {
        if ( libft::to_upper( (*it.base())->getNickName() ) == libft::to_upper( nickName ) )
            return (*it.base());
    }

    throw NotFoundException( "Nick Not Found" );
}

// @brief gets the client that matches their file descriptor
// @params[in] fd File Descriptor
// @returns[in,out] Clients Record of the found file descriptor
// @throws std::runtime_error Throws on not found or rtd not loaded
// @exceptsafe strong
Clients* RuntimeData::getClientFromFD( const int& fd ) const {
    if ( m_sData.m_clients.empty() )
        throw std::logic_error( "RunTimeData Not Loaded" );

    for ( std::vector<Clients *>::iterator it = m_sData.m_clients.begin(); it != m_sData.m_clients.end(); ++it ) {
        if ( (*it.base())->getFD() == fd )
            return ( *it.base() );
    }

    throw NotFoundException( "FD Not Found" );
}

// @brief get the server password
// @returns[out] std::string password
const std::string& RuntimeData::getPassword( void ) const {
    return m_sData.m_password;
}

// @brief Get the Total Number of clients connected to the server
// @returns[out] size_t count of connected clients
// @exceptsafe no-throw
size_t RuntimeData::getClientsCount( void ) const {
    return m_sData.m_clients.size();
}

// @brief Get the Total Number of Invisible clients connected to the server
// @returns[ouy] size_t count of invisible clients
// @exceptsafe no-throw
size_t RuntimeData::getClientsInvCount( void ) const {
    size_t ret = 0;

    for ( std::vector<Clients *>::const_iterator it = m_sData.m_clients.begin(); it != m_sData.m_clients.end(); ++it ) {
        if ( (*it.base())->getMode() & 2 )
            ret += 1;
    }

    return ret;
}

// @brief Get the Maximum Nick Name Length
// @returns[out] size_t The max nickname length
// @exceptsafe no-throw
const size_t& RuntimeData::getNickMaxLength( void ) const {
    return m_nickNameMaxLength;
}

// @brief Gets the Channel Class holding the channel info
// @params[in] chanName Name of channel
// @throws std::invalid_argument if chanName is empty
// @throws NotFoundException if channel not found
// @exceptsafe strong
Channels* RuntimeData::getChannel( const std::string& chanName ) const {
    std::vector<Channels *>::iterator it;

    try { it = searchChan( chanName ); }
    catch ( std::invalid_argument& ex ) { throw ; }
    catch ( NotFoundException& ex) { throw ; }

    return *it;
}

// @brief Add a channel to the server
// @params[in] chanName Name of the channel to add
// @throws std::invalid_argument if a Duplicate Channel, chanName is empty or chanName is invalid (!# or !&)
// @exceptsafe strong
Channels* RuntimeData::addChannel( const std::string& chanName ) {

    try {
        getChannel( chanName );
        throw std::invalid_argument( "Duplicate Channel" );
    }
    catch( std::invalid_argument& ex ) { throw ; }
    catch ( NotFoundException& ex ) { }

    try { m_sData.m_channels.push_back( new Channels( chanName ) ); }
    catch ( std::invalid_argument& ex ) { throw ; }

    return m_sData.m_channels.back();
}

// @brief Delete Channel from the server
// @params[in] chanName Channel name to delete from server
// @throws std::invalid_argument if the chanName is empty
// @throws NotFoundException if the Channel does not exist
// @exceptsafe strong
void RuntimeData::delChannel( const std::string& chanName ) {
    std::vector<Channels *>::iterator it;

    try { it = searchChan( chanName ); }
    catch ( std::invalid_argument& ex ) { throw ; }
    catch ( NotFoundException& ex ) { throw ; }

    delete *it;
    m_sData.m_channels.erase( it );
}

// @brief gets a list of clients on a given channel
// @params[in] chanName Name of the channel to return
// @returns[out] std::vector<std::pair< Client NickName, Mode > > Returns list of clients and modes on the given channel
// @throws NotFoundException if channel is not found
// @exceptsafe strong
std::vector<std::pair<std::string, unsigned char> > RuntimeData::getChannelClient( const std::string& chanName ) const {
    std::vector<std::pair<std::string, unsigned char> > ret;
    std::vector<Channels *>::iterator it;

    try { it = searchChan( chanName ); }
    catch ( std::invalid_argument& ex ) { throw NotFoundException( "Channel Name Empty" ); }
    catch ( NotFoundException& ex ) { throw ; }

    const std::vector<std::pair<Clients *, unsigned char> > chanList = (*it.base())->getChannelList();

    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator cit = chanList.begin(); cit != chanList.end(); ++cit )
        ret.push_back( std::make_pair( (cit->first)->getNickName(), cit->second ));

    return ret;
}

std::deque<Channels *> RuntimeData::getMissingChannelOps( const int& count, const std::string& nickNameExclude ) const {
    std::deque<Channels *> ret;

    for ( std::vector<Channels *>::const_iterator it = m_sData.m_channels.begin(); it != m_sData.m_channels.end(); ++it ) {
        std::vector<std::pair<std::string, unsigned char> >tmp = getChannelClient( (*it.base())->getName() );

        int opsCount = 0;
        for ( std::vector<std::pair<std::string, unsigned char> >::const_iterator cit = tmp.begin(); cit != tmp.end(); ++cit ) {
            if ( cit->second & 2 && cit->first != nickNameExclude )
                opsCount++;
        }

        if ( opsCount <= count )
            ret.push_back( *it );
    }

    return ret;
}

// @brief geta a list of clients on a given channel by clinet *
// @params[in] chanName Name of the channel to return
// @returns[out] std::vector<Clients *> a vector of client *
// @throws NotFoundException on chanName not found
// @exceptsafe no-throw
std::vector<Clients *> RuntimeData::getChannelList( const std::string& chanName ) const {
    std::vector<Clients *> ret;
    std::vector<Channels *>::iterator it;

    try { it = searchChan( chanName ); }
    catch ( std::invalid_argument& ex ) { throw NotFoundException( "Channel Name Empty" ); }
    catch ( NotFoundException& ex ) { throw ; }

    const std::vector<std::pair<Clients *, unsigned char> > chanList = (*it.base())->getChannelList();

    for ( std::vector<std::pair<Clients *, unsigned char> >::const_iterator cit = chanList.begin(); cit != chanList.end(); ++cit ) {
        ret.push_back( cit->first );
    }

    return ret;
}

// @brief gets a list of clients that are in the same channel as the client parameter
// @params[in] client parameter of search
// @returns[out] std::vector<Clients *> a list of clients who have a common channel with client
// @exceptsafe no-throw
std::vector<Clients *> RuntimeData::getChannelList( Clients* client ) const {
    std::vector<Clients *> ret;
    std::set<Clients *>tmp;

    if ( m_sData.m_channels.empty() )
        return ret;

    for ( std::vector<Channels *>::const_iterator it = m_sData.m_channels.begin(); it != m_sData.m_channels.end(); ++it ) {

        if ( (*it.base())->isClientInChannel( client ) ) {
            std::vector<Clients *> clients = (*it.base())->getClientList();
            std::copy( clients.begin(), clients.end(), std::inserter( tmp, tmp.begin() ) );
        }
    }
    std::copy( tmp.begin(), tmp.end(), std::back_inserter( ret ) );

    return ret;
}

std::string RuntimeData::getServerStartTime( void ) const {
    return std::string();
}


// Private Methods

std::vector<Channels *>::iterator RuntimeData::searchChan( std::string chanName ) const {
    if ( chanName.empty() )
        throw std::invalid_argument( "Channel Name Empty" );

    std::vector<Channels *>::iterator it = m_sData.m_channels.begin();

    for ( ; it != m_sData.m_channels.end(); ++it ) {
        if ( (*it.base())->getName() == chanName )
            return it;
    }

    throw NotFoundException( "Channel Not Found" );
}
