#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <errno.h>
# include <string.h>
# include <fcntl.h>
# include "libft.h"
# include "get_next_line.h"
# include "color.h"

char	**command_split(char *str);
int		check_white_space(char c);
int		count_split_size(char *str);
int		check_size(char *line, char **str, int *i);
void	print_wallpaper(void);

/* built-in */

int		ft_exit(int status);

/* utils */
int		is_same_string(char *str1, char *str2);

#endif