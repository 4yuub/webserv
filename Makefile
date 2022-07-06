# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/20 00:06:42 by zoulhafi          #+#    #+#              #
#    Updated: 2022/07/06 23:19:48 by akarafi          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp Config.cpp Server.cpp VirtualServer.cpp Request.cpp Response.cpp CGI.cpp
DEPS = Config.hpp Server.hpp VirtualServer.hpp Request.hpp Response.hpp CGI.hpp
OBJECTS = $(SRCS:.cpp=.o)
RED = \033[1;31m
GREEN = \033[1;32m
BLUE = \033[1;34m
YELLOW = \033[1;33m
NC = \033[1;0m

all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "$(RED)Compiling $(NAME)...$(NC)"
	@echo "$(RED)Linking...$(NC)"
	@$(CC) $(FLAGS) $(OBJECTS) -o $(NAME) 
	@echo "$(GREEN)Finished linking && compiling...$(NC)"

%.o: %.cpp $(DEPS)
	@$(CC) $(FLAGS) -c -o $@ $<
	@echo "$(RED)Compiling $< ...$(NC)"

clean:
	@echo "$(RED)Cleaning objects...$(NC)"
	@rm -rf $(OBJECTS)
	@echo "$(GREEN)Cleaned objects...$(NC)"

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(NC)"
	@rm -rf $(NAME)
	@echo "$(GREEN)Cleaned $(NAME)...$(NC)"

re: fclean all
