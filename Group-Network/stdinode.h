
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#define SIGNATURE "KM"
#define USER_ID_SIZE 8
#define USERNAME 28
#define GROUP_COUNT 5
#define GROUP_DESCRIPTION 30
#define GROUP_TITLE 20
#define MESSAGE_SIZE 180
#define COMMENT_SIZE 112
#define SIGNATURE_LEN 2
#define BLOCK_SIZE 128
#define DIRECT_BLOCK_SIZE 10
#define INDIRECT_BLOCK_SIZE 5
#define BLOCK_POINTERS 32 /* Each block has 128 bits, 128 / sizeof(int) = 32*/
#define TOTAL_NO_BLOCKS 8192 /*(2^20)/128*/
#define DATA_BLOCK_POSITION (1024*1024)+(3)
#define FILE_LOCATION "./networkdata.bin"

/*
* @author: Kruthi Meghana Anumandla
*
* Using a 1MB bin file, Hence the Inode data structure is implemented using Direct and Single Indirect Block Pointers
* Below are the structure definitions of Users, Groups, Messages, Comments, and Block Pointers.
* 
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct users {
	char user_name[USERNAME][USER_ID_SIZE];
	int users_count;
}; //size of users is 256-bits = BLOCK_SIZE * 2

struct groups {
	char group_title[GROUP_TITLE];
	int message_count;
	char description[GROUP_DESCRIPTION];
	int message_direct[DIRECT_BLOCK_SIZE];
	int message_indirect[INDIRECT_BLOCK_SIZE];
	char user_admin[10];
	int user_information;
}; //Size of groups is 128-bits = BLOCK_SIZE

struct messages {
	char user_name[USER_ID_SIZE];
	int likes_count;
	int comments_count;
	char message_body[MESSAGE_SIZE];
	int comment_direct[DIRECT_BLOCK_SIZE];
	int comment_indirect[INDIRECT_BLOCK_SIZE];
}; //size of messages is 256-bits = BLOCK_SIZE * 2

struct comments {
	char user_name[USER_ID_SIZE];
	int comment_likes_count;
	char comment_body[COMMENT_SIZE];
}; //size of comments is 128-bits = BLOCK_SIZE




struct blockPointers {
	int pointers[BLOCK_POINTERS];
}; //size of blockPointers is 128-bits = BLOCK_SIZE

/*Initiating the Block Pointers and pointing them to -1*/

int verify_free_block_position(int bits_occupied, FILE* fp, int no_of_blocks) {

	int current_value = 255 & bits_occupied;

	if (current_value == 255)
		return -1;
	if (no_of_blocks == 1) {
		for (int bit = 0; bit < 8; bit++) {
			if (current_value % 2 == 0)
				return bit;
			current_value = current_value >> 1;
		}
	}
	else {
		int bit, bit1, bit2;
		for (bit = 0; bit < 8; bit++) {
			bit1 = current_value % 2;
			current_value = current_value >> 1;
			bit2 = current_value % 2;

			if (bit1 == 0 && bit2 == 0)
				break;
		}
		if (bit != 7)
			return bit;
	}
	return -1;
}

long int free_block_position(FILE* fp, int no_of_blocks) {
	
	fseek(fp, 3, SEEK_SET);
	unsigned char free_block[128];
	int block_position = -1, count, bit;


	/*There are two types of block requests: 1 block (128-bits) or 2 blocks (256-bits) 
	* We have initialised 128 * 8192 bits to 0, now inorder to keep a track of occupied blocks,
	* We perform bitwise operations, so that whene ever:
	*                        1) single block is occupied we perfom bitwise left shit with one.
	*				         2) two blocks are occupied we perform bitwsie left shift with three.
	*                        3) If the value in the bit is 255 we move to the next bit. Becuase the means all the 8-bits are occupied.
	* Hence at every free block request, we need to know if the block is occupied by checking the bit values.
	*/

	for (count = 1; count <= TOTAL_NO_BLOCKS; count++) {
		fread(free_block, sizeof(free_block), 1, fp);

		for (bit = 0; bit < 128; bit++) {
			block_position = verify_free_block_position(free_block[bit], fp, no_of_blocks);
			if (block_position >= 0)
				break;
		}

		if (block_position >= 0)
			break;
	}
	int shift_bit = (no_of_blocks == 1)? 1: 3;
	shift_bit = shift_bit << block_position;
	free_block[bit] = shift_bit | free_block[bit];

	/*Update the value in the file*/
	fseek(fp, -(count * BLOCK_SIZE), SEEK_CUR);
	fseek(fp, bit, SEEK_CUR);
	fwrite(&free_block[bit], sizeof(free_block[bit]), 1, fp);

	return (DATA_BLOCK_POSITION + ((count - 1)*BLOCK_SIZE + bit*8 + block_position) * BLOCK_SIZE);
}



