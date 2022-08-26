#include "../../include/minishell.h"

int	execute_command(t_process *process)
{
	char	*command;
	char	**argv_curr;

	if (is_accessable_command(process->cmd_line, g_minishell_info.ps_list->paths))
		command = get_accessable_command(process->cmd_line, g_minishell_info.ps_list->paths);
	else
		ft_error_exit("command not found");
	return (execve(command, process->argv, process->envp));
}

int	execute_process(t_process *process, t_pipes *pipes)
{
	safe_dup2(pipes->prev_pipe[READ], STDIN_FILENO);
	safe_dup2(pipes->next_pipe[WRITE], STDOUT_FILENO);
	safe_close_pipes(pipes);
	if (apply_redirections(process->cmd_line) == FAILURE)
		return (FAILURE);
	if (is_built_in(process))
		exit(execute_built_in(process));
	else
		execute_command(process);
	exit(EXIT_FAILURE);
}

int	execute_pipeline(void)
{
	int			idx;
	t_process	*ps_curr;

	idx = 0;
	ps_curr = g_minishell_info.ps_list;
	init_pipe(&g_minishell_info.pipes);
	while (ps_curr)
	{
		swap_pipe(&g_minishell_info.pipes);
		if (ps_curr->next != NULL)
		{
			if (pipe(g_minishell_info.pipes.next_pipe) == -1)
				ft_error_exit("fail_pipe()");
		}
		ps_curr->pid = fork();
		if (ps_curr->pid == -1)
			ft_error_exit("fail fork()\n");
		else if (ps_curr->pid == 0)
			execute_process(ps_curr, &(g_minishell_info.pipes));
		else
		{
			safe_close_pipe(&g_minishell_info.pipes.prev_pipe[READ]);
			safe_close_pipe(&g_minishell_info.pipes.next_pipe[WRITE]);
		}
		ps_curr = ps_curr->next;
	}
	return (wait_childs());
}

int	execute_single_cmdline(void)
{
	pid_t		pid;
	t_process	*process;

	process = g_minishell_info.ps_list;
	if (apply_redirections(process->cmd_line) == FAILURE)
		return (EXIT_FAILURE);
	if (is_built_in(process))
		return (execute_built_in(process));
	pid = fork();
	if (pid == -1)
		ft_error_exit("fork error");
	else if (pid == 0)
		execute_command(process);//exit
	else
	{
		// free(g_minishell_info.last_status);
		// g_minishell_info.last_status = ft_itoa(wait_child(pid));
		// return (ft_atoi(g_minishell_info.last_status));
		return (wait_child(pid));
	}
	return (EXIT_FAILURE);
}

void	executor(void)
{
	int	status;

	if (execute_heredoc())
		return ;
	if (g_minishell_info.ps_list->size == 1)
		status = execute_single_cmdline();
	else
		status = execute_pipeline();
	free(g_minishell_info.last_status);
	g_minishell_info.last_status = ft_itoa(status);
	restore_stdio();
}
