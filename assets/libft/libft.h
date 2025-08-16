/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 17:53:10 by maoliiny          #+#    #+#             */
/*   Updated: 2025/08/16 11:09:44 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

/*void				ft_lstadd_front(t_list **lst, t_list *new);*/
int					ft_lstsize(t_list *lst);
t_list				*ft_lstlast(t_list *lst);
/*void				ft_lstadd_back(t_list **lst, t_list *new);*/
void				ft_lstdelone(t_list *lst, void (*del)(void *));
void				ft_lstclear(t_list **lst, void (*del)(void *));
void				ft_lstiter(t_list *lst, void (*f)(void *));
t_list				*ft_lstmap(t_list *lst, void *(*f)(void *),
						void (*del)(void *));

int					ft_isalpha(int c);
int					ft_isdigit(int c);
int					ft_isalnum(int c);
int					ft_isascii(int c);
int					ft_isprint(int c);
int					ft_toupper(int c);
int					ft_tolower(int c);

void				*ft_memset(void *s, int c, size_t n);
void				ft_bzero(void *s, size_t n);
void				*ft_memcpy(void *dst, const void *src, size_t n);
void				*ft_memmove(void *dst, const void *src, size_t n);
void				*ft_calloc(size_t nmemb, size_t size);
void				*ft_memchr(const void *s, int c, size_t n);
int					ft_memcmp(const void *s1, const void *s2, size_t n);

size_t				ft_strlcpy(char *dst, const char *src, size_t dstsize);
size_t				ft_strlcat(char *dst, const char *src, size_t dstsize);
char				*ft_strchr(const char *s, int c);
char				*ft_strrchr(const char *s, int c);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
char				*ft_strnstr(const char *big, const char *little,
						size_t len);
char				*ft_strdup(const char *s1);
size_t				ft_strlen(const char *s);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strtrim(char const *s1, char const *set);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void				ft_striteri(char *s, void (*f)(unsigned int, char *));
int					ft_str_is_whitespace(const char *str);

long				ft_atoi(const char *nptr);
int					ft_atoi_safe(const char *nptr, long *result);
char				**ft_split(char const *s, char c);
char				*ft_itoa(int n);

void				ft_putchar_fd(char c, int fd);
void				ft_putstr_fd(char *s, int fd);
void				ft_putendl_fd(char *s, int fd);
void				ft_putnbr_fd(int n, int fd);

int					ft_printf(const char *s, ...);

int					printf_fd(int fd, const char *s, va_list *args);
int					ft_fprintf(int fd, const char *s, ...);
int					choose_type_fd(va_list *args, const char **s, int fd);
int					printf_fd_c(char c, int fd);
int					printf_fd_s(char *s, int fd);
int					printf_fd_p(void *ptr, int fd);
int					printf_fd_pnum(unsigned long long un, char *base, int fd);
int					printf_fd_diux(long un, char *base, int fd);
int					write_reverse_fd(const char *buffer, int len, int fd);
int					ft_strcmp(const char *s1, const char *s2);

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 9000
# endif

# ifndef FD_NUM
#  define FD_NUM 1024
# endif

char				*ft_strncpy(const char *src, int n);
char				*extract_line(char **res);
char				*get_next_line(int fd);
int					ft_fnl(char *s, char c);
size_t				ft_strlen(const char *s);
int					ft_strcat(char **res, int fd);
char				*ft_strjoin(char const *s1, char const *s2);
void				*ft_memcpy_gnl(void *dest, const void *src, size_t n);

double				ft_strtod(const char *str, char **endptr);

#endif
