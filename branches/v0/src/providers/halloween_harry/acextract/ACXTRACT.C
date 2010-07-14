#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "dclib.h"

/* File entry header (uses Pascal-style strings) */
typedef struct {
    unsigned char id_len;
    unsigned char id[4];
    unsigned char filename_len;
    unsigned char filename[12];
    long length_compressed;
    long length_uncompressed;
} __attribute__((packed)) FILEHEADER;

/* Parameter structure for explode routine */
typedef struct {
    FILE *input_file,*output_file;
    long remaining_compressed;
    long remaining_uncompressed;
} PARAM;

/* Command line arguments structure */
typedef struct {
    char *output_dir;
    char *output_ext;
} OPTION;

/* Help message */
const char *help_message=
    "Usage: acxtract [OPTION] FILE...\n"
    "\n"
    "Options:\n"
    "  -o dir   Directory to write files to\n"
    "  -x ext   Dump files of this extension only\n";

/* Valid file ID marker */
const char *valid_id="-ID-";

/* Read callback function for explode() */
UInt16 read_data(UInt8 *buffer,UInt16 size,void *param)
{
    if(((PARAM *)param)->remaining_compressed>0)
    {
        if(size>((PARAM *)param)->remaining_compressed)
            size=((PARAM *)param)->remaining_compressed;
        ((PARAM *)param)->remaining_compressed-=size;
        return fread(buffer,1,size,((PARAM *)param)->input_file);
    } else return 0;
}

/* Write callback function for explode() */
void write_data(UInt8 *buffer,UInt16 size,void *param)
{
    if(((PARAM *)param)->remaining_uncompressed>0)
    {
        if(size>((PARAM *)param)->remaining_uncompressed)
            size=((PARAM *)param)->remaining_uncompressed;
        ((PARAM *)param)->remaining_uncompressed-=size;
        fwrite(buffer,1,size,((PARAM *)param)->output_file);
    }
}

void extract_archive(char *input_filename,OPTION *option)
{
    FILEHEADER current_file_header;
    PARAM param;
    char output_filename[PATH_MAX];
    UInt8 *work_buf;

    /* Open input archive */
    param.input_file=fopen(input_filename,"rb");
    if(param.input_file==NULL)
    {
        perror(input_filename);
        exit(EXIT_FAILURE);
    }

    /* Allocate work buffer */
    work_buf=malloc(EXP_BUFFER_SIZE);

    /* Read and explode each stored file */
    while(!feof(param.input_file))
    {
        /* Read file header */
        if(fread(&current_file_header,1,sizeof(FILEHEADER),param.input_file)<sizeof(FILEHEADER)) break;

        /* Check for ID marker */
        if(strncmp(current_file_header.id,valid_id,4)!=0)
        {
            fprintf(stderr,"Unable to find a valid ID marker\n");
            exit(EXIT_FAILURE);
        }

        /* Convert filename to C string */
        memset(output_filename,0,PATH_MAX);
        if(option->output_dir!=NULL)
        {
            strcat(output_filename,option->output_dir);
            strcat(output_filename,"/");
        }
        strncat(output_filename,&current_file_header.filename,current_file_header.filename_len);

        /* Check if extension matches what we want */
        if(option->output_ext!=NULL)
        {
            if(strnicmp(output_filename+strlen(output_filename)-strlen(option->output_ext),option->output_ext,strlen(option->output_ext))!=0)
            {
                fseek(param.input_file,current_file_header.length_compressed,SEEK_CUR);
                continue;
            }
        }

        /* Open output file */
        param.output_file=fopen(output_filename,"wb");
        if(param.output_file==NULL)
        {
            perror(output_filename);
            exit(EXIT_FAILURE);
        }

        /* Dump compressed data */
        printf("writing %s\n",output_filename);
        param.remaining_compressed=current_file_header.length_compressed;
        param.remaining_uncompressed=current_file_header.length_uncompressed;
        explode(read_data,write_data,work_buf,&param);

        /* Close output file */
        fclose(param.output_file);
    }

    /* Deallocate work buffer */
    free(work_buf);

    /* Close input file */
    fclose(param.input_file);
}

int main(int argc,char **argv)
{
    OPTION option={NULL,NULL};
    int i,c;

    while((c=getopt(argc,argv,"ho:x:"))!=-1)
    {
        switch(c)
        {
            case ':':
            case '?':
                fprintf(stderr,"Try 'acxtract -h' for more information.\n");
                exit(EXIT_FAILURE);
            case 'h':
                printf(help_message);
                exit(EXIT_SUCCESS);
            case 'o':
                option.output_dir=optarg;
                break;
            case 'x':
                option.output_ext=optarg;
                break;
        }
    }

    if(optind>=argc)
    {
        printf(help_message);
        exit(EXIT_SUCCESS);
    } else {
        for(i=optind;i<argc;i++)
        {
            printf("extracting files from %s\n",argv[i]);
            extract_archive(argv[i],&option);
        }
    }
}
