/********************************************************************
	created:	2005/10/17											
	created:	17:10:2005   18:20									
	filename: 	ini_manage.h																						
	file base:	ini_manage											
	file ext:	h													
	author:		Chanchai Jaiyutitam									
																				
	purpose:	Header for manipulate ini file					

	updated:	2005/10/19
	updated by: Chanchai
	updated:	2008/12/10
	updated by:	wujunqiang
*********************************************************************/

#ifndef INI_MANAGE_H
#define INI_MANAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMENT		'#'
#define INI_ERROR	-1

#ifndef bool
 #define bool	int
#endif
#ifndef false
 #define false	0
#endif
#ifndef true
 #define true	1
#endif
/************************************************************************/
/* type def                                                             */
/************************************************************************/
typedef enum
{
	REPLACE = 1,
	NON_REPLACE = 0
}REPLACE_FLAG;

/************************************************************************/
/* for record of ini content                                            */
/************************************************************************/
struct record
{
	char comments[255];
	char key[255];
	char value[255];
	struct record *next;
};

/************************************************************************/
/* for content of file                                                  */
/************************************************************************/
struct section
{
	struct record *data_first;
	struct record *data_last;
	unsigned int data_size;
	char comments[255];
	char name[255];
	struct section *next;
};
/************************************************************************/
/* for content list                                                     */
/************************************************************************/
typedef struct
{
	unsigned int ini_section_size;
	struct section *first;
	struct section *last;
}content;

/************************************************************************/
/* main ini manage function                                             */
/************************************************************************/
/**
 *	function: ini_start														
 *	parameter:														
 *		@filename															
 *	return:	true if success														
 *	purpose: for start ini manipulate file														
 */
bool ini_start(const char* filename);

/**
 *	function:	ini_load													
 *	parameter:														
 *		@filename															
 *	return:		true if success													
 *	purpose:	for ini_load ini file to content													
 */

bool ini_load(const char *filename);

/**
 *	function:	_save													
 *	parameter:														
 *		@filename															
 *	return:		true if success													
 *	purpose:	ini_save content to ini file(ini_save as)													
 */
bool ini_save();	// ini_save to ini_load filebool 
bool ini_save_as(const char *filename);

/**
 *	function:	ini_get_value													
 *	parameter:														
 *		@															
 *	return:		value
 *	purpose:	?												
 */
char *ini_get_value (const char *sec,const char *key);
char *_get_value(const char *sec,const char *key,		// return data and comment
				 char *comment);

// added by wujunqiang,2008-12-10
/**
 *	function:	ini_get_int_value													
 *	parameter:														
 *		@															
 *	return:		int value
 *	purpose:	?												
 */
int ini_get_int_value (const char *sec,const char *key,int i_default);
/**
 *	function:	ini_set_int_value													
 *	parameter:														
 *		@															
 *	return:		true if success
 *	purpose:			
 */
bool ini_set_int_value  (const char *sec,const char *key,		// will auto replace
				 const int i_value);
// end of added,2008-12-10

/**
 *	function:	ini_set_value													
 *	parameter:														
 *		@															
 *	return:		true if success
 *	purpose:			
 */
bool ini_set_value  (const char *sec,const char *key,		// will auto replace
				 const char *value);	
bool _set_value (const char *sec,const char *key,		// select replace or not replace
				 const char *value,const char *comment,REPLACE_FLAG flag);

int  ini_remove_sel (const char *sec,char *key);
int  int_remove_all (const char * sec);				// remove all record in section

// add/remove section
void ini_add_section(const char *sec,const char *comment);	// add section
int  ini_remove_section(char *sec);				// remove section (remove all record in section if not empty)

void ini_clear();								// ini_clear all content

// size of section
int  ini_content_size();
int  ini_section_size(char *sec);

// for console display use stdio.h stdout
void ini_print_content();						// print all content
void print_section();						// print all only section
void print_record(char *sec,char *key);		// print selection record
void print_allrecord(char *sec);			// print all record in section

/**
 *	function: ini_end													
 *	parameter:														
 *		none																	
 *	return:	void														
 *	purpose: for end ini manipulate file													
 */
void ini_end();
void _ini_end(REPLACE_FLAG flag);

/**
 *	function: ini_get_last_error														
 *	parameter:														
 *		none															
 *	return:	type of error														
 *	purpose:	for get error													
 */
char *ini_get_last_error();

/************************************************************************/
/*Helper function                                                       */
/************************************************************************/ 
// initaial data/ini_save
void init_content();
bool _save(const char *filename);
// add/remove record
void _append(const char *sec,const char *key,			// append data to section
			 const char *value,const char *comment);											
int  _remove(const char *sec,const char *key);
int  _remove_all(struct section *sec);
// display function
void _print_allrecord(struct record *sec);	// print all record in section
// search section
struct section *ini_get_section(const char *sec);		// search section
struct record *ini_get_record(struct section *sec,const char *key);	// get record
// ini_trim
void ini_trim(char *buffer);
#endif
