OS_TYPE := $(shell sh -c 'uname -s')
COMO_OBJECT_LIB=libanaobject.so
COMO_OBJECT_LIB_LD_FLAGS= -shared
CFLAGS := -fPIC -I. -Wall -Wextra -g -ggdb -std=c99
LDFLAGS = -fPIC -shared
BINDIR = $(shell pwd)/../bin
COMO_OBJECT_LIB_SOURCES =arena.c function.c frame.c bool.c container.c string.c long.c double.c map.c code.c array.c
COMO_OBJECT_LIB_OBJECTS =$(COMO_OBJECT_LIB_SOURCES:.c=.o)

OBJECTS= $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

COMO_OBJECT_TARGETS = $(addprefix $(BINDIR)/, $(COMO_OBJECT_LIB_OBJECTS))

all: $(COMO_OBJECT_LIB)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@

$(COMO_OBJECT_LIB): $(COMO_OBJECT_LIB_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINDIR)/$@ $(COMO_OBJECT_TARGETS)

clean:
	rm -rf $(BINDIR)/*.so $(BINDIR)/*.o
