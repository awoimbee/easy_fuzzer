# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: awoimbee <awoimbee@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/27 16:31:48 by awoimbee          #+#    #+#              #
#    Updated: 2019/05/31 11:26:39 by awoimbee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = g++
CPPFLAGS = -std=c++14 -O3 -Wall -Wextra -Wpedantic
NAME = fuzzer
SRCS = fuzzer.cpp

all : $(NAME)

$(NAME): $(SRCS)  # the project is only 1 .cpp files so compiling a single .o file and then linking is useless
	$(CXX) $(CPPFLAGS) $(SRCS) -o $(NAME)

fclean:
	rm $(NAME)

re: fclean all

.PHONY: allf clean re
