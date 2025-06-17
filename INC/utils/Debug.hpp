/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 09:20:31 by cofische          #+#    #+#             */
/*   Updated: 2025/06/17 09:25:16 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifdef DEBUG
    #define DEBUG_PRINT(msg) do { \
        std::cout << "[DEBUG][" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl; \
    } while(0)
#else
    #define DEBUG_PRINT(msg)
#endif

/*
__FILE__ => special macro for preprocessor that will replace this word with the current source file name
__LINE__ => same as above with the line number
do{} while {} => ensure the macro is treated as one statement only 

*/