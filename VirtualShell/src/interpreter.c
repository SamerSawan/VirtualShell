#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shellmemory.h"
#include "shell.h"
#include "scheduler.h"

int MAX_ARGS_SIZE = 8;

int help();
int quit();
int badcommand();
int my_ls();
int set(char *var, char *value[], int tokens);
int print(char *var);
int echo(char *var);
int run(char *script);
int badcommandFileDoesNotExist();
int badcommandTooManyTokens();
int exec(char *args[], int size);
int resetmem();

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size)
{
	int i;

	if (args_size < 1 || args_size > MAX_ARGS_SIZE)
	{
		return badcommand();
	}

	for (i = 0; i < args_size; i++)
	{ // strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help") == 0)
	{
		// help
		if (args_size != 1)
			return badcommand();
		return help();
	}
	else if (strcmp(command_args[0], "quit") == 0)
	{
		// quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}
	else if (strcmp(command_args[0], "set") == 0)
	{
		// set
		if (args_size < 3)
			return badcommand();
		if (args_size > 7)
			return badcommandTooManyTokens();
		return set(command_args[1], (command_args + 2), (args_size - 2));
	}
	else if (strcmp(command_args[0], "print") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}
	else if (strcmp(command_args[0], "echo") == 0)
	{
		return echo(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_ls") == 0)
	{
		return my_ls();
	}
	else if (strcmp(command_args[0], "run") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return run(command_args[1]);
	}
	else if (strcmp(command_args[0], "exec") == 0)
	{
		if (args_size < 3)
			return badcommand();
		if (args_size > 5)
			return badcommandTooManyTokens();
		return exec(command_args + 1, args_size - 1);
	}
	else
		return badcommand();
}
int x=XVAL;
int y=YVAL;

int help()
{

	char help_string[1024];
	snprintf(help_string, sizeof(help_string), "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n \
echo $VAR       Displays the STRING assigned to VAR\n \
echo STRING     Displays the STRING\n \
my_ls           Displays all files in the current directory\n \
Frame Store Size = %i Variable Store Size = %i", x, y);


	printf("%s\n", help_string);
	return 0;
}

int quit()
{
	printf("%s\n", "Bye!");
	system("rm -rf backingstore");
	exit(0);
}

int badcommand()
{
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist()
{
	printf("%s\n", "Bad command: File not found");
	return 3;
}
int badcommandTooManyTokens()
{
	printf("%s\n", "Bad command: Too many tokens.");
	return 1;
}

int set(char *var, char *value[], int tokens)
{

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	for (int i = 0; i < tokens; i++)
	{
		strcat(buffer, value[i]);
	}
	char temp[1000];
	strcpy(temp, value[0]);
	for (int i = 1; i < tokens; i++)
	{
		strcat(temp, " ");
		strcat(temp, value[i]);
	}

	mem_set_value(var, temp);

	return 0;
}

int print(char *var)
{
	printf("%s\n", mem_get_value(var));
	return 0;
}

int echo(char *var)
{
	if (var[0] == '$')
	{
		if (strcmp(mem_get_value(var + 1), "Variable does not exist") == 0)
		{
			printf("\n");
		}
		else
		{
			printf("%s\n", mem_get_value(var + 1));
		}
	}
	else
	{
		printf("%s\n", var);
	}
	return 0;
}
int my_ls()
{
	system("ls -1");
}

int run(char *script)
{
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL)
	{
		return badcommandFileDoesNotExist();
	}

	load_file(script);
	runProcess();

	fclose(p);

	return 0;
}

int exec(char *args[], int size)
{
	int errCode = 0;
	char *policy = args[size - 1];
	
	for (int i = 0; i < (size - 1); i++)
	{
		char *current = args[i];
		for (int j = i + 1; j < (size - 1); j++)
		{
			if (strcmp(current, args[j]) == 0)
			{
				printf("Bad command: same file name\n");
				return 1;
			}
		}
	}
	if (strcmp(policy, "FCFS") != 0 && strcmp(policy, "SJF") != 0 && strcmp(policy, "RR") != 0 && strcmp(policy, "AGING") != 0)
	{
		printf("Invalid policy!\n");
		return 1;
	}
	for (int i = 0; i < (size - 1); i++)
	{
		load_file(args[i]);
	}
	if (strcmp(policy, "FCFS") == 0)
	{
		for (int i = 0; i < (size - 1); i++)
		{
			errCode = runProcess();
		}
	}
	else if (strcmp(policy, "SJF") == 0)
	{
		sortByLen();
		for (int i = 0; i < (size - 1); i++)
		{
			errCode = runProcess();
		}
	}
	else if (strcmp(policy, "RR") == 0)
	{
		errCode = RRRun();
	}
	else if (strcmp(policy, "AGING") == 0)
	{
		errCode = Aging();
	}

	return errCode;
}

int resetmem()
{
	var_mem_reset();
	return 0;
}
