BUILD_DIR := build

NAME := chat
SRCS := main.c common.c server.c client.c sockutil.c alist.c termin.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))
CLIBS := -pthread -lm
CC := gcc

.PHONY: $(NAME) install
$(NAME): $(BUILD_DIR)/$(NAME)

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.

# CREATING THE BUILD DIRECTORY
$(BUILD_DIR)/.:
	mkdir -p $@

.SECONDEXPANSION:
	
# COMPILING SOURCE FILES TO OBJECTS
$(BUILD_DIR)/%.o: src/%.c | $$(@D)/.
	$(CC) -c $< -o $@

$(BUILD_DIR)/%.o: src/utils/%.c | $$(@D)/.
	$(CC) -c $< -o $@

$(BUILD_DIR)/%.o: ./%.c | $$(@D)/.
	$(CC) -c $< -o $@
	
# LINKING THE OBJECTS INTO AN EXECUTABLE
$(BUILD_DIR)/$(NAME): $(OBJS)
	$(CC) $^ -o $@ $(CLIBS)

clean:
	rm -rf ./$(BUILD_DIR)/*.o