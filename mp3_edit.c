#include "mp3_header.h"
#include<stdlib.h>

void edit(struct MP3 *mp3)
{
   
  mp3->org_mp3_fptr=fopen(mp3->mp3_filename,"rb");
  
    if(mp3->org_mp3_fptr==NULL)
    {
        printf("ERROR : Unable to open the File\n");
        return;
    }
    //printf("Opened the Original File\n");
    mp3->dup_mp3_fptr=fopen("temp.mp3","wb");//it should presrve the original file ,we can write the edited tags in this dupliacte mp3 file(purpose)
    if(mp3->dup_mp3_fptr==NULL)
    {
        printf("ERROR : Umable to create duplicate MP3 file\n");
        fclose(mp3->org_mp3_fptr);
        return;
    }
    unsigned char header[10];
    if(fread(header,sizeof(char),10,mp3->org_mp3_fptr)!=10)
    {
        printf("ERROR : Unable to read 10 bytes of Header from Original File\n");
        fclose(mp3->org_mp3_fptr);
        fclose(mp3->dup_mp3_fptr);
        return;
    }
    //checking header is valid or not,if valid copied into the duplicate
    if(memcmp(header,"ID3",3)!=0 || header[3]!=3 || header[4]!=0)
    {
    printf("ERROR : Invalid ID3v2.3 File\n");
    fclose(mp3->org_mp3_fptr);
    fclose(mp3->dup_mp3_fptr);
    return;
    }
    fwrite(header,sizeof(char),10,mp3->dup_mp3_fptr);
    //reading the frame id and storing in array
   
   //convert the edit option into frame id
   char required_frame[5];
   if(strcmp(mp3->edit_option,"-t")==0)
   {
    strcpy(required_frame,"TIT2");
   }
   else if(strcmp(mp3->edit_option,"-a")==0)
   {
    strcpy(required_frame,"TPE1");
   }
   else if(strcmp(mp3->edit_option,"-A")==0)
   {
    strcpy(required_frame,"TALB");
   }
   else if(strcmp(mp3->edit_option,"-y")==0)
   {
    strcpy(required_frame,"TYER");
   }
   else if(strcmp(mp3->edit_option,"-m")==0)
   {
    strcpy(required_frame,"TCON");
   }
   else if(strcmp(mp3->edit_option,"-c")==0)
   {
    strcpy(required_frame,"TCOM");
   }
   unsigned int old_tag_size;
   unsigned int remaining_size;
   old_tag_size = ((header[6] & 0x7F) << 21)|
               ((header[7] & 0x7F) << 14)|
               ((header[8] & 0x7F) << 7)|
               (header[9] & 0x7F);
    remaining_size = old_tag_size;
   while(remaining_size>=10)
   {
     char frame_id[5];
   if(fread(frame_id,sizeof(char),4,mp3->org_mp3_fptr)!=4)
   {
    printf("ERROR : Unable to read frame ID\n");
    fclose(mp3->org_mp3_fptr);
    fclose(mp3->dup_mp3_fptr);
    return;
   }
   frame_id[4]='\0';
   printf("Frame ID : %s\n",frame_id);
  //after converting edit option into frame id ,now we are comparing the frame_id we read from original and edit option converted
  unsigned char frame_size[4];//but these 4 bytes are stored in big-endian format in ID3v2.3.
  unsigned int frame_data_size;
  unsigned char flag_buffer[2];
  unsigned char *frame_data;
  
  if(fread(frame_size,sizeof(char),4,mp3->org_mp3_fptr)!=4)
  {
   printf("ERROR : The Frame size is not valid for this frame ID\n");
   fclose(mp3->org_mp3_fptr);
   fclose(mp3->dup_mp3_fptr);
   return;
  }
  frame_data_size=(((unsigned int)frame_size[0])<<24)|
                  (((unsigned int)frame_size[1])<<16)|
                  (((unsigned int)frame_size[2])<<8)|
                  (((unsigned int)frame_size[3]));

    //validating frame size              
   if(frame_data_size > remaining_size - 10)
   {
    printf("ERROR: Invalid frame size\n");
    fclose(mp3->org_mp3_fptr);
    fclose(mp3->dup_mp3_fptr);
    return;
  }               
  frame_data=malloc(frame_data_size+1);
  if(frame_data==NULL)
  {
    printf("Memory allocation failed\n");
    fclose(mp3->dup_mp3_fptr);
    fclose(mp3->org_mp3_fptr);
    return;
  }
  printf("Frame_size for particular frame id is %u\n",frame_data_size);
  //after this 2 bytes of buffer
  if(fread(flag_buffer,sizeof(char),2,mp3->org_mp3_fptr)!=2)
  {
   printf("ERROR : Unable to read frame flags\n");
   free(frame_data);
   fclose(mp3->org_mp3_fptr);
   fclose(mp3->dup_mp3_fptr);
   
   return;
  }
  if(fread(frame_data,sizeof(char),frame_data_size,mp3->org_mp3_fptr)!=frame_data_size)
  {
      printf("ERROR : Unable to read frame data\n");
      free(frame_data);
      fclose(mp3->org_mp3_fptr);
      fclose(mp3->dup_mp3_fptr);
      return;
  }
   
   frame_data[frame_data_size]='\0';
   if(strcmp(frame_id,required_frame)==0)
   {
       //printf("Frame id found\n");
       
       printf("Encoding byte: %d\n", frame_data[0]);
       //editing actual data
       unsigned int new_frame_size;
       new_frame_size=1+strlen(mp3->new_tag_value);
       //creating new buffer
       unsigned char new_frame_data[new_frame_size];
       new_frame_data[0]=frame_data[0];//assgining the encoding bit
       memcpy(&new_frame_data[1],mp3->new_tag_value,strlen(mp3->new_tag_value));//copies the new text from index 1
       
       unsigned char new_size[4];
       new_size[0]=(new_frame_size>>24)&0xFF;
       new_size[1]=(new_frame_size>>16)&0xFF;
       new_size[2]=(new_frame_size>>8)&0xFF;
       new_size[3]=new_frame_size&0xFF;
       unsigned int new_tag_size;
       
       
    new_tag_size = old_tag_size - frame_data_size + new_frame_size;
    header[6] = (new_tag_size >> 21) & 0x7F;
    header[7] = (new_tag_size >> 14) & 0x7F;
    header[8] = (new_tag_size >> 7) & 0x7F;
    header[9] = new_tag_size & 0x7F;
    printf("NEW Tag size is : %u\n",new_tag_size);
    fwrite(frame_id,sizeof(char),4,mp3->dup_mp3_fptr);
    fwrite(new_size,sizeof(char),4,mp3->dup_mp3_fptr);
    fwrite(flag_buffer,sizeof(char),2,mp3->dup_mp3_fptr);
    fwrite(new_frame_data,sizeof(char),new_frame_size,mp3->dup_mp3_fptr);
    long current_pos = ftell(mp3->dup_mp3_fptr);

   fseek(mp3->dup_mp3_fptr, 0, SEEK_SET);

   fwrite(header, sizeof(char), 10, mp3->dup_mp3_fptr);

   fseek(mp3->dup_mp3_fptr, current_pos, SEEK_SET);
}else{
   //copying elements from original to duplicate directly
   fwrite(frame_id, 1, 4, mp3->dup_mp3_fptr);
   fwrite(frame_size, 1, 4, mp3->dup_mp3_fptr);
   fwrite(flag_buffer, 1, 2, mp3->dup_mp3_fptr);
   fwrite(frame_data, 1, frame_data_size, mp3->dup_mp3_fptr);
}

remaining_size=remaining_size-(10+frame_data_size);
free(frame_data);
}
unsigned char buffer[4096];
size_t bytes_read;

while((bytes_read = fread(buffer, 1, sizeof(buffer), mp3->org_mp3_fptr)) > 0)
{
    fwrite(buffer, 1, bytes_read, mp3->dup_mp3_fptr);
}
fclose(mp3->org_mp3_fptr);
fclose(mp3->dup_mp3_fptr);
}

