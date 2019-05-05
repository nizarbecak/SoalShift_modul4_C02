# SoalShift_modul4_C02

## Soal 1
  ```
  #define FUSE_USE_VERSION 28
  #include <fuse.h>
  #include <stdio.h>
  #include <string.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <dirent.h>
  #include <errno.h>
  #include <sys/time.h>

  static const char *source = "/home/pejuangsisop/shift4/";

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
    char filename1[1024];
      char filename2[1024];
      snprintf(filename1, sizeof(filename1), "%s", path);
      enkripsi(filename1);
      snprintf(filename2, sizeof(filename2), "%s%s",source, filename1);

      lstat(filename2, stbuf);

    return 0;
  }

  static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
          struct fuse_file_info *fi)
  {
      char filename1[1024];
      char filename2[1024];
      snprintf(filename1, sizeof(filename1), "%s", path);
      enkripsi(filename1);
      snprintf(filename2, sizeof(filename2), "%s%s",source, filename1);

    int res,fd;

    (void) fi;
    fd=open(filename2, O_RDONLY);
    res=pread(fd, buf, size, offset);
    close(fd);
    return res;
  }

  static int xmp_write(const char *path, const char *buf, size_t size,
           off_t offset, struct fuse_file_info *fi)
  {
      char filename1[1024];
      char filename2[1024];
      snprintf(filename1, sizeof(filename1), "%s", path);
      enkripsi(filename1);
      snprintf(filename2, sizeof(filename2), "%s%s",source, filename1);

    int res,fd;

    (void) fi;
    fd=open(filename2, O_WRONLY);
    res=pwrite(fd, buf, size, offset);
    close(fd);
    return res;
  }

  static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
  {
      char filename1[1024];
      char filename2[1024];
      snprintf(filename1, sizeof(filename1), "%s", path);
      enkripsi(filename1);
      snprintf(filename2, sizeof(filename2), "%s%s",source, filename1);

    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(filename2);
    if (dp == NULL)
      return -errno;

    while ((de = readdir(dp)) != NULL) {
      struct stat st;
      memset(&st, 0, sizeof(st));
      st.st_ino = de->d_ino;
      st.st_mode = de->d_type << 12;
      snprintf(filename1, sizeof(filename1),"%s",de->d_name);
        dekripsi(filename1);
      if (filler(buf, filename1, &st, 0))
          break;
    }

    closedir(dp);
    return 0;
  }

  static struct fuse_operations xmp_oper = {
    .getattr	    = xmp_getattr,
    .readdir	    = xmp_readdir,
    .read		    = xmp_read,
    .write		    = xmp_write,
  };

  int main(int argc, char *argv[])
  {
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
  }
  ```
  
  Pada soal nomor 1 ini, saya menggunakan fungsi enkripsi dan dekripsi. Dekripsi digunakan untuk menampilkan nama file dan foldernya. Sedangkan enkripsinya digunakan untuk mengakses folder masternya.<br>
  Berikut adalah hasilnya.<br>
  ![soal1](/image/soal1.JPG)
  
 # Revisi
 ## Soal 2
  ```
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
  ```
  Pada soal nomor 2 ini, kami menggunakan fungsi pre init untuk menjalankan file system sebelum inisialisasi. Pada fungsi pre init ini dibuat folder Videos, menscan file yang memiliki ekstensi .mkv, lalu dijoin pada folder Videos.
  Lalu kami juga mengguanakan fungsi destroy untuk menghapus folder Videos<br>
  Berikut adalah hasilnya.<br>
  ![soal2a](/image/soal2a.JPG)
  ![soal2b](/image/soal2b.JPG)
  ![soal2c](/image/soal2c.JPG)
  
## Soal 3
 
## Soal 4
  ```
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
  ```
  Pada soal nomor 4 ini, kami menggunakan fungsi pre init untuk membuat folder YOUTUBE.
  Lalu kami juga menggunakan fungsi chmod untuk merubah permission dari file tersebut.
  Lalu kami juga menggunakan fungsi create untuk menambahkan ekstensi file yang dicreate<br>
  Berikut adalah hasilnya.<br>
  ![soal4a](/image/soal4a.JPG)
  ![soal4b](/image/soal4b.JPG)
  ![soal4c](/image/soal4c.JPG)
## Soal 5
