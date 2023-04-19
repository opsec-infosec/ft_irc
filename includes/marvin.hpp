/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   marvin.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 21:41:48 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/14 19:09:59 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MARVIN_HPP
# define MARVIN_HPP

# include "clients.hpp"
# include "channels.hpp"
# include "commandWrapper.hpp"
# include "libft.hpp"
# include "serverData.hpp"
# include <string>
# include <cstring>
# include <cstdlib>
# include <iostream>
# include <vector>
# include <deque>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>

namespace irc {

    class Marvin {
        public:
            Marvin( const std::string& nickName, const std::string& ipAddress, const int& port );
            ~Marvin( );

            class MarvinFellDown : public std::runtime_error {
                public:
                    MarvinFellDown( const std::string& ex ) : std::runtime_error( ex ) { }
            };

            void talk( RuntimeData* rtd );

        private:
            typedef enum t_status { OFFILINE, CONNECT, REGISTER, ONLINE } e_status;
            std::deque<std::vector<std::string> >m_msgQR;
            std::deque<std::pair<std::string,std::string> > m_prevChannels; //first = channel, second = client who asked for help
            std::string m_rawBuffer;
            const std::string m_nickName;
            e_status m_registered;
            RuntimeData* m_rtd;
            Clients* m_client;
            Logger m_log;
            int m_fd;

            // Main execution of Marvin where we parse messages
            void executeCmd( const std::string& source, const std::string& target, const std::string& command, const std::vector<std::string>& params, const std::string& trailing );

            // Helper functions to parse the messages
            std::string getSource( void ) const;
            std::string getTarget( void ) const;
            std::string getCmd( void ) const ;
            std::string getTrailing( void ) const;
            std::vector<std::string> getParams( void ) const;
            std::vector<std::string> splitMessage( const std::string& val );

            // Client operations for connecting, disconnecting and send and receive
            void connectBot( const std::string& ipAddress = "127.0.0.1", const int& port = 6667 );
            void disconnectBot( void );
            void sendMsg( const std::string& reply );
            void recvMsg( void );

    };

}

#endif
