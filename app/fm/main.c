#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/stat.c"
#include "../../include/iformat.c"

void display_mode(unsigned short int mode)
{
	char *ftype;
	char *sperm[8],*perm;
	int x,y;
	ftype="EpcEdEbE-ElEsEEEE";
	sperm[0]="rwxrwxrwx---------";
	sperm[1]="rwxrwxrwt--------T";
	sperm[2]="rwxrwsrwx-----S---";
	sperm[3]="rwxrwsrwt-----S--T";
	sperm[4]="rwsrwxrwx--S------";
	sperm[5]="rwsrwxrwt--S-----T";
	sperm[6]="rwsrwsrwx--S--S---";
	sperm[7]="rwsrwsrwt--S--S--T";
	write(1,ftype+(mode>>12),1);
	x=9;
	y=0;
	perm=sperm[mode>>9&7];
	while(x)
	{
		--x;
		if(mode&1<<x)
		{
			write(1,perm+y,1);
		}
		else
		{
			write(1,perm+y+9,1);
		}
		++y;
	}
}
void display_time(unsigned long int sec)
{
	int year,month,day,hour,minute,second,pm;
	char buf[256];
	int days_in_month[12];
	days_in_month[0]=31;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
	year=1970+sec/(86400*146097);
	sec%=(86400*146097);
	while(1)
	{
		if(year%4==0&&year%100!=0||year%400==0)
		{
			days_in_month[1]=29;
			if(sec<=366*86400)
			{
				break;
			}
			sec-=366*86400;
		}
		else
		{
			days_in_month[1]=28;
			if(sec<=365*86400)
			{
				break;
			}
			sec-=365*86400;
		}
		++year;
	}
	month=0;
	while(month<12)
	{
		if(sec<days_in_month[month]*86400)
		{
			break;
		}
		sec-=days_in_month[month]*86400;
		++month;
	}
	day=sec/86400;
	hour=sec%86400;
	minute=hour%3600;
	hour/=3600;
	second=minute%60;
	minute/=60;
	buf[0]=0;
	sprinti(buf,month+1,1);
	strcat(buf,"/");
	sprinti(buf,day+1,1);
	strcat(buf,"/");
	sprinti(buf,year,1);
	strcat(buf," ");
	if(hour>=12)
	{
		pm=1;
		hour-=12;
	}
	else
	{
		pm=0;
	}
	if(hour==0)
	{
		hour=12;
	}
	sprinti(buf,hour,2);
	strcat(buf,":");
	sprinti(buf,minute,2);
	strcat(buf,":");
	sprinti(buf,second,2);
	if(pm)
	{
		strcat(buf," PM\n");
	}
	else
	{
		strcat(buf," AM\n");
	}
	write(1,buf,strlen(buf));
}
int display_stat(char *name)
{
	struct stat st;
	int err,n;
	char buf[4096];
	if(err=lstat(name,&st))
	{
		return err;
	}
	strcpy(buf,"Size: ");
	sprinti(buf,st.size,1);
	strcat(buf,"\nSize on Disk: ");
	sprinti(buf,st.blocks<<9,1);
	strcat(buf,"\nInode: ");
	sprinti(buf,st.ino,1);
	strcat(buf,"\nLinks: ");
	sprinti(buf,st.nlink,1);
	strcat(buf,"\nMajor Device Number: ");
	sprinti(buf,st.rdev>>8,1);
	strcat(buf,"\nMinor Device Number: ");
	sprinti(buf,st.rdev&0xff,1);
	strcat(buf,"\nUID: ");
	sprinti(buf,st.uid,1);
	strcat(buf,"\nGID: ");
	sprinti(buf,st.gid,1);
	strcat(buf,"\nMode: ");
	write(1,buf,strlen(buf));
	display_mode(st.mode);
	write(1,"\n",1);
	write(1,"Last Access: ",13);
	display_time(st.atime);
	write(1,"Last Status Change: ",20);
	display_time(st.ctime);
	write(1,"Last Modification: ",19);
	display_time(st.mtime);

	if((st.mode&0170000)==STAT_LNK)
	{
		n=readlink(name,buf,4096);
		if(n>0)
		{
			write(1,"Link Target: ",13);
			write(1,buf,n);
			write(1,"\n",1);
		}
	}
	return 0;
}

int main(int argc,char **argv)
{
	unsigned short int mode;
	struct stat st;
	int x,err,c;
	if(argc<3)
	{
		return 1;
	}
	if(!strcmp(argv[1],"-s"))
	{
		return display_stat(argv[2]);
	}
	if(argc<4)
	{
		return 1;
	}
	if(!strcmp(argv[1],"-n"))
	{
		return rename(argv[2],argv[3]);
	}
	if(!strcmp(argv[1],"-m"))
	{
		err=stat(argv[2],&st);
		if(err)
		{
			return err;
		}
		x=1;
		mode=0;
		while(x<5)
		{
			c=argv[3][x];
			if(c>='0'&&c<='7')
			{
				mode=mode<<3|c-'0';
			}
			else
			{
				break;
			}
			++x;
		}
		if(argv[3][0]=='+')
		{
			mode=st.mode&07777|mode;
		}
		else if(argv[3][0]=='-')
		{
			mode=st.mode&07777&~mode;
		}
		else
		{
			return 1;
		}
		return chmod(argv[2],mode);
	}

	return 1;
}
