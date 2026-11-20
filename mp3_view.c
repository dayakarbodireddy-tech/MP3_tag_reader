#include "mp3_header.h"

void view(struct MP3 *mp3)
{
    //opened the file using fopen and returns file pointer to org_mp3_fptr
   mp3->org_mp3_fptr=fopen(mp3->mp3_filename,"rb");
   if(mp3->org_mp3_fptr==NULL)
   {
    printf("ERROR : unable to open this MP3 file\n");
    return;
   }
   //reading 3 bytes of header frame(which is ID3)
   char id3_buffer[3];
   if(fread(id3_buffer,sizeof(char),3,mp3->org_mp3_fptr)!=3)
   {
    printf("ERROR:Unable to read ID3 bytes\n");
    return;
   }
   //here we checking(ID3 is present or not)
   if(strncmp(id3_buffer,"ID3",3)!=0)
   {
    printf("ERROR : Invalid MP3 file or ID3 Tag not found\n");
    return;
   }
   //upto here ID3header occupies 10 bytes includes tag size 
   //important thing is after ID3header 10 bytes ,the metadata will be stored after that file pointer starts pointing to actual audio data
   //so total tag length is ID3 header + metadata after stored ID3 header.
   //metadata means artist name ,album ,year,genre,composer like that bytes depends on our assigning names to that..
   //we have lot of tags my project works on 6 tags onlyyy
   //they are TPE1(ARTIST),TIT2(TITLE/SONG),TALB(ALBUM),TYER(YEAR),TCON(GENRE),TCOM(COMPOSER) 
   unsigned char header_buffer[7];
   if(fread(header_buffer,sizeof(char),7,mp3->org_mp3_fptr)!=7)
   {
    printf("ERROR : Our program didn't read the Remaining 7 bytes\n");
    return;
   }
   unsigned int tag_size;
   tag_size=((header_buffer[3] & 0x7F)<<21)|
            ((header_buffer[4] & 0x7F)<<14)|
            ((header_buffer[5] & 0x7F)<<7)|
            ((header_buffer[6] & 0x7F));
printf("Tag size is : %u\n",tag_size);
printf("\n");
unsigned int bytes_read=0;
while(bytes_read<tag_size)
{
   char frame_id[5];
   if(fread(frame_id,sizeof(char),4,mp3->org_mp3_fptr)!=4)
   {
    printf("ERROR : Unable to read frame ID\n");
    return;
   }
   frame_id[4]='\0';
   printf("Frame ID : %s\n",frame_id);
   
   unsigned char frame_size[4];//but these 4 bytes are stored in big-endian format in ID3v2.3.
   if(fread(frame_size,sizeof(char),4,mp3->org_mp3_fptr)!=4)
   {
    printf("ERROR : The Frame size is not valid for this frame ID\n");
    return;
   }
   unsigned int frame_data_size;
   frame_data_size=(((unsigned int)frame_size[0])<<24)|
                   (((unsigned int)frame_size[1])<<16)|
                   (((unsigned int)frame_size[2])<<8)|
                   (((unsigned int)frame_size[3]));
   printf("Frame_size for particular frame id is %u\n",frame_data_size);
   unsigned char flag_buffer[2];
    if(fread(flag_buffer,sizeof(char),2,mp3->org_mp3_fptr)!=2)
    {
        printf("ERROR : FLAG doesn't have 2 bytes\n");
        return;
    }
    //actual frame data
    char frame_data[frame_data_size+1];
    if(fread(frame_data,sizeof(char),frame_data_size,mp3->org_mp3_fptr)!=frame_data_size)
    {
       printf("ERROR : Unable to read frame data\n");
       return;
    }
    frame_data[frame_data_size]='\0';
    //printf("Encoding byte: %d\n", (unsigned char)frame_data[0]);
    if(strcmp(frame_id,"TIT2")==0)
   {
    printf("Title is %s\n",&frame_data[1]);
   }
   else if((strcmp(frame_id,"TPE1")==0))
   {
    printf("Artist Name : %s\n",&frame_data[1]);
   }
   else if((strcmp(frame_id,"TALB")==0))
   {
    printf("ALBUM : %s\n",&frame_data[1]);
   }
   else if((strcmp(frame_id,"TYER")==0))
   {
    printf("YEAR : %s\n",&frame_data[1]);
   }
   else if((strcmp(frame_id,"TCON")==0))
   {
    printf("GENRE : %s\n",&frame_data[1]);
   } 
   else if((strcmp(frame_id,"TCOM")==0))
   {
    printf("COMPOSER : %s\n",&frame_data[1]);
   }
   printf("\n");
    
    bytes_read=bytes_read+10+frame_data_size;
}  

}

void print_tag_data(FILE *org_mp3_fptr)
{

}
