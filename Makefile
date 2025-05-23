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

SRC = $(SRC_DIR)/Server.cpp \
	  $(SRC_DIR)/Socket.cpp \
	  $(SRC_DIR)/Location.cpp \
	  $(SRC_DIR)/ServerManager.cpp \
	  $(SRC_DIR)/HTTPRequest_test.cpp \
	  $(SRC_DIR)/Content_test.cpp \
	  $(SRC_DIR)/Client.cpp \
	  $(SRC_DIR)/HTTPResponse.cpp \
	  $(SRC_DIR)/Utils.cpp \
	  $(SRC_DIR)/main.cpp

INC = $(INC_DIR)/Colors.hpp \
	  $(INC_DIR)/Webserver.hpp \
	  $(INC_DIR)/Socket.hpp \
	  $(INC_DIR)/Server.hpp \
	  $(INC_DIR)/Location.hpp \
	  $(INC_DIR)/Client.hpp \
	  $(INC_DIR)/MET.hpp \
	  $(INC_DIR)/HTTPRequest_copy.hpp \
	  $(INC_DIR)/HTTPResponse.hpp \
	  $(INC_DIR)/Content_copy.hpp \
	  $(INC_DIR)/Utils.hpp \
	  $(INC_DIR)/ServerManager.hpp \

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# ======================= RULES =========================
all: $(NAME)

$(NAME): banner $(OBJ)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
	@echo "${BLUE}===============================${NC}"
	@echo "${NC}"
	@echo "[${GREEN}WEBSERV${NC}] Executable ${BLUE}$(NAME)${NC} created!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@
	@echo "[${GREEN}webserv${NC}] Compiled into $@"

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

# NAME = webserv
# CXX = clang++
# CXXFLAGS = -Wall -Werror -Wextra -std=c++98
# INC_DIR = INC/utils
# SRC_DIR = SRC
# OBJ_DIR = OBJ
# SRC = $(SRC_DIR)/Server.cpp \
#       $(SRC_DIR)/Socket.cpp \
# 	  $(SRC_DIR)/Location.cpp \
# 	  $(SRC_DIR)/ServerManager.cpp \
# 	  $(SRC_DIR)/HTTPRequest_test.cpp \
# 	  $(SRC_DIR)/Content_test.cpp \
# 	  $(SRC_DIR)/Client.cpp \
# 	  $(SRC_DIR)/HTTPResponse.cpp \
# 	  $(SRC_DIR)/Utils.cpp \
#       $(SRC_DIR)/main.cpp 
# OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# INC = $(INC_DIR)/Colors.hpp \
# 	  $(INC_DIR)/Webserver.hpp \
# 	  $(INC_DIR)/Socket.hpp \
# 	  $(INC_DIR)/Server.hpp \
# 	  $(INC_DIR)/Location.hpp \
# 	  $(INC_DIR)/Client.hpp \
# 	  $(INC_DIR)/MET.hpp \
# 	  $(INC_DIR)/HTTPRequest_copy.hpp \
# 	  $(INC_DIR)/HTTPResponse.hpp \
# 	  $(INC_DIR)/Content_copy.hpp \
# 	  $(INC_DIR)/Utils.hpp \
# 	  $(INC_DIR)/ServerManager.hpp \

# all : $(NAME)

# $(NAME): $(OBJ)
# 	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)
# 	@echo "Executable $(NAME) created!"

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@mkdir -p $(OBJ_DIR)
# 	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@
# 	@echo "Compiled $< into $@"

# clean:
# 	@rm -rf $(OBJ_DIR)
# 	@echo "Object files and directories removed."

# fclean: clean
# 	@rm -f $(NAME)
# 	@echo "Executable $(NAME) removed."

# re: fclean all

# .PHONY: all clean fclean re
