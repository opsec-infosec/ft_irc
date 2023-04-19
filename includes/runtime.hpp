/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runtime.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:08 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 01:48:24 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RUNTIME_DATA_HPP
# define RUNTIME_DATA_HPP
# include "clients.hpp"
# include "channels.hpp"
# include "serverData.hpp"
# include <map>
# include <vector>
# include <set>
# include <iterator>
# include <algorithm>
# include <iostream>

namespace irc {

    class ServerData;

    class RuntimeData {
        public:
            RuntimeData( ServerData& sData );
            ~RuntimeData( );

            class NotFoundException : public std::runtime_error {
                public:
                    NotFoundException( const std::string &ex ) : std::runtime_error( ex ) { }

            };

            // Getters
            Clients* getClientFromNick( const std::string& nickName ) const;
            Clients* getClientFromFD( const int& fd ) const;
            std::vector<Clients *> getClientList( void ) const;
            const std::string& getPassword ( void ) const;
            size_t getClientsCount( void ) const;
            size_t getClientsInvCount( void ) const;

            std::string getServerStartTime( void ) const;
            const size_t& getNickMaxLength( void ) const;

            std::deque<Channels *> getMissingChannelOps( const int& count, const std::string& nickNameExclude ) const;
            Channels* getChannel( const std::string& chanName ) const;
            Channels* addChannel( const std::string& chanName );
            void delChannel( const std::string& chanName );
            std::vector<std::pair<std::string, unsigned char> > getChannelClient( const std::string& chanName ) const;
            std::vector<Clients *> getChannelList( const std::string& chanName ) const;
            std::vector<Clients *> getChannelList( Clients* client ) const;


        protected:

        private:
            size_t m_nickNameMaxLength;
            ServerData& m_sData;

            std::vector<Channels *>::iterator searchChan( std::string chanName ) const;

    };
}
#endif
