
SRC_M		=	memo_test.cpp

SRC_S		=	sergio_test1.cpp

SRC_PROTO	=	proto.cpp

SRC_RELEVANCE	=	relevanceVectors.cpp

CXX			=	g++

CFLAGS		=	-W -Wall -Werror

#LDLIBS		=	-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_features2d
LDLIBS		=	 -lopencv_core -lopencv_flann -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_objdetect -lopencv_photo -lopencv_nonfree -lopencv_features2d -lopencv_calib3d -lopencv_legacy -lopencv_contrib -lopencv_stitching -lopencv_videostab -lopencv_gpu -lopencv_ts	

LDFLAGS		=	$(LDLIBS)

NAME_M		=	memo_test

NAME_S		=	sergio_test

NAME_PROTO	=	prototype

NAME_RELEVANCE	=	relevance

OBJS_M		=	$(SRC_M:.cpp=.o)

OBJS_S		=	$(SRC_S:.cpp=.o)

OBJS_PROTO	=	$(SRC_PROTO:.cpp=.o)

OBJS_RELEVANCE	=	$(SRC_RELEVANCE:.cpp=.o)

RM			=	rm -f

all:			$(NAME_M) $(NAME_S) $(NAME_PROTO) $(NAME_RELEVANCE)

$(NAME_M):		$(OBJS_M)
				$(CXX) $(CFLAGS) $(OBJS_M) $(LDFLAGS) -o $(NAME_M)

$(NAME_S):		$(OBJS_S)
				$(CXX) $(CFLAGS) $(OBJS_S) $(LDFLAGS) -o $(NAME_S)

$(NAME_PROTO):		$(OBJS_PROTO)
				$(CXX) $(CFLAGS) $(OBJS_PROTO) $(LDFLAGS) -o $(NAME_PROTO)

$(NAME_RELEVANCE):	$(OBJS_RELEVANCE)
				$(CXX) $(CFLAGS) $(OBJS_RELEVANCE) $(LDFLAGS) -o $(NAME_RELEVANCE)
clean:			
				$(RM) $(OBJS_M) $(OBJS_S) $(OBJS_PROTO) $(OBJS_RELEVANCE)

fclean:			clean
				$(RM) $(NAME_M) $(NAME_S) $(NAME_PROTO) $(NAME_RELEVANCE)

re:				clean all

.PHONY:			all clean fclean re
