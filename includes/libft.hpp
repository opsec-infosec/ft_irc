/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:11:27 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/07 09:06:45 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_HPP
# define LIBFT_HPP
# include <string>
# include <sstream>
# include <limits>
# include <exception>
# include <cstdlib>
# include <queue>
# include <ctime>
# include <algorithm>

namespace libft {
    // @brief  Converts Type T to std::string
    // @param[in] val Converts const type T val to std::string
    // @returns[out] std::string
    // @exceptsafe no-throw
    template <typename T>
    std::string to_string( const T val ) {
        std::stringstream ss;

        ss << val;
        return ss.str();
    };

    int stoi( const std::string& val );
    long stol( const std::string& val );
    std::string to_upper( const std::string& val );
    std::queue<std::string> splitStr( std::string& val, const std::string& delim );
    std::vector<std::string> splitSpace( const std::string& val );
    std::string getCurTimeString( void );
    std::string getCurUnixTimeString( void );
    bool isalnumStr( const std::string& val );

}

#endif
