#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <netinet/in.h>

#define PATTERN_LENGTH 64
#define NUM_CHANNELS 4
#define MAX_SAMPLES 31

/* Sample entry in MOD header */
typedef struct {
    char name[22];
    unsigned short length;
    unsigned char finetune;
    unsigned char volume;
    unsigned short repeat_start;
    unsigned short repeat_length;
} __attribute__((packed)) SAMPLEENTRY;

/* MOD file header structure */
typedef struct {
    char name[20];
    SAMPLEENTRY sample_entry[31];
    unsigned char num_rows;
    unsigned char max_rows;
    unsigned char row_pattern[128];
    char sig[4];
} __attribute__((packed)) MODHEADER;

/* Expanded form of a pattern note */
typedef struct {
    unsigned char sample;
    unsigned short period;
    unsigned short effect;
} NOTE;

/* Structure for holding an entire MOD file */
typedef struct {
    MODHEADER header;
    NOTE ***note;
    unsigned char **sample;
    int num_patterns;
} MODFILE;

/* Reads a single pattern note and expands it */
void read_note(FILE *input_file,NOTE *note)
{
    unsigned char byte_array[4];

    fread(&byte_array,1,4,input_file);
    note->sample=(byte_array[0] & 0xF0) | ((byte_array[2] & 0xF0) >> 4);
    note->period=((byte_array[0] & 0x0F) << 8) | byte_array[1];
    note->effect=((byte_array[2] & 0x0F) << 8) | byte_array[3];
}

/* Compacts and writes a single pattern note */
void write_note(FILE *output_file,NOTE *note)
{
    unsigned char byte_array[4];

    byte_array[0]=(note->sample & 0xF0) | (note->period >> 8);
    byte_array[1]=(note->period & 0x00FF);
    byte_array[2]=((note->sample & 0x0F) << 4) | (note->effect >> 8);
    byte_array[3]=(note->effect & 0x00FF);
    fwrite(&byte_array,1,4,output_file);
}

void read_4md_file(FILE *input_file,MODFILE *mod_file)
{
    int i,j,k;

    /* Read in MOD header */
    fread(&(mod_file->header),sizeof(MODHEADER),1,input_file);

    /* Need to fix endian order (MOD & 4MD use big endian) */
    for(i=0;i<31;i++)
    {
        mod_file->header.sample_entry[i].length=ntohs(mod_file->header.sample_entry[i].length);
        mod_file->header.sample_entry[i].repeat_start=ntohs(mod_file->header.sample_entry[i].repeat_start);
        mod_file->header.sample_entry[i].repeat_length=ntohs(mod_file->header.sample_entry[i].repeat_length);
    }

    /* Find out number of patterns */
    mod_file->num_patterns=1;
    for(i=0;i<mod_file->header.num_rows;i++)
    {
        if(mod_file->header.row_pattern[i]+1>mod_file->num_patterns)
           mod_file->num_patterns=mod_file->header.row_pattern[i]+1;
    }

    /* Allocate buffers for expanded patterns */
    mod_file->note=malloc(mod_file->num_patterns*sizeof(NOTE **));
    for(i=0;i<mod_file->num_patterns;i++)
    {
        mod_file->note[i]=malloc(NUM_CHANNELS*sizeof(NOTE *));
        for(j=0;j<NUM_CHANNELS;j++)
            mod_file->note[i][j]=malloc(PATTERN_LENGTH*sizeof(NOTE));
    }

    /* Read in pattern data */
    for(i=0;i<mod_file->num_patterns;i++)
        for(k=0;k<PATTERN_LENGTH;k++)
            for(j=0;j<NUM_CHANNELS;j++)
                read_note(input_file,&(mod_file->note[i][j][k]));

    /* Allocate buffers for samples */
    mod_file->sample=malloc(MAX_SAMPLES*sizeof(unsigned char *));

    /* Read in samples */
    for(i=0;i<MAX_SAMPLES;i++)
    {
        if(mod_file->header.sample_entry[i].length>0)
        {
            mod_file->sample[i]=malloc(mod_file->header.sample_entry[i].length+mod_file->header.sample_entry[i].length%2);
            memset(mod_file->sample[i],0,mod_file->header.sample_entry[i].length+mod_file->header.sample_entry[i].length%2);
            fread(mod_file->sample[i],1,mod_file->header.sample_entry[i].length,input_file);
        } else mod_file->sample[i]=NULL;
    }

    /* Round up and divide sample lengths by 2 */
    /* (4MD uses bytes rather than words) */
    for(i=0;i<31;i++)
    {
        mod_file->header.sample_entry[i].length++;
        mod_file->header.sample_entry[i].repeat_start++;
        mod_file->header.sample_entry[i].repeat_length++;
        mod_file->header.sample_entry[i].length/=2;
        mod_file->header.sample_entry[i].repeat_start/=2;
        mod_file->header.sample_entry[i].repeat_length/=2;
    }

}

void write_mod_file(FILE *output_file,MODFILE *mod_file)
{
    int i,j,k;

    /* Need to fix endian order (MOD & 4MD use big endian) */
    for(i=0;i<31;i++)
    {
        mod_file->header.sample_entry[i].length=htons(mod_file->header.sample_entry[i].length);
        mod_file->header.sample_entry[i].repeat_start=htons(mod_file->header.sample_entry[i].repeat_start);
        mod_file->header.sample_entry[i].repeat_length=htons(mod_file->header.sample_entry[i].repeat_length);
    }

    /* Write out MOD header */
    fwrite(&(mod_file->header),sizeof(MODHEADER),1,output_file);

    /* Reverse endian order again so sample data can be written later */
    for(i=0;i<31;i++)
    {
        mod_file->header.sample_entry[i].length=ntohs(mod_file->header.sample_entry[i].length);
        mod_file->header.sample_entry[i].repeat_start=ntohs(mod_file->header.sample_entry[i].repeat_start);
        mod_file->header.sample_entry[i].repeat_length=ntohs(mod_file->header.sample_entry[i].repeat_length);
    }

    /* Write out pattern data */
    for(i=0;i<mod_file->num_patterns;i++)
        for(k=0;k<PATTERN_LENGTH;k++)
            for(j=0;j<NUM_CHANNELS;j++)
                write_note(output_file,&(mod_file->note[i][j][k]));

    /* Deallocate pattern buffers */
    for(i=0;i<mod_file->num_patterns;i++)
    {
        for(j=0;j<NUM_CHANNELS;j++) free(mod_file->note[i][j]);
        free(mod_file->note[i]);
    }
    free(mod_file->note);

    /* Write out samples */
    for(i=0;i<MAX_SAMPLES;i++)
    {
        if(mod_file->sample[i]!=NULL)
            fwrite(mod_file->sample[i],1,mod_file->header.sample_entry[i].length*2,output_file);
    }

    /* Deallocate sample buffers */
    for(i=0;i<MAX_SAMPLES;i++)
        if(mod_file->sample[i]!=NULL) free(mod_file->sample[i]);
    free(mod_file->sample);
}

void convert_4md_mod(const char *input_filename,const char *output_filename)
{
    FILE *input_file,*output_file;
    MODFILE mod_file;
    int i,j,k;

    input_file=fopen(input_filename,"rb");
    if(input_file==NULL)
    {
        perror(input_filename);
        exit(EXIT_FAILURE);
    }

    output_file=fopen(output_filename,"wb");
    if(output_file==NULL)
    {
        perror(output_filename);
        exit(EXIT_FAILURE);
    }

    read_4md_file(input_file,&mod_file);

    /* Double the period (4MD seems to have the note periods halved) */
    for(i=0;i<mod_file.num_patterns;i++)
        for(k=0;k<PATTERN_LENGTH;k++)
            for(j=0;j<NUM_CHANNELS;j++)
                mod_file.note[i][j][k].period*=2;

    write_mod_file(output_file,&mod_file);

    fclose(input_file);
    fclose(output_file);
}

void patch_extension(char *dest,const char *src,char *new_ext)
{
    char *ext_start;

    strcpy(dest,src);
    ext_start=strrchr(dest,'.');
    if(ext_start==NULL) ext_start=dest+strlen(dest);
    strcpy(ext_start,new_ext);
}

int main(int argc,char **argv)
{
    int i;
    char output_filename[PATH_MAX];

    if(argc<1)
    {
        fprintf(stderr,"Usage: 4md2mod FILE...\n");
        exit(EXIT_FAILURE);
    }

    for(i=1;i<argc;i++)
    {
        patch_extension(output_filename,argv[i],".mod");
        printf("converting %s to %s\n",argv[i],output_filename);
        convert_4md_mod(argv[i],output_filename);
    }
}
