#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry_i {
    unsigned long  st_ino;
	unsigned short st_mode;
	char * name;
};

struct t_1 {
	char * name1;
	char * name2;
	char * name3;
	char * name4;
};

struct zero_t {
};


void prin_struct(){
	printf("========================== struct ===========================\n");
	printf("size(struct entry_i)=%d \n", 	sizeof(struct entry_i));
	printf("size(struct entry_i*)=%d \n", 	sizeof(struct entry_i *));
	printf("8 + 2(aglin to 8) + 8 = 24 \n\n");

	printf("size(struct zero)=%d \n", 		sizeof(struct zero_t));
	printf("size(struct zero*)=%d \n\n", 	sizeof(struct zero_t *));
}

void prin_normal(){
	printf("========================== normal ===========================\n");
	printf("size(char)=%d \n", 				sizeof(char ));
	printf("size(char *)=%d \n\n", 			sizeof(char *));

	printf("size(short)=%d \n", 			sizeof(short ));
	printf("size(short *)=%d \n\n", 		sizeof(short *));

	printf("size(unsigned short)=%d \n", 		sizeof(unsigned short ));
	printf("size(unsigned short *)=%d \n\n", 	sizeof(unsigned short *));

	printf("size(int) =%d \n", 				sizeof(int ));
	printf("size(int *)=%d \n\n", 			sizeof(int *));

	printf("size(unsigned int) =%d \n", 	sizeof(unsigned int ));
	printf("size(unsigned int *)=%d \n\n", 	sizeof(unsigned int *));

	printf("size(long)=%d \n", 				sizeof(long ));
	printf("size(long *)=%d \n\n", 			sizeof(long *));

	printf("size(unsigned long)=%d \n", 		sizeof(unsigned long ));
	printf("size(unsigned long *)=%d \n\n", 	sizeof(unsigned long *));

	printf("size(float)=%d \n", 			sizeof(float ));
	printf("size(float *)=%d \n\n", 		sizeof(float *));

	printf("size(double)=%d \n", 			sizeof(double ));
	printf("size(double *)=%d \n\n", 		sizeof(double *));
}

struct name_t {
	char * name;
};

void char_point(){
	printf("========================== char point test ===========================\n");
	char * char_t;
	printf("addr(no init)=%p \n",				char_t);
	printf("size(no init)=%d \n",				strlen(char_t));
	printf("cont(content)=%s \n\n",				char_t);

	char_t = malloc(sizeof(char) * 64);
	printf("addr(   init)=%p \n",				char_t);
	printf("size(   init)=%d \n",				strlen(char_t));
	printf("cont(content)=%s \n\n",				char_t);

	char_t[0] = 'f';
	char_t[1] = 'e';
	char_t[2] = 'i';
	char_t[10] = '\0';
	printf("addr(   init)=%p \n",				char_t);
	printf("size(   init)=%d \n",				strlen(char_t));
	printf("cont(content)=%s \n\n",				char_t);

	struct name_t name;
	printf("size name(no init)=%d \n",			sizeof(name));
	printf("addr name(no init)=%p \n",			name.name);
	printf("cont name(content)=%s \n",			name.name);

	name.name = malloc(sizeof(char) * 5);
	name.name[0] = 'y';
	name.name[1] = 'i';
	name.name[2] = 'n';
	name.name[3] = '\0';
	printf("size-sizeof name(   init)=%d \n",			sizeof(name));
	printf("addr name-struct(   init)=%p \n",			name.name);
	printf("addr name-struct(&  init)=%p \n",			&name.name);
	printf("addr name-st-out(   init)=%p \n",			name);
	printf("addr name-st-out(&  init)=%p \n",			&name);
	printf("name-st-out and name-st-out& is not the same!!! why? "
			" name-st-out& it the point of char point.\n");
	printf("cont name(content)='%s' \n",				name.name);
	printf("cont name(content out)='%s' \n",			name);
	printf("size-struct name(   init)=%d \n",			strlen(name.name));
	printf("size-st-out name(   init)=%d \n",			strlen(&name));
	printf("size-st-out is not the same size each time!!! why?\n");
}

void main(){
	//prin_normal();
	//prin_struct();

	char_point();
}
