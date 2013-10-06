
SRC_M		=	memo_test.cpp

SRC_S		=	sergio_test1.cpp

CXX			=	g++

CFLAGS		=	-W -Wall -Werror

LDLIBS		=	-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LDFLAGS		=	$(LDLIBS)

NAME_M		=	memo_test

NAME_S		=	sergio_test

OBJS_M		=	$(SRC_M:.cpp=.o)

OBJS_S		=	$(SRC_S:.cpp=.o)

RM			=	rm -f

all:			$(NAME_M) $(NAME_S)

$(NAME_M):		$(OBJS_M)
				$(CXX) $(CFLAGS) $(OBJS_M) $(LDFLAGS) -o $(NAME_M)

$(NAME_S):		$(OBJS_S)
				$(CXX) $(CFLAGS) $(OBJS_S) $(LDFLAGS) -o $(NAME_S)

clean:			
				$(RM) $(OBJS_M) $(OBJS_S)

fclean:			clean
				$(RM) $(NAME_M) $(NAME_S)

re:				clean all

.PHONY:			all clean fclean re
