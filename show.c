/*
 * =====================================================================================
 *	   Filename:  type_prompt.c
 *	Description:
 *		Version:  1.0
 *		Created:  2013.10.16 20h18min28s
 *		 Author:  wuyue (wy), vvuyve@gmail.com
 *		Company:  UESTC
 * =====================================================================================
 */
//#include <sys/utsname.h>
#include "pshell.h"
const int max_name_len = 256;
const int max_path_len = 1024;

void type_prompt(char *prompt)
{
	extern struct passwd *pwd;
	char hostname[max_name_len];
	char *pathname=NULL;
	int length;
	pwd = getpwuid(getuid());
	pathname=getcwd(NULL,0);
	if(gethostname(hostname,max_name_len)==0)
		sprintf(prompt,"%s@%s:",pwd->pw_name,strtok(hostname,"."));
	else
		sprintf(prompt,"%s@unknown:",pwd->pw_name);
	//printf("pathname: %s,length:%d\npw_dir:%s,length:%d\n",
	//pathname,strlen(pathname),pwd->pw_dir,strlen(pwd->pw_dir));
	length = strlen(prompt);
	if(strlen(pathname) < strlen(pwd->pw_dir) ||
	        strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))!=0)
		sprintf(prompt+length,"%s",pathname);
	else
		sprintf(prompt+length,"~%s",pathname+strlen(pwd->pw_dir));
	length = strlen(prompt);
	if(geteuid()==0)
		sprintf(prompt+length,"# ");
	else
		sprintf(prompt+length,"$ ");
	free(pathname);
	return;
}

//int main()
//{
//	type_prompt();
//	return 0;
//}
