# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/15 11:05:26 by maoliiny          #+#    #+#              #
#    Updated: 2025/06/10 20:18:35 by maoliiny         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

SRCS = ft_atoi.c ft_isalpha.c ft_itoa.c ft_memmove.c ft_putnbr_fd.c  ft_strdup.c    ft_strlcpy.c  ft_strnstr.c \
	   ft_tolower.c  ft_bzero.c    ft_isascii.c  ft_memchr.c  ft_memset.c      ft_putstr_fd.c  ft_striteri.c  \
	   ft_strlen.c   ft_strrchr.c  ft_toupper.c ft_calloc.c   ft_isdigit.c  ft_memcmp.c  ft_putchar_fd.c  \
	   ft_split.c      ft_strjoin.c   ft_strmapi.c  ft_strtrim.c  ft_isalnum.c  ft_isprint.c  ft_memcpy.c \
	   ft_putendl_fd.c  ft_strchr.c     ft_strlcat.c   ft_strncmp.c  ft_substr.c   get_next_line.c \
	   get_next_line_utils.c printf_fd.c printf_fd_utils.c \
	   ft_lstsize_bonus.c ft_lstlast_bonus.c ft_lstdelone_bonus.c \
	   ft_lstclear_bonus.c ft_lstiter_bonus.c ft_strtod.c ft_strcmp.c ft_str_is_whitespace.c

OBJDIR = .build
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
.SECONDARY : $(OBJS)
CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f
.PHONY: all clean fclean re

all: $(NAME)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -I. -c $< -o $@

$(OBJDIR):
	@mkdir -p .build

$(NAME): $(OBJS) libft.h
	@ar rcs $(NAME) $(OBJS)

clean:
	@$(RM) -rf $(OBJDIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all
