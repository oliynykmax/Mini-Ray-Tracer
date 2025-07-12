# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/09 19:29:32 by maoliiny          #+#    #+#              #
#    Updated: 2025/07/11 22:56:31 by maoliiny         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    := miniRT

INCLUDES := -Iassets/libft -Iassets/MLX42/include
MLX42 := assets/MLX42/build/libmlx42.a 
HEADERS := src/minirt.h assets/libft/libft.h assets/MLX42/include/MLX42/MLX42.h

CC      := cc
CFLAGS  := -Wall -Wextra -Werror -MMD -MP -O3 -march=native -ffast-math
CFLAGS  += -Iincl -Iassets/libft -Iassets/MLX42/include
LDFLAGS := -ldl -lglfw -pthread -lm

INCLUDES := -Iassets/libft -Iassets/MLX42/include

SRCS    := src/input.c \
		   src/main.c \
		   src/assert.c \
       	src/loop.c \
        src/vec3_arithmetic.c \
        src/vec3_geometric.c \

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
	echo "🗑️  Removing $(NAME)"
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

.SILENT:
.PHONY: all clean fclean re norm

-include $(OBJ:%.o=%.d)
