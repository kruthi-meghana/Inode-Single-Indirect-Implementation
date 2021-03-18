#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "stdmessages.h"

void create_groups(struct groups *group, char* title, char* description, char *userName, FILE* fp) {
	
	int direct_message_links[10] = { -1 };
	int indirect_message_links[5] = { -1 };

	for (int i = 0; i < DIRECT_BLOCK_SIZE; i++)
		group->message_direct[i] = -1;
	for (int i = 0; i < INDIRECT_BLOCK_SIZE; i++)
		group->message_indirect[i] = -1;
	strcpy(group->user_admin, "Admin");
	strcpy(group->group_title, title);
	strcpy(group->description, description);
	group->user_information = (create_group_users(fp) - (1024*1024+3))/256;
	group->message_count = 0;
}

void get_group_details(int group_number, struct groups* group, FILE* fp) {

	int address = DATA_BLOCK_POSITION + (BLOCK_SIZE * (group_number) + (BLOCK_SIZE*10));
	fseek(fp, address, SEEK_SET);
	fread(&group, sizeof(struct groups), 1, fp);
}

void enter_group(int group_number, FILE* fp, char* username) {
	int option;
	struct groups group;
	char message_body[MESSAGE_SIZE];
	int address = DATA_BLOCK_POSITION + (BLOCK_SIZE * (group_number)+(BLOCK_SIZE * 10));
	fseek(fp, address, SEEK_SET);
	fread(&group, sizeof(struct groups), 1, fp);
	
	printf("\t\t****************************************************************\n");
	printf("\t\t**                 Welcome to Group: %s              **\n", group.group_title);
	printf("\t\t****************************************************************\n");

	add_new_users(&group, username, fp, group_number);

	do {
		printf("\t\t%s! select a choice from the below\n", username);
		printf("\t\t1. Message Viewer\n\t\t2. Users Viewer\n\t\t3. Add Message\n\t\t4. Exit\n");
		scanf("%d", &option);
		switch (option)
		{
		case 1: message_viewer(&group, fp, username);
			break;
		case 2: users_viewer(&group, fp, group_number);
			break;
		case 3: add_message(fp, &group, username);
			break;
		}
		fseek(fp, address, SEEK_SET);
		fwrite(&group, sizeof(struct groups), 1, fp);

		fflush(fp);

	} while (option != 4);
}


