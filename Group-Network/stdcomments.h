#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdusers.h"


void prepare_comment(struct comments* comment, char* username, char* comment_body) {

	comment->comment_likes_count = 0;
	strcpy(comment->user_name, username);
	strcpy(comment->comment_body, comment_body);

}

void view_comments(FILE* fp, long int address) {
	struct blockPointers blck_ptr;
	struct messages message;
	struct comments comment;
	fseek(fp, address, SEEK_SET);
	fread(&message, sizeof(struct messages), 1, fp);
	printf("\t\t%s: %s\n", message.user_name, message.message_body);
	printf("\t\t*******************************************\n");
	if (message.comments_count == 0) {
		printf("\t\t\tNo comments on the post\n");
		return;
	}
	
	for (int i = 0; i < message.comments_count; i++) {
		if (i < 10) {
			fseek(fp, message.comment_direct[i], SEEK_SET);
		}
		else {
			fseek(fp, message.comment_indirect[i / BLOCK_POINTERS], SEEK_SET);
			fread(blck_ptr.pointers, sizeof(blck_ptr.pointers), 1, fp);
			fseek(fp, blck_ptr.pointers[i - 10], SEEK_SET);
		}
		fread(&comment, sizeof(struct comments), 1, fp);
		printf("\t\t\t%s: %s\n",comment.user_name, comment.comment_body);
	}
	printf("\n\t\tReached END!\n\t\t******************************************\n");

}

void add_comment(FILE* fp, char* username, long int msg_addr) {
	char comment_body[COMMENT_SIZE];
	struct comments comment;
	int index = 0, i = 0;
	struct blockPointers blck_ptr;
	struct messages message;
	fseek(fp, msg_addr, SEEK_SET);
	fread(&message, sizeof(struct messages), 1, fp);
	printf("\t\tPlease enter your message");
	scanf(" %[^\n]s", comment_body);
	prepare_comment(&comment, username, comment_body);
	long int address = free_block_position(fp, 1);

	if (message.comments_count < DIRECT_BLOCK_SIZE) {
		while (index < DIRECT_BLOCK_SIZE) {
			if (message.comment_direct[index] == -1)
				break;
			index++;
		}
		message.comment_direct[index] = address;
	}
	else {
		while (index < INDIRECT_BLOCK_SIZE) {
			if (message.comment_indirect[index] == -1) {
				message.comment_indirect[index] = create_indirect_pointers_block(fp, &blck_ptr);
				blck_ptr.pointers[0] = address;
				break;
			}
			else {
				fseek(fp, message.comment_indirect[index], SEEK_SET);
				fread(&blck_ptr, sizeof(blck_ptr), 1, fp);
				for (i = 0; i < BLOCK_POINTERS; i++) {
					if (blck_ptr.pointers[i] == -1) {
						blck_ptr.pointers[i] = address;
						fseek(fp, message.comment_indirect[index], SEEK_SET);
						fwrite(&blck_ptr, sizeof(blck_ptr), 1, fp);
						break;
					}
				}
			}
			if (i < BLOCK_POINTERS)
				break;
		}
		if (i == 5) {
			printf("\n No more comments are allowed at this point due to full storage.\n");
			return;
		}
	}
	message.comments_count++;
	fseek(fp, address, SEEK_SET);
	fwrite(&comment, sizeof(struct comments), 1, fp);
	fseek(fp, msg_addr, SEEK_SET);
	fwrite(&message, sizeof(struct messages), 1, fp);
}
