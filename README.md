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
  ![soal1](/image/soal1.jpg)
  
  
  ## Soal 2
  ## Soal 3
  ## Soal 4
  ## Soal 5
