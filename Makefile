NAME  = main

SRC = Configfile.cpp Server.cpp location.cpp main.cpp

CC = c++

all : $(NAME)


$(NAME) : 
	$(CC) $(SRC) -o $(NAME)

clean : 
	rm -rf $(NAME)


re : clean all