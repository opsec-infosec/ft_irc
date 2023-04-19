/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 09:54:30 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/07 12:34:05 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERDATA_HPP
# define SERVERDATA_HPP

# include <string>

# include "clients.hpp"
# include "channels.hpp"

namespace irc {

    class ServerData {
        public:
            ServerData( std::vector<Clients *>& clients, std::vector<Channels *>& channels ) : m_clients( clients ), m_channels( channels ) { };
            ~ServerData() { };

            std::vector<Clients *>& m_clients;
            std::vector<Channels *>& m_channels;
            std::string m_password;
            std::string m_startUpTime;

        private:
    };

}

#endif
