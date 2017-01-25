int my_fopen(int *fd, char *UserFile); //function prototype

int my_fopen(int my_fopen(int *fd, char *UserFile) 
{
  *fd = open(UserFile, O_RDONLY); //Open file for reading
}