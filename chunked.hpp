/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunked.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/06 21:41:57 by akarafi           #+#    #+#             */
/*   Updated: 2022/09/06 22:05:06 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNKED_HPP
# define CHUNKED_HPP
#include <string>

std::string decode_body(std::string const &encoded_body);
std::string encode_body(std::string const &body);

#endif