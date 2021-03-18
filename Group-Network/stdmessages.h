#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdcomments.h"


long int get_message(int msg_no, struct groups* grp, FILE* fp) {
	
	long int address;
	if (msg_no < 10) {
		address = grp->message_direct[msg_no];
	}
	else {
		struct blockPointers ptrs;
		fseek(fp, grp->message_indirect[msg_no / BLOCK_POINTERS], SEEK_SET);
		fread(ptrs.pointers, sizeof(ptrs.pointers), 1, fp);
		address = ptrs.pointers[msg_no - 10];
	}
	return address;	
}

void like_message(struct groups* grp, FILE* fp, long int address) {
	
	struct messages message;
	fseek(fp, address, SEEK_SET);
	fread(&message, sizeof(struct messages), 1, fp);
	message.likes_count++;
	fseek(fp, address, SEEK_SET);
	fwrite(&message, sizeof(struct messages), 1, fp);
}

int display_message( FILE* fp, struct groups* grp, int start, int end) {
	int i;
	struct messages message;
	long int address;
	for (i = start; i < end && i < grp->message_count; i++) {
		address = get_message(i, grp, fp);
		fseek(fp, address, SEEK_SET);
		fread(&message, sizeof(struct messages), 1, fp);
		printf("\t\t%d) %s: %s\n",i+1,message.user_name, message.message_body);
	}
	if (i == grp->message_count)
		printf("\n\t\tReached END!\n\t\t******************************************\n");	

	return i;
}

void message_viewer(struct groups* group, FILE* fp, char* username) {
	if (group->message_count == 0) {
		printf("\t\t\tNo conversation in the group \n");
		return;
	}
	long int addr;
	int option, msg_no;
	int displayed_count = display_message(fp, group, 0, 10);
	printf("\t\t******************************************\n");
	do {
		if (displayed_count < group->message_count) {
			printf("\t\t%s! Here are the posts retrieved from the group\nchoose next to view the next page\n", username);
			printf("\t\t*******************************************\n");
			printf("\n\t\t%s! select a choice from the below\n", username);
			printf("\t\t1. Next \n\t\t2. Like a post\n\t\t3. Comment on a post\n\t\t4. view comments\n\t\t5. Back\n");
			int item = scanf("%d", &option);
			if (option == 1) {
				displayed_count = display_message(fp, group, displayed_count, displayed_count + 10);
			}
			else {
				printf("\n\t\tPlease Enter the message number you wish to respond\n");
				item = scanf("%d", &msg_no);
				addr = get_message(msg_no, group, fp);
				if (option == 2) {
					like_message(group, fp, addr);
				}
				else {
					add_comment(fp, username, addr);
				}
			}
		}
		else {
			printf("\t\t*******************************************\n");
			printf("\n\t\t%s! select a choice from the below\n", username);
			printf("\t\t1. Like a post\n\t\t2. Comment on a post\n\t\t3. View comments\n\t\t4. Back\n");
			int item = scanf("%d", &option);
			if (option != 4) {
				printf("\n\t\tPlease Enter the message number you wish to respond\n");
				item = scanf("%d", &msg_no);
				addr = get_message(msg_no-1, group, fp);
				switch (option) {
				case 1: like_message(group, fp, addr);
					break;
				case 2: add_comment(fp, username, addr);
					break;
				case 3: view_comments(fp, addr);
					break;
				}
			}
		}
	} while ((displayed_count < group->message_count && option != 5) && (displayed_count >= group->message_count && option != 4));

	
}


void prepare_message(struct messages* message, char* username, char* message_body) {

	int direct[10] = { -1 };
	int indirect[5] = { -1 };

	message->comments_count = 0;
	for (int i = 0; i < DIRECT_BLOCK_SIZE; i++) {
		message->comment_direct[i] = -1;
	}
	for (int i = 0; i < INDIRECT_BLOCK_SIZE; i++) {
		message->comment_indirect[i] = -1;
	}
	message->likes_count = 0;
	strcpy(message->user_name, username);
	strcpy(message->message_body, message_body);

}

void add_message(FILE* fp, struct groups* group, char* username) {
	struct messages message;
	char message_body[MESSAGE_SIZE];
	long int address;
	int index = 0, i = 0;
	struct blockPointers blck_ptr;

	printf("\t\tPlease enter your message");
	scanf(" %[^\n]s", message_body);

	prepare_message(&message, username, message_body);
	address = free_block_position(fp, 2);

	if (group->message_count < DIRECT_BLOCK_SIZE) {
		while (index < DIRECT_BLOCK_SIZE) {
			if (group->message_direct[index] == -1)
				break;
			index++;
		}
		group->message_direct[index] = address;
	}
	else {
		while (index < INDIRECT_BLOCK_SIZE) {
			if (group->message_indirect[index] == -1) {
				group->message_indirect[index] = create_indirect_pointers_block(fp, &blck_ptr);
				blck_ptr.pointers[0] = address;
				break;
			}
			else {
				fseek(fp, group->message_indirect[index], SEEK_SET);
				fread(&blck_ptr, sizeof(blck_ptr), 1, fp);
				for (i = 0; i < BLOCK_POINTERS; i++) {
					if (blck_ptr.pointers[i] == -1) {
						blck_ptr.pointers[i] = address;
						break;
					}
				}
				fseek(fp, group->message_indirect[index], SEEK_SET);
				fwrite(&blck_ptr, sizeof(blck_ptr), 1, fp);
			}
			if (i < BLOCK_POINTERS)
				break;
		}
		if (i == 5) {
			printf("\n\t\tNo more posts are allowed at this point due to full storage.\n");
			return;
		}
	}
	group->message_count++;
	fseek(fp, address, SEEK_SET);
	fwrite(&message, sizeof(struct messages), 1, fp);
}
