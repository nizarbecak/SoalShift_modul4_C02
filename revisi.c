#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <grp.h>
#include <pwd.h>


static const char *source = "/home/pejuangsisop/shift4";

char *encryptchar = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";

void enkripsi(char *filename)
{
    if(!strcmp(filename, ".")) return;
    else if(!strcmp(filename, "..")) return;
    else
    {
        for (int i=0; i<strlen(filename); i++)
        {
            for (int j=0; j<strlen(encryptchar); j++)
            {
                if(filename[i]==encryptchar[j])
                {
                    filename[i]=encryptchar[(j+17)%94];
                    break;
                }
            }
        }
    }
}

void dekripsi(char *filename)
{
    if(!strcmp(filename, ".")) return;
    else if(!strcmp(filename, "..")) return;
    else
    {
        for (int i=0; i<strlen(filename); i++)
        {
            for (int j=0; j<strlen(encryptchar); j++)
            {
                if(filename[i]==encryptchar[j])
                {
                    filename[i]=encryptchar[(j+77)%94];
                    break;
                }
            }
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
    char filename2[1024];
	char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	
	res = lstat(filename2, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);

	int res, fd;

	(void) fi;
	fd = open(filename2, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(filename2);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
        char filename3[1024];
		strcpy(filename3,de->d_name);
		char filename4[1024];
		sprintf(filename4,"%s%s",filename2,filename3);
		dekripsi(filename3);

		struct stat tmp;
		stat(filename4,&tmp);
		struct passwd *filename1 = getpwuid(tmp.st_uid);
    	struct group *grup = getgrgid(tmp.st_gid);
		
		if( (strcmp(filename1->pw_name,"chipset") == 0 || strcmp(filename1->pw_name,"ic_controller") == 0) 
			&& strcmp(grup->gr_name,"rusak")==0 
			&& ((tmp.st_mode & S_IRUSR) == 0 || (tmp.st_mode & S_IRGRP) == 0 || (tmp.st_mode & S_IROTH) == 0) )
		{
			printf("%s\n",filename4);
			char root[1024];
			strcpy(root,source);
			char note[1024] = "/filemiris.txt";
			enkripsi(note);
			strcat(root,note);
			FILE * fp;
   			fp = fopen (root, "a+");
			char t[1024];
			time_t now = time(NULL);
			strftime(t, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
			char buffer[1024];
			sprintf(buffer,"%s-%d-%d-%s",filename3,filename1->pw_uid,grup->gr_gid,t);
			fprintf(fp,"%s\n",buffer);
			remove(filename4);
			fclose(fp);
			chown(root,1024,1024);
		}
		else{
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			if (filler(buf, filename3, &st, 0))
				break;
		}
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	if (S_ISREG(mode)) {
		res = open(filename2, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(filename2, mode);
	else
		res = mknod(filename2, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
	if(strlen(filename1)>9 && strncmp(filename1,"/YOUTUBER",9)==0)
	{
		enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
		res = mkdir(filename2, 0750);	
	}
	else{
    	enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
		res = mkdir(filename2, mode);
		if (res == -1)
			return -errno;
	}
	return 0;
}

static void* pre_init(struct fuse_conn_info *conn)
{
        char folder1[1024] = "/Videos";
		char folder2[1024] = "/YOUTUBER";
		enkripsi(folder1);
		enkripsi(folder2);
		char filename2[1024];
    	sprintf(filename2,"%s%s", source, folder1);
		mkdir(filename2,0755);
		memset(filename2,0,sizeof(filename2));
		sprintf(filename2,"%s%s", source, folder2);
		mkdir(filename2,0755);
		memset(filename2,0,sizeof(filename2));

		pid_t child1;
		child1=fork();
		if(child1==0){
			DIR *dp;
			struct dirent *de;
			dp = opendir(source);
			while((de = readdir(dp))){
				if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){
					char ext[1024] = ".mkv";
					enkripsi(ext);
					if(strlen(de->d_name)>7 && strncmp(de->d_name+strlen(de->d_name)-8,ext,4)==0){

							char joined[1024];
							char video[1024] = "/Videos";
							enkripsi(video);
							sprintf(joined,"%s%s/",source,video);
							strncat(joined,de->d_name,strlen(de->d_name)-4);
							FILE* mainj;
							mainj = fopen(joined,"a+");
							FILE* need;
							char this[1024];
							sprintf(this,"%s/%s",source,de->d_name);
							need = fopen(this,"r");
							int c;
							while(1) {
   								c = fgetc(need);
   								if( feof(need) ) {
   								   break;
   								}
   								fprintf(mainj,"%c",c);
   							}
							
					}
				}
			}
			exit(EXIT_SUCCESS);
		}

        (void) conn;
        return NULL;
}

static void post_destroy(void* private_data)
{
	char filename2[1024];
	char folder[1024] = "/Videos";
	enkripsi(folder);
    sprintf(filename2,"%s%s", source, folder);

	DIR *dp;
	struct dirent *de;
	dp = opendir(filename2);
	while((de = readdir(dp))){
		if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){
			char file[1024];
			sprintf(file,"%s/%s",filename2,de->d_name);
			remove(file);
		}
	}
	rmdir(filename2);

	return;
}

static int xmp_unlink(const char *path)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
	if(strstr(filename1,".swp")==0 && strstr(filename1,".gooutpustream")==0){
		char folder[1024] = "/RecycleBin";
		enkripsi(folder);
		char filename2[1024];
    	sprintf(filename2,"%s%s", source, folder);
		mkdir(filename2,0755);

		char t[1024];
		time_t now = time(NULL);
		strftime(t, 22, "_%Y-%m-%d_%H:%M:%S", localtime(&now));

		char zip[1024];
		char fzip[1024];
		char fname[1024];
		memset(zip,0,sizeof(zip));
		memset(fzip,0,sizeof(fzip));
		memset(fname,0,sizeof(fname));
		sprintf(zip,"/RecycleBin%s_deleted_%s.zip",filename1,t);
		enkripsi(filename1);
		sprintf(fname,"%s%s",source,filename1);
		enkripsi(zip);
		sprintf(fzip,"%s%s",source,zip);
		pid_t child1;
		
		child1=fork();
		if(child1==0){
			execl("/usr/bin/zip","/usr/bin/zip","-q","-m","-j",fzip,fname,NULL);
			exit(EXIT_SUCCESS);
		}
		else{
			wait(NULL);
		}
		

		char fback[1024] = "/Backup";
		enkripsi(fback);
		char foldbackp[1024];
		sprintf(foldbackp,"%s%s",source,fback);
		DIR *dp;
		struct dirent *de;

		dp = opendir(foldbackp);
		while((de = readdir(dp))){
			if(strncmp(filename1+1,de->d_name,strlen(de->d_name)-29)==0){

				memset(fzip,0,sizeof(fzip));
				memset(zip,0,sizeof(zip));
				memset(fname,0,sizeof(fname));
				dekripsi(filename1);
				sprintf(zip,"/RecycleBin%s_deleted_%s.zip",filename1,t);
				sprintf(fname,"%s/%s",foldbackp,de->d_name);
				enkripsi(zip);
				enkripsi(filename1);
				sprintf(fzip,"%s%s",source,zip);

				child1=fork();
				if(child1==0){
					execl("/usr/bin/zip","/usr/bin/zip","-q","-m","-j","-u",fzip,fname,NULL);
					exit(EXIT_SUCCESS);
				}
				else{
					wait(NULL);
				}
			}
		}
		dekripsi(filename1);
		memset(zip,0,sizeof(zip));
		memset(fzip,0,sizeof(fzip));
		memset(fname,0,sizeof(fname));
		sprintf(zip,"/RecycleBin%s_deleted_%s.zip",filename1,t);
		enkripsi(filename1);
		sprintf(fname,"%s%s",source,filename1);
		enkripsi(zip);
		sprintf(fzip,"%s%s",source,zip);
		memset(fname,0,sizeof(fname));
		strncpy(fname,fzip,strlen(fzip));
		strcat(fzip,".zip");
		rename(fzip,fname);
	}
	else{
    	enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
		res = unlink(filename2);
		if (res == -1)
			return -errno;
	}
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	res = rmdir(filename2);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
    char new_from[1024];
    char new_to[1024];
	char name1[1024];
	char name2[1024];
	sprintf(name1,"%s",from);
	sprintf(name2,"%s",to);
	enkripsi(name1);
	enkripsi(name2);
    sprintf(new_from,"%s%s",source,name1);
    sprintf(new_to,"%s%s",source,name2);
	res = rename(new_from, new_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
	if(strlen(filename1)>9 && strncmp(filename1,"/YOUTUBER",9)==0 && strcmp(filename1+strlen(filename1)-4,".iz1")==0)
	{
		pid_t child1;
		child1=fork();
		if(child1==0){
			execl("/usr/bin/zenity","/usr/bin/zenity","--error","--text=File ekstensi iz1 tidak boleh diubah permissionnya.","--title=Tidak bisa merubah",NULL);

				printf("test\n");
	}
		else{
			wait(NULL);
		}
	}
	else{
    	enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
		res = chmod(filename2, mode);
	}
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	res = lchown(filename2, uid, gid);
	if (res == -1) 
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	res = truncate(filename2, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];

	char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1024;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1024;

	res = utimes(filename2, tv);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);
	res = open(filename2, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
    char filename2[1024];
    char filename1[1024];

	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2,"%s%s",source,filename1);

	char newname[1024];
	char folder[1024] = "/Backup";
	enkripsi(folder);
	char folderdir[1024];
	sprintf(folderdir,"%s%s",source,folder);
	mkdir(folderdir,0755);

	(void) fi;
	fd = open(filename2, O_WRONLY);
	if (fd == -1)
		return -errno;
	printf("%s\n",buf);
	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	struct stat sd;
	if(stat(filename2,&sd)>-1){
		char t[1024];
		time_t now = time(NULL);
		strftime(t, 22, "_%Y-%m-%d_%H:%M:%S", localtime(&now));
		dekripsi(filename1);
		sprintf(newname,"/Backup%s%s.ekstensi",filename1,t);
		enkripsi(newname);
		memset(filename2,'\0',sizeof(filename2));
		sprintf(filename2,"%s%s",source,newname);
		// printf("%s\n",filename2);
	
		FILE *fptr = fopen(filename2, "w+"); 
		fprintf(fptr,"%s",buf);
		fclose(fptr);

		return res;
	}
	
	return res;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{

    (void) fi;
    char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    int res;
	if(strlen(filename1)>9 && strncmp(filename1,"/YOUTUBER",9)==0)
	{
		strcat(filename1,".iz1");
		enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
    	res = creat(filename2, 0640);
	}
	else{
    	enkripsi(filename1);
		sprintf(filename2, "%s%s",source,filename1);
    	res = creat(filename2, mode);
	}
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);

	(void) filename2;
	(void) fi;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;

	char filename2[1024];
    char filename1[1024];
	sprintf(filename1,"%s",path);
    enkripsi(filename1);
	sprintf(filename2, "%s%s",source,filename1);

	res = access(filename2, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = 
{
	.init		= pre_init,
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.access		= xmp_access,
	.rename		= xmp_rename,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.create		= xmp_create,
	.release	= xmp_release,
	.destroy	= post_destroy,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}