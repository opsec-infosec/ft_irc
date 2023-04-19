/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/31 17:56:11 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/07 15:28:28 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_NOTONCHANNEL (442)
    ERR_CHANOPRIVSNEEDED (482)
    RPL_NOTOPIC (331)
    RPL_TOPIC (332)
    RPL_TOPICWHOTIME (333)
*/

// @brief Topic command to get or set the current topic of a channel
Topic::Topic( ) : ICommand( ) {

}

Topic::~Topic( ) {

}

void Topic::cmdExecute( void ) {
    Clients* client;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }

    std::string trailing = getTrailing();

    topic( client, params.front(), trailing );
}

// @brief Sets the topic of the channel, checks if correct ChanOps
// @params[in] client the clinet making the request
// @params[in] chanName The name of the channel to change the topic
// @params[in] topic The topic to change
// @exceptsafe no-throw
void Topic::topic( const Clients* client, const std::string& chanName, const std::string& topic ) {
    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch( ICommandException& ex) { return ; }

    Channels* channel;
    try { channel = rtd->getChannel( chanName ); }
    catch( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHCHANNEL, chanName ); }
    catch ( Channels::NotFoundException& ex ) { return cmdError( ERR_NOSUCHCHANNEL, chanName ) ; }

    if ( topic.empty() ) {
        std::string cTopic = channel->getTopic();

        if ( cTopic.empty() ) {
            return sendReply( RPL_NOTOPIC, "No topic is set", client->getNickName(), chanName );
        }

        return sendReply( RPL_TOPIC, cTopic, client->getNickName(), chanName );
    }

    std::vector<Clients *> cClients;
    try {
        channel->setTopic( client, topic );
        cClients = rtd->getChannelList( chanName );
    }
    catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOTONCHANNEL, chanName); }
    catch ( Channels::NotFoundException& ex ) {  return cmdError( ERR_NOTONCHANNEL, chanName ); }
    catch ( Channels::ModeViolationException& ex ) { return cmdError( ERR_CHANOPRIVSNEEDED, chanName ); }


    std::pair<std::string, std::string> topicWho = channel->getTopicWho();

    sendReply( RPL_TOPIC, topic, client->getNickName(), chanName );
    sendReply( RPL_TOPICWHOTIME, "", client->getNickName(), chanName + " " + topicWho.first + " " + topicWho.second );
    sendReply( client->getNickName(), "TOPIC", channel->getTopic(), chanName );

    sendReplyToList( cClients, RPL_TOPIC, topic, client->getNickName(), chanName, libft::to_string( SERVER ) );
    sendReplyToList( cClients, client, libft::to_string( RPL_TOPICWHOTIME ), chanName  + " " + topicWho.first + " " + topicWho.second );
    sendReplyToList( cClients, client->getNickName(), "TOPIC", channel->getTopic(), chanName );
}
