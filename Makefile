CC = gcc


CFLAGS = -Wall -Wextra -g

TARGET = game


SRCS = main.c player.c enemy.c game.c items.c utils.c save_game.c


OBJS = $(SRCS:.c=.o)

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(TARGET) $(OBJS)


.PHONY: all clean 