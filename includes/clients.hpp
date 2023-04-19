/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:11:08 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:36:53 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTS_HPP
# define CLIENTS_HPP
# define BUFFSIZE 4095
# include "logger.hpp"
# include "libft.hpp"

# include <string>
# include <cstdio>
# include <algorithm>
# include <deque>
# include <list>
# include <sys/socket.h>
# include <netinet/in.h>

namespace irc {
    class Clients {
        public:
            Clients( );
            Clients( const int& fd, const std::string& address );
            ~Clients( );

            size_t receive( void );

            // Getters / Setters Specific to Server Operations
            int getFD( void );
            std::deque<std::vector<std::string> >& getMsgQR( void );
            std::deque<std::string>& getMsgQS( void );
            void setMsgQS( std::string msg );

            // Getters Specific to Clients
            bool isAuthenticated( void ) const;
            bool isRegistered( void ) const;
            bool isOperator( void ) const;
            bool isMarkedForDeletion( void ) const;
            bool isMarkedForKill( void ) const;
            int& getClientAttempts( void );
            const std::string getLastJoinChan( void ) const;
            const std::vector<std::string> getJoinChan( void ) const;
            const std::string getNickName( void ) const;
            const std::string getUserName( void ) const;
            const std::string getRealName( void ) const;
            const std::string getHostName( void ) const;
            const std::string getConnHostName( void ) const;
            unsigned char getMode( void ) const;
            int getChannelCount( void ) const;

            // Setters Specific to Clients
            void addJoinChan( const std::string& chanName );
            void delJoinChan( const std::string& chanName );
            void setNickName( const std::string& nick );
            void setUserName( const std::string& userName );
            void setRealName( const std::string& realName );
            void setHostName( const std::string& hostName );
            void setConnHostName( const std::string& hostName );
            void setAuthenticated( bool isAuthenticated );
            void setRegisterd( bool isRegistered );
            void setMode( unsigned char mode );
            void markForDeletion( int fd );
            void markForKill( int fd );

        private:
            int m_fd;
            std::string m_address;
            std::string m_rawBuffer;

            // Specific to Clients
            int m_clientAttempts;
            unsigned char m_mode;
            bool m_isAuthenticated;
            bool m_isRegistered;
            bool m_markForDeletion;
            bool m_markForKill;
            std::list<std::string> m_channelJoined;
            std::string m_nickName;
            std::string m_userName;
            std::string m_realName;
            std::string m_hostName;
            std::string m_connectedHostName;

            std::deque<std::vector<std::string> > m_msgQR;
            std::deque<std::string> m_msgQS;

    };
}

#endif
