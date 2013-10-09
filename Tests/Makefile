
SRC_M		=	memo_test.cpp

SRC_S		=	sergio_test1.cpp

SRC_PROTO	=	proto.cpp

CXX			=	g++

CFLAGS		=	-W -Wall -Werror

LDLIBS		=	-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LDFLAGS		=	$(LDLIBS)

NAME_M		=	memo_test

NAME_S		=	sergio_test

NAME_PROTO	=	prototype

OBJS_M		=	$(SRC_M:.cpp=.o)

OBJS_S		=	$(SRC_S:.cpp=.o)

OBJS_PROTO	=	$(SRC_PROTO:.cpp=.o)

RM			=	rm -f

all:			$(NAME_M) $(NAME_S) $(NAME_PROTO)

$(NAME_M):		$(OBJS_M)
				$(CXX) $(CFLAGS) $(OBJS_M) $(LDFLAGS) -o $(NAME_M)

$(NAME_S):		$(OBJS_S)
				$(CXX) $(CFLAGS) $(OBJS_S) $(LDFLAGS) -o $(NAME_S)

$(NAME_PROTO):	$(OBJS_PROTO)
				$(CXX) $(CFLAGS) $(OBJS_PROTO) $(LDFLAGS) -o $(NAME_PROTO)

clean:			
				$(RM) $(OBJS_M) $(OBJS_S) $(OBJS_PROTO)

fclean:			clean
				$(RM) $(NAME_M) $(NAME_S) $(NAME_PROTO)

re:				clean all

.PHONY:			all clean fclean re
