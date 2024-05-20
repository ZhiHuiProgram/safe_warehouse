CC := aarch64-linux-gnu-gcc
#CC := aarch64-none-linux-gnu-gcc

SRC := $(shell find soc -name *.c)
INC := ./inc \
	./usr/local/include \
	./usr/usr/include \
	./usr/usr/include/python3.10 \
	./usr/usr/include/aarch64-linux-gnu/python3.10/ \
	./usr/usr/include/aarch64-linux-gnu/

OBJ := $(subst soc/,obj/,$(SRC:.c=.o))

TARGET := obj/smart_home
CFLAGS := $(foreach item,$(INC),-I $(item))
LIBS_PATH := ./usr/local/lib \
	./usr/lib/aarch64-linux-gnu/ \
	./usr/usr/lib/aarch64-linux-gnu/ \
	./usr/usr/lib/python3.10/ \

LDFLAGS := $(foreach item,$(LIBS_PATH),-L $(item))
LIBS := -lwiringPi -lpython3.10 -lpthread -lexpat -lz -lcrypt
obj/%.o: soc/%.c
	mkdir -p obj
	$(CC) -c $< -o  $@ -fPIC $(CFLAGS)
$(TARGET): $(OBJ)
	$(CC)  $^ -o $@ -fPIC $(CFLAGS) $(LDFLAGS) $(LIBS)
#	scp ./obj/smart_home orangepi@192.168.31.111:/home/orangepi/yuanxueshe/smart_home/face_recg
all: $(TARGET)
clean:
	rm -rf obj/*
debug:
	echo $(SRC)
	echo $(OBJ)
	echo $(TARGET)
	echo $(CFLAGS)
	echo $(LDFLAGS)
	echo $(LIBS)
	echo $(INC)
	echo $(CC)
.PHONY: all clean debug
