#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

#define PARAMETER_A "-a"
#define PARAMETER_U "-u"
#define PARAMETER_N "-n"
#define PARAMETER_F "-f"
#define PROC_PATH "/proc/"
#define COMM_FILENAME "/comm"

typedef enum {false, true} bool;

bool check_if_valid_main_parameters(char *parameter)
{
    return ((strcmp(PARAMETER_A, parameter) == 0)
          || (strcmp(PARAMETER_N, parameter) == 0)
          || (strcmp(PARAMETER_U, parameter) == 0));
}
bool check_f_parameter(char *parameter)
{
     if(strcmp(parameter, PARAMETER_F))
     {
          fprintf(stderr,"invalid argument\n");
          return false;
     }
    return true;
}
void save_to_file(char *buf,char *filename)
{
    FILE *fp1=fopen(filename,"a+");
    fprintf(fp1,"%s",buf);
    fclose(fp1);
}
void save_to_file_2D(char **buf,char *filename)
{
    FILE *fp=fopen(filename,"a+");
    
    while(*buf !='\0')
    {
        fprintf(fp,"%s",*buf);
        buf++;
    }  
    fclose(fp);
}
void print_2D(char **buf)
{
  while(*buf !='\0')
  {
    printf("%s",*buf);
    buf++;
  }  
}
int get_file_content_size(FILE *fp)
{
    int size = 0;
    char ch;
    while((ch=getc(fp)!=EOF))
        size++;
    rewind(fp);
    return size;
}
char* get_name_of_process(char *pid)
{
    int pid_file_path_length = strlen(PROC_PATH) + strlen(COMM_FILENAME) + 
                               strlen(pid);
    
    char *pid_file_path = malloc(sizeof(char)*pid_file_path_length+1);
    strcpy(pid_file_path,PROC_PATH);
    strcat(pid_file_path,pid);
    strcat(pid_file_path,COMM_FILENAME);
    FILE *fp=fopen(pid_file_path,"r");
    if(!fp)
    {
        fprintf(stderr,"There is no process with PID %s\n", pid);
        return;
    }
    int file_content_size = get_file_content_size(fp);
   
    char *buf=malloc(sizeof(char)*file_content_size+1);
    fgets(buf,file_content_size+1,fp);
    fclose(fp);
    free(pid_file_path);
    return buf;
}
int get_number_of_pid_directories()
{
    DIR *d;
    struct dirent *dir;
    d=opendir(PROC_PATH);

    int count=0;
    if(d)
    {   while ((dir=readdir(d)))
        {  
            if(isdigit(dir->d_name[0]))
            {  
                count++;
            }
        }
    }
    rewinddir(d);
    return count;
}
char** get_pids_and_names(int number_pid_directories)
{   
    DIR *d;
    struct dirent *dir;
    d=opendir(PROC_PATH);
    
    char **buf=(char **)malloc((number_pid_directories)*sizeof(char *)); 
    
    int i=0;
    if(d)
    {   
        while((dir=readdir(d)))
        {   
            if(isdigit(dir->d_name[0]))
            {   
                char *process_name = get_name_of_process(dir->d_name);
                int line_length = strlen(dir->d_name) + strlen(process_name) +2;
                buf[i]=(char*)malloc(line_length*sizeof(char));
                strcpy(buf[i],dir->d_name);
                strcat(buf[i]," ");
                strcat(buf[i++],process_name);
                           
            }
        }
      
      closedir(d);
      
    }
    return buf;
}
char* get_number_of_process(char *string)
{
    DIR *d;
    struct dirent *dir;
    bool processFound = false;
 
    d=opendir(PROC_PATH);
    
    if(d)
    {   
        while((dir=readdir(d)))
        {   
            if(isdigit(dir->d_name[0]))
            {   
                char fileName[8]=PROC_PATH;
                char temp[7]="/comm";
                strcat(fileName,dir->d_name);
                strcat(fileName,temp);
                FILE *fp=fopen(fileName,"r");
                int fileContentSize = get_file_content_size(fp);
                char buf[fileContentSize+1];
                fgets(buf,fileContentSize+1,fp);
                fclose(fp);

                int len=strlen(buf);
                if(len>0 && (buf[len-1]=='\n'))
                {
                    buf[len-1]=0;
                }
                
                 
                if(!strcmp(string,buf))
                {   
                    int line_length = strlen(dir->d_name)+1;                 
                    char *buffer=malloc(sizeof(char)*line_length);
                    strcpy(buffer,dir->d_name);
                    strcat(buffer,"\n");
                    processFound = true;
                    return buffer;
                }
            }
        }
    }
    closedir(d);
    if(!processFound)
    {
        char error_message[100]={"There is no process with name: "};
        strcat(error_message,string);
        int line_length = strlen(error_message);
        char *buffer=malloc(sizeof(char)*line_length);
        strcpy(buffer,error_message);
        strcat(buffer,"\n");
        return buffer;
    }
   
}
void usage()
{
    fprintf(stderr,"usage: -a [-f <file>]\n         -f <file>: save to file named <file> pids and names of all processes running on the system (default display on screen)\n");
    fprintf(stderr,"usage: -u <pid> [-f <file>]\n       -f <file>: save to file named <file> name of process whose pid is in the given parameter <pid> (default display on screen)\n");
    fprintf(stderr,"usage : -n <name> [-f <file]\n      -f <file>: save to file named <file> pid of process whose name is in the given parameter <name> (default display on screen)\n");
}
int main(int argc, char **argv){
    int allowable_number_of_parameteres=5;
    int minimum_number_of_parameteres_A=2;
    int minimum_number_of_parameteres_U_N=3;
    if (argc > allowable_number_of_parameteres)
    {
        usage();
        return -1;
    }
    bool if_save_to_file = false;
    char *filename=NULL;
    if(!check_if_valid_main_parameters(argv[1]))
    {    usage();        
        return -1;
    }
    if(strcmp(PARAMETER_A, argv[1]) == 0)
    {
        if(argc>minimum_number_of_parameteres_A)
        {
            if(!check_f_parameter(argv[2]))
            {
                usage();
                return -1;
            }
            int size_of_filename=strlen(argv[3]);
            filename=malloc(sizeof(char)*size_of_filename);
            strcpy(filename, argv[3]);
            if_save_to_file = true;          
        }
        int number_of_pid_directories = get_number_of_pid_directories()+1;
        char **result=get_pids_and_names(number_of_pid_directories);
        if(if_save_to_file)
        {   
            save_to_file_2D(result, filename);
            free(filename);
        }
        else
        {    
            print_2D(result);
        }
        for(int i = 0; i < number_of_pid_directories; i++)
            free(result[i]);
        free (result);

    }
    else 
    {   
        if(argc>minimum_number_of_parameteres_U_N)
        {   
            if(!check_f_parameter(argv[3]))
            {
                usage();
                return -1;
            }
            int size_of_filename=strlen(argv[4]);
            filename=malloc(sizeof(char)*(size_of_filename+1));
            strcpy(filename, argv[4]);
            if_save_to_file = true;  
        }
        char *result = NULL;
        if(strcmp(PARAMETER_N, argv[1]) == 0)
            result = get_number_of_process(argv[2]);
        else
            result = get_name_of_process(argv[2]);
        if(if_save_to_file)
        {
            save_to_file(result, filename);
            free(filename);
        }
        else
            printf("%s", result);
        free(result);
          
    }
   return 0;
}

