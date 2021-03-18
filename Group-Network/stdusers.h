#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdpointers.h"

long int create_group_users(FILE* fp) {
	
	struct users user;
	struct users user1;
	strcpy(user.user_name[0], "Admin");
	user.users_count = 1;
	long int start_address = free_block_position(fp, 2);
	fseek(fp, start_address, SEEK_SET);
	fwrite(&user, sizeof(struct users), 1, fp);
	
	return start_address;
}

void add_new_users(struct groups* group, char* user_name, FILE* fp, int group_no) {

	struct users group_users;
	long int addr = DATA_BLOCK_POSITION + (BLOCK_SIZE * 2) * (group_no);
	fseek(fp, addr, SEEK_SET);
	fread(&group_users, sizeof(struct users), 1, fp);

	strcpy(group_users.user_name[group_users.users_count], user_name);
	group_users.users_count++;
	fseek(fp, addr, SEEK_SET);
	fwrite(&group_users, sizeof(struct users), 1, fp);
}

void users_viewer(struct groups* group, FILE* fp, int group_no) {
	struct users group_users;
	long int addr = DATA_BLOCK_POSITION + (BLOCK_SIZE*2) * (group_no);
	fseek(fp, addr, SEEK_SET);
	fread(&group_users, sizeof(struct users), 1, fp);
	printf("\t\t******************************************\n");
	printf("\t\tBelow are the list of names in this group:\n\n");
	for (int i = 0; i < group_users.users_count; i++) {
		printf("\t\t\t%d) %s\n", (i+1), group_users.user_name[i]);
	}
	printf("\n\t\t******************************************\n");
}