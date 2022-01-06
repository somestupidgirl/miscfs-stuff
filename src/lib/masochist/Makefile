CC = cc
AR = /usr/bin/ar
TS = /usr/local/bin/ts
RM = rm -f

LIB = libmasochist.a
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
INC = 	-I../../include \
		-I../../../MacKernelSDK/Headers

CFLAGS = -Wall -Winline -pipe -O3 -ffunction-sections -fdata-sections -Bstatic $(INC)

all: $(LIB)

$(LIB):$(OBJ)
	@$(AR) rcs $@ $^
	@echo "$^" | xargs -n1 | $(TS) "AR"
	@echo "AR $@"

$(LIB): $(OBJ)
		@ar rcs $@ $^

%.o:%.c
	@$(CC) -o $@ $(CFLAGS) -c $< $(INC)
	@echo "$@" | xargs -n1 | $(TS) "CC"

clean:
	@$(RM) $(OBJ) $(LIB)
	@echo "$(OBJ)" | xargs -n1 | $(TS) "clean"
