int exec_internal_cmd(char **argv)
{
	int err;
	char msg[64];
	if(!strcmp(argv[0],"chdir"))
	{
		if(argv[1])
		{
			err=chdir(argv[1]);
			if(err)
			{
				strcpy(msg,"chdir failed: -");
				sprinti(msg,-err,1);
				strcat(msg,"\n");
				write(2,msg,strlen(msg));
			}
		}
		return 0;
	}
	if(!strcmp(argv[0],"exit"))
	{
		ioctl(0,TCSETS,&oldterm);
		exit(0);
	}
	if(!strcmp(argv[0],"kill"))
	{
		if(argv[1]&&argv[2])
		{
			unsigned long int pid,sig;
			sinputi(argv[1],&pid);
			sinputi(argv[2],&sig);
			if(pid>0&&pid<0x80000000&&sig>=0&&sig<=0x7f)
			{
				err=kill(pid,sig);
				if(err)
				{
					strcpy(msg,"kill failed: -");
					sprinti(msg,-err,1);
					strcat(msg,"\n");
					write(2,msg,strlen(msg));
				}
			}
		}
		return 0;
	}
	if(!strcmp(argv[0],"reset"))
	{
		write(1,"\x0f\033[2J\033[1;1H\033[0m",15);
		ioctl(0,TCSETS,&term);
		return 0;
	}

	return 1;
}
