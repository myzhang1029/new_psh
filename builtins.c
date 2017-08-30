/*
 * =====================================================================================
 *       Filename:  builtin_command.c
 *    Description:  
 *        Version:  1.0
 *        Created:  2013.11.01 15h31m28s
 *         Author:  wuyue (wy), vvuyve@gmail.com
 *        Company:  UESTC
 * =====================================================================================
 */
#include "pshell.h"
#define cmdis(cmd) (strcmp(command,cmd) == 0)

int builtin_command(char *command, char **parameters)
{
	extern struct passwd *pwd;
    if(cmdis("exit") || cmdis("quit"))
    {
        if(parameters[1]==NULL)
            exit(0);
        else
            exit(atoi(parameters[1]));
    }
    else if(cmdis("about"))
    {
        printf("psh is a not fully implemented shell in UNIX.\n");
        return 1;
    }
    else if(cmdis("cd")||cmdis("chdir"))
    {
        char *cd_path = NULL;
        
        if(parameters[1] == NULL/* 'cd' */)
        {
            cd_path=malloc(strlen(pwd->pw_dir)+1);
            if(cd_path == NULL)
            {
                OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
                return 2;
            }
            strcpy(cd_path, pwd->pw_dir);
        }
        else if(parameters[1][0] == '~')
        {
            cd_path = malloc(strlen(pwd->pw_dir)+strlen(parameters[1])+1);
            if(cd_path == NULL)
            {
                OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
                return 2;
            }
            strcpy(cd_path, pwd->pw_dir);
            strncpy(cd_path+strlen(pwd->pw_dir),parameters[1]+1,strlen(parameters[1]));
        }
        else
        {
            cd_path = malloc(strlen(parameters[1]+1));
            if(cd_path == NULL)
            {
                OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
                return 2;
            }
            strcpy(cd_path,parameters[1]);
        }
        if(chdir(cd_path)!= 0)
            OUT2E("%s: %s: %s\n", command, strerror(errno), cd_path);
        free(cd_path);
        return 1;
    }
    else if(cmdis("echo"))
    {
        if(parameters[1] == NULL)
        {
            /* A blank line */
            printf("\n");
        }
        else if(parameters[1][0] == '-')
        {
            switch(parameters[1][1])
            {
                case 0:
                    /* Another blank line */
                    puts("");
                    break;
                case 'n':
                    if(parameters[2] == NULL)
                    {
                        /* No more blank line */
                        break;
                    }
                    else
                    {
                        int cnt=2;
                        while (parameters[cnt] != NULL)
                        {
                            printf("%s", parameters[cnt]);
                            cnt++;
                        }
                        return 1;
                    }
            }
        }
        else
        {
            int cnt=1;
            printf("%s", parameters[cnt]);
            while (parameters[++cnt] != NULL)
            {
                printf(" %s", parameters[cnt]);
                cnt++;
            }
            puts("");
            return 1;
        }
    }
    else if(cmdis("export")||cmdis("alias"))
    {
        OUT2E("psh: export Not supported");
        return 1;
    }
    
    return 0;
}
