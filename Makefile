# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cofische <cofische@student.42london.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 01:20:10 by ssottori          #+#    #+#              #
#    Updated: 2025/06/03 08:55:02 by cofische         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ======================= COLORS ========================
MAKEFLAGS += --silent
GREEN = \033[1;32m
RED = \033[1;31m
BLUE = \033[1;34m
NC = \033[0m

# ======================= COMPILER ======================
NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

# ======================= DIRS/FILES ====================
SRC_DIR = SRC
OBJ_DIR = OBJ
INC_DIR = INC/utils
CGI_INC = CGI/inc
CGI_SRC = CGI/src

SRC = $(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Autoindex.cpp \
	  $(SRC_DIR)/Socket.cpp \
	  $(SRC_DIR)/Location.cpp \
	  $(SRC_DIR)/ServerManager.cpp \
	  $(SRC_DIR)/HTTPRequest.cpp \
	  $(SRC_DIR)/Content.cpp \
	  $(SRC_DIR)/Client.cpp \
	  $(SRC_DIR)/HTTPResponse.cpp \
	  $(SRC_DIR)/Utils.cpp \
	  $(SRC_DIR)/main.cpp 

CGI_SRC_FILES = CGI/cgi_test.cpp \
	  $(CGI_SRC)/CgiHandler.cpp \
	  $(CGI_SRC)/executeScript.cpp \
	  $(CGI_SRC)/prepEnv.cpp \
	  $(CGI_SRC)/receiveRequest.cpp \
	  $(CGI_SRC)/returnOutput.cpp \

INC = $(INC_DIR)/Colors.hpp \
	  $(INC_DIR)/Webserver.hpp \
	  $(INC_DIR)/Socket.hpp \
	  $(INC_DIR)/Server.hpp \
	  $(INC_DIR)/Location.hpp \
	  $(INC_DIR)/Client.hpp \
	  $(INC_DIR)/MET.hpp \
	  $(INC_DIR)/HTTPRequest.hpp \
	  $(INC_DIR)/HTTPResponse.hpp \
	  $(INC_DIR)/Content.hpp \
	  $(INC_DIR)/Utils.hpp \
	  $(INC_DIR)/ServerManager.hpp \
	  $(CGI_INC)/CgiHandler.hpp \
	  $(CGI_INC)/executeScript.hpp \
	  $(CGI_INC)/prepEnv.hpp \
	  $(CGI_INC)/receiveRequest.hpp \
	  $(CGI_INC)/returnOutput.hpp \

CGI_OBJ = $(CGI_SRC_FILES:$(CGI_SRC)/%.cpp=$(OBJ_DIR)/CGI/%.o)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) $(CGI_OBJ)

# ======================= RULES =========================
all: $(NAME)

$(NAME): banner $(OBJ) $(CGI_OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@echo "${BLUE}===============================${NC}"
	@echo "${NC}"
	@echo "[${GREEN}WEBSERV${NC}] Executable ${BLUE}$(NAME)${NC} created!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(CGI_INC) -c $< -o $@
	@echo "[${GREEN}webserv${NC}] Compiling... $<"

$(OBJ_DIR)/CGI/%.o: $(CGI_SRC)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(CGI_INC) -c $< -o $@
	@echo "[${GREEN}webserv${NC}] Compiling... $<"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "[${RED}webserv${NC}] Object files removed."

fclean: clean
	@$(RM) $(NAME)
	@echo "[${RED}webserv${NC}] Executable $(NAME) removed."

re: fclean all

leaks:
	valgrind --leak-check=full ./$(NAME)

banner:
	@echo "${BLUE}"
	@echo " ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗ "
	@echo " ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║ "
	@echo " ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║ "
	@echo " ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝ "
	@echo " ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝  "
	@echo "  ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝   "
	@echo "              BY: cofische | chuleung | sottori"
	@echo "${NC}"

.PHONY: all clean fclean re

