#include "mp3_header.h"

int validate_cla(int argc, char *argv[], struct MP3 *mp3)
{
    if( argc == 1 )
    {
	printf("------------------------------------------------------------------------------\n");
	printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
	printf("USAGE : To view please pass like: ./a.out -v mp3filename\n");
	printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c changing_text mp3filename\n");
	printf("To get help pass like : ./a.out --help\n");
	printf("------------------------------------------------------------------------------\n");
	return 0;
    }

    if( (strcmp(argv[1], "--help") == 0) )
    {
	printf("------------------------------->HELP<-----------------------------------------\n");
	printf("1. -v -> to view mp3 file contents\n");
	printf("2. -e -> to edit mp3 file contents\n");
	printf("\t\t2.1. -t -> to edit song title\n");
	printf("\t\t2.2. -a -> to edit artist name\n");
	printf("\t\t2.3. -A -> to edit album name\n");
	printf("\t\t2.4. -y -> to edit year\n");
	printf("\t\t2.5. -m -> to edit content\n");
	printf("\t\t2.6. -c -> to edit comment\n");
	printf("------------------------------------------------------------------------------\n");
	return 0; 
    }

    if(!strcmp(argv[1], "-v"))
    {
	// Do the necessary validation

	return VIEW;
    }
    
    if(!strcmp(argv[1], "-e"))
    {
	// Do the necessary validation

	return EDIT;
    }

}


void toggle_endianess(struct MP3 *mp3, int *size)
{


}

