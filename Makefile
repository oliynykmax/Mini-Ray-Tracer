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
CFLAGS  := -Wall -Wextra -Werror -O3 -march=native -ffast-math

INCLUDES := -Iassets/libft -Iassets/MLX42/include

SRCS    := src/input.c \
		   src/main.c \
		   src/assert.c \

OBJ    := $(SRCS:src/%.c=.build/%.o)

LIBFT_DIR := assets/libft
LIBFT_A   := $(LIBFT_DIR)/libft.a


all: $(NAME)

$(MLX42):
	@echo "✨ mlx is compiling..."
	@cmake -S assets/MLX42 -B assets/MLX42/build > /dev/null 2>&1
	@make -C assets/MLX42/build -j4 > /dev/null 2>&1

$(NAME): $(OBJ) assets/libft/libft.a $(MLX42)
	@echo "🔗 Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) assets/libft/libft.a $(MLX42) -ldl -lglfw -pthread -lm -o $(NAME)
	@echo "🎉 Build complete!"

.build/%.o: src/%.c $(HEADERS) | .build
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT_A):
	@echo "📚 Building libft..."
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR)


.build:
	@mkdir -p .build

clean:
	@echo "🧹 Cleaning..."
	@rm -rf .build assets/MLX42/build
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean

fclean: clean
	@echo "🗑️  Removing $(NAME)"
	@rm -f $(NAME)
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean

re:
	@echo "🔄 Rebuilding..."
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

.PHONY: all clean fclean re
