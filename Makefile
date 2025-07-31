NAME    := miniRT

INCLUDES := -Iassets/libft -Iassets/MLX42/include
MLX42 := assets/MLX42/build/libmlx42.a
HEADERS := src/minirt.h assets/libft/libft.h assets/MLX42/include/MLX42/MLX42.h

CC      := clang
CFLAGS  := -Wall -Wextra -Werror -MMD -MP -O3 -flto -march=native -ffast-math
CFLAGS  += -Iincl -Iassets/libft -Iassets/MLX42/include -g
LDFLAGS := -ldl -lglfw -pthread -lm -flto

INCLUDES := -Iassets/libft -Iassets/MLX42/include

SRCS := \
	src/array_utils.c \
	src/assert.c \
	src/camera.c \
	src/cleanup.c \
	src/cylinder.c \
	src/input.c \
	src/lighting.c \
	src/loop.c \
	src/main.c \
	src/math.c \
	src/object.c \
	src/paraboloid.c \
	src/parse_light_camera.c \
	src/parsing_objects.c \
	src/plane.c \
	src/quaternion.c \
	src/random.c \
	src/sphere.c \
	src/texturing.c \
	src/threads.c \
	src/title.c \
	src/trace.c \
	src/vec3_arithmetic.c \
	src/vec3_geometric.c \
	src/vec3_utility.c \
	src/vec_parse_utils.c \

OBJ    := $(SRCS:src/%.c=.build/%.o)

LIBFT_DIR := assets/libft
LIBFT_A   := $(LIBFT_DIR)/libft.a

all: $(NAME)

$(MLX42):
	echo "✨ Building MLX42..."
	cmake -S assets/MLX42 -B assets/MLX42/build > /dev/null 2>&1
	make -C assets/MLX42/build -j4 > /dev/null 2>&1

$(NAME): $(OBJ) assets/libft/libft.a $(MLX42)
	echo "🔗 Linking $(NAME)..."
	$(CC) $(LDFLAGS) $^ -o $@ -o $@
	echo "🎉 Build complete!"

.build/%.o: src/%.c | .build
	@$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT_A):
	echo "📚 Building libft..."
	$(MAKE) --no-print-directory -C $(LIBFT_DIR)


.build:
	mkdir -p .build

clean:
	echo "🧹 Cleaning..."
	rm -rf .build assets/MLX42/build
	$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean

fclean: clean
	echo "🗑️ Removing $(NAME)"
	rm -f $(NAME)
	$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean

re:
	echo "🔄 Rebuilding..."
	$(MAKE) --no-print-directory fclean
	$(MAKE) --no-print-directory all

norm:
	norminette src | awk '\
		/^.*: Error!/ { file = $$1; seen = 0; next } \
		/Error:/ && $$0 !~ /INVALID_HEADER/ { \
			if (!seen) { print file ": Error!"; seen = 1 } \
			print \
		}'

start: all
	./$(NAME) maps/subject.rt

.SILENT:
.PHONY: all clean fclean re norm start

-include $(OBJ:%.o=%.d)
