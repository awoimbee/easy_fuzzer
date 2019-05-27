# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/27 16:31:48 by awoimbee          #+#    #+#              #
#    Updated: 2019/05/27 16:53:10 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CPPFLAGS = -std=c++14 -O3 -Wall -Wextra -Wpedantic
NAME = donateToAwoimbee
SRCS = inputFileDestroyer.cpp

all :
	@printf "I accept donations from wire transferts, cash, Paypal, bitcoin, ethereum, all sorts of opioids, anything really\n(my Paypal: http://paypal.me/awoimbee)\n"
	@$(MAKE) $(NAME) --no-print-directory

$(NAME): $(SRCS)  # the project is only 1 .cpp files so compiling a single .o file and then linking is useless
	$(CXX) $(CPPFLAGS) $(SRCS) -o $(NAME)

fclean:
	rm $(NAME)

re: fclean all

.PHONY: allf clean re
