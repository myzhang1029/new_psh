#include "builtin.h"

int builtin_cd(char *command, char **parameters)
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
