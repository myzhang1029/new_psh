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
#define cmdif(cmd) if(strcmp(command,cmd) == 0)
int builtin_command(char *command, char **parameters)
{
	extern struct passwd *pwd;
    if(strcmp(command,"exit")==0 || strcmp(command,"quit")==0)
    {
        if(parameters[1]==NULL)
            exit(0);
        else
            exit(atoi(parameters[1]));
    }
    else cmdif("about")
    {
        OUT2E("This is a simulation of shell (bash) in Linux.\n");
        return 1;
    }
    else cmdif("cd")
    {
        char *cd_path = NULL;
        
        if(parameters[1] == NULL/* 'cd' */)
        {
            cd_path=malloc(strlen(pwd->pw_dir)+1);
            if(cd_path == NULL)
            {
                OUT2E("cd: malloc failed: %s\n", strerror(errno));
                return 2;
            }
            strcpy(cd_path, pwd->pw_dir);
        }
        else if(parameters[1][0] == '~')
        {
            cd_path = malloc(strlen(pwd->pw_dir)+strlen(parameters[1])+1);
            if(cd_path == NULL)
            {
                OUT2E("cd: malloc failed: %s\n", strerror(errno));
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
                OUT2E("cd: malloc failed: %s\n", strerror(errno));
                return 2;
            }
            strcpy(cd_path,parameters[1]);
        }
        if(chdir(cd_path)!= 0)
            OUT2E("cd: %s: %s\n", strerror(errno), cd_path);
        free(cd_path);
        return 1;
    }
    else cmdif("echo")
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
    else cmdif("export")
    {
        OUT2E("psh: export Not supported");
    }
    
    return 0;
}
