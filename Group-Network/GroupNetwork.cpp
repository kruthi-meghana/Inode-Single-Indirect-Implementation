#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "stdgroup.h"
#include <stdlib.h>

/*
* @author: Kruthi Meghana Anumandla
* This file initiates the application, by creating file, creating groups, setting the file values etc.
*/
void display_user_interface(FILE* fp) {

	char user_name[8];
	int option = -1;

	printf("\t\t****************************************************************\n");
	printf("\t\t**                 Welcome to Group Network                   **\n");
	printf("\t\t****************************************************************\n");
	printf("\n\t\tHi! I am bot! I will be directing you through this application.\n\t\tI shall Provide you with the menu at every step to understand your choice\n\t\tBefore starting our application, provide us your name\n");
	scanf(" %[^\n]s", user_name);
	printf("\n\t\tHi! %s\n", user_name);
	do {
		printf("\t\tChoose a group to enter the forum\n");
		printf("\t\t1. DL Forum\n\t\t2. Data structures and Algorithms\n\t\t3. Linux OS\n\t\t4. ML Forum\n\t\t5. AI Forum\n\t\t6. Logout\n");
		scanf("%d", &option);
		if (option != 6)
			enter_group(option-1, fp, user_name);
	} while (option != 6);

}

void create_file() {

	FILE* inode_file = NULL;
	inode_file = fopen(FILE_LOCATION, "wb+");
	fwrite(SIGNATURE, sizeof(SIGNATURE), 1, inode_file); /* Inserting my Signature = KM. Usually this is referred to as MAGIC in Linux Inode structures*/

	/* Below are the group details, creating 5 groups, Initializing their titles, descriptions, Single direct and indirect Link to = -1*/
	struct groups groups[5];
	char group_description[GROUP_COUNT][GROUP_DESCRIPTION] = { "Discussions related to DL.", "Problem solving discussions", "Underlying concepts of Linux", "Algorithms of ML", "AI applications and business" };
	char group_titles[GROUP_COUNT][GROUP_TITLE] = { "Deep Learning", "Data Structures", "Linux OS", "Machine Learning", "AI Forums" };
	
	char user[6] = "Admin";

	/* Before we create the groups, lets fill the entire file with zeros, so we can use this later in finding the empty blocks position.*/
	int file_fill[128] = { 0 };

	for (int count = 0; count < TOTAL_NO_BLOCKS; count++) {
		fwrite(file_fill, sizeof(file_fill), 1, inode_file);
	}

	/*Creating the groups and writing them to the inode*/
	for (int i = 0; i < GROUP_COUNT; i++) {
		create_groups(&groups[i], group_titles[i], group_description[i], user, inode_file);
	}
	for (int i = 0; i < GROUP_COUNT; i++) {
		long int start_address = free_block_position(inode_file, 1);
		fseek(inode_file, start_address, SEEK_SET);
		fwrite(&groups[i], sizeof(groups[i]), 1, inode_file);
	}
	fflush(inode_file);
	fclose(inode_file);
}

int main() {

	if (fopen(FILE_LOCATION, "rb+") == NULL) {
		create_file();
	}
	FILE* fp = NULL;
	fp = fopen(FILE_LOCATION, "rb+");
	display_user_interface(fp);

	fflush(fp);
	fclose(fp);
}







