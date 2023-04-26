LDFLAGS+=-lpthread

SRC=main.c media_lib.c thr_list.c
TARGET=server

$(TARGET) : $(SRC)
	gcc -o $@ $^ $(LDFLAGS)

clean:
	rm $(TARGET)


