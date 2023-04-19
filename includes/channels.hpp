/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 11:16:11 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 01:48:04 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# define DEFAULT_OP_MODE 0
# define DEFAULT_CHAN_MODE 256 | 512

# include "clients.hpp"

# include <iostream>
# include <map>

namespace irc {
    class Channels {
        public:
            Channels( const std::string& channelName );
            ~Channels( );

            class NotFoundException : public std::runtime_error {
                public:
                    NotFoundException( const std::string& ex ) : std::runtime_error( ex ){ }
            };

            class ModeViolationException : public std::runtime_error {
                public:
                    ModeViolationException( const std::string& ex ) : std::runtime_error( ex ) { }
            };

            // Getters Specific to Channel Ops
            const std::string& getName( void ) const;
            const std::string& getTopic( void ) const;
            std::pair<std::string, std::string> getTopicWho( void ) const;
            const std::string& getKey( void ) const;
            size_t getClientCount( void ) const;
            unsigned short int getChanMode( void ) const;
            unsigned char getChanOpMode( const Clients* client ) const;
            const std::vector<std::pair<Clients*, unsigned char> > getChannelList( void ) const ;
            const std::vector<Clients *> getClientList( void ) const;

            // Setters Specific to Channel Ops
            void setTopic( const Clients* client, const std::string& topic );
            void setKey( std::string key );
            void setChanMode( unsigned short int mode );
            void setChanOpMode( Clients* client, unsigned char mode );
            void setChanLimit( size_t size );

            // Methods Specific to Clients
            bool isClientInChannel( const Clients* client ) const;
            void addClientInvite( Clients* client );
            void addClient( Clients* client );
            void delClient( Clients* client );
            void chanMsg( Clients* client, std::string source, std::string command, std::string trailing, std::string target, std::string operation = "" );
            void sendQuitMsg( Clients* client, std::string reason = "leaving" ) const;

        private:
            std::vector<std::pair<Clients*, unsigned char> > m_channelClients;
            std::vector<std::string> m_inviteList;
            size_t m_channelLimit;
            std::string m_name;
            std::string m_topic;
            std::string m_whoTopic;
            std::string m_timestamp;
            std::string m_key;
            unsigned short int m_mode;
            Logger m_log;

            Clients* getClient( std::string nickName ) const;
            std::vector<std::pair<Clients *, unsigned char> >::iterator searchClient( Clients* client );
    };
}
#endif
