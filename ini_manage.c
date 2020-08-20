/************************************************************************
	created:	2005/10/17											
	created:	17:10:2005   18:20				
	filename: 	ini_manage.c																									
	file base:	ini_manage											
	file ext:	c											
	author:		Chanchai Jaiyutitam													
																				
	purpose:	ini file management 	
	
	updated:	2005/10/19
	updated by: Chanchai
	updated:	2008/12/10
	updated by:	wujunqiang
	updated:	2008/12/14
	updated by:	wujunqiang
*************************************************************************/

#include "ini_manage.h"

content *ini_content;
char gfilename[255];
char *error_msg;
REPLACE_FLAG w_flag;

//////////////////////////////////////////////////////////////////////////
//	ini start
//  [10/18/2005]
bool ini_start(const char* filename)
{
	init_content();
	
	error_msg = NULL;
	w_flag = REPLACE;
	memcpy(gfilename,filename,sizeof(gfilename));
	
	if(ini_load(gfilename) == false)
	{
		error_msg = "initial parse file error";
		return false;
	};
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ini end
// [10/19/2005]
void _ini_end(REPLACE_FLAG flag)
{
	if(flag == REPLACE)
		ini_save();
	ini_clear();
}

//////////////////////////////////////////////////////////////////////////
// end ini
// [02/14/2009]
void end_ini()
{
	ini_clear();
}

//////////////////////////////////////////////////////////////////////////
// ini end
void ini_end()
{
	_ini_end(REPLACE);
}

//////////////////////////////////////////////////////////////////////////
// ini_load
//  [10/18/2005]
bool ini_load(const char *filename)
{
	FILE *in_stream;
	char buffer[255];
	char comments[1024];
	char current_section[255];	
	char key[255];
	char value[255];
	char *pdest;
	int  index;

	strcpy(comments,"");
	strcpy(current_section,"");
	error_msg = NULL;

	if( (in_stream = fopen( filename, "r" )) != NULL )
	{
		while(fgets(buffer,sizeof(buffer),in_stream) != NULL)
		{
			ini_trim(buffer);
			switch(buffer[0])
			{
				case '[' : // section;
					pdest = strrchr(buffer,']');
					if (pdest == NULL)
					{
						fclose(in_stream);
						error_msg = "parse ini error";
						return false;
					}
					index = pdest - buffer;
					memcpy(current_section,buffer + 1,index - 1);
					current_section[index - 1] = '\0';
					ini_add_section(current_section,comments);	
					strcpy(comments,"");
					break;
				case '#' : // comment
				case ';' :
					if(strlen(comments) > 0)
						strcat(comments,"\n");
					strcat(comments,buffer);
					break;
				default : // find content
					pdest = strrchr(buffer,'=');
					if (pdest == NULL ) 
					{
						if(strlen(comments) > 0)
							strcat(comments,"\n");
						strcat(comments,buffer);
						break;
					}
					index = pdest - buffer;
					memcpy(key,buffer,index);
					key[index] = '\0';
					memcpy(value,buffer + index + 1,strlen(buffer)-index);
					
					if(strcmp(current_section,"") == 0)
					{
						fclose(in_stream);
						error_msg = "parse ini error";
						return false;
					}
					else
					{
						_append(current_section,key,value,comments);
						strcpy(comments,"");
					}
					break;
			}
		}
		fclose(in_stream);
	}
	else
	{
		error_msg = "open file error";
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// _save
//  [10/18/2005]
bool _save(const char *filename)
{
	FILE *stream;
	struct section *sec = ini_content->first;
	struct record *rec;	
	
	error_msg = NULL;
	
	if( (stream = fopen(filename, "w" )) == NULL )
	{
		error_msg = "open file error";
		return false;
	}

	while (sec != NULL)
	{
		if(strlen(sec->comments) != 0)
		{
			fprintf(stream,"%s\n",sec->comments);		
		}
		fprintf(stream,"[%s]\n",sec->name);
		// print section content
		rec = sec->data_first;
		while(rec != NULL)
		{
			if(strlen(rec->comments) != 0)
			{
				fprintf(stream,"%s\n",rec->comments);				
			}
			fprintf(stream,"%s=%s\n",rec->key,rec->value);
			
			rec = rec->next;
		}		
		sec = sec->next;		
	}	
	fclose(stream);
	return true;
}
//////////////////////////////////////////////////////////////////////////
// ini_save to default file
// [10/18/2005]
bool ini_save()
{
	return _save(gfilename);
}

//////////////////////////////////////////////////////////////////////////
// ini_save as (for users)
// [10/19/2005]
bool ini_save_as(const char *filename)
{
	return _save(filename);
}

//////////////////////////////////////////////////////////////////////////
// get value
// [10/19/2005]
char *ini_get_value(const char *sec,const char *key)
{
    int len;
	struct record *result =	ini_get_record(ini_get_section(sec),key);	
	if(result != NULL) {
	    len = strlen(result->value);
	    while (len--) {
	        if (result->value[len] != '\r' && result->value[len] != '\n')
	            break;
	        result->value[len] = '\0';
	    }
		return result->value;
	} else {
		return "";
	}
}
//////////////////////////////////////////////////////////////////////////  
// [10/19/2005]
char *_get_value(const char *sec,const char *key,  char *comment)
{
	struct record *result =	ini_get_record(ini_get_section(sec),key);	
	if(result != NULL)
	{
		strcpy(comment,result->comments);
		return result->value;
	}	
	else
	{
		strcpy(comment,"");
		return "";
	}
}

// added by jqwu,2008-12-10
//////////////////////////////////////////////////////////////////////////
// get int value
// [12/10/2008]
int ini_get_int_value(const char *sec,const char *key,int i_default)
{
	char *buffer;
	buffer = ini_get_value(sec,key);
	if(strcmp(buffer,"") == 0) return i_default;
	return atoi(buffer);
}
//////////////////////////////////////////////////////////////////////////
// set int value if exitst will be replace
// [12/10/2008]
bool ini_set_int_value  (const char *sec,const char *key,const int i_value)
{
	char buffer[10];
	memset( buffer, 0, sizeof(buffer) );
	sprintf( buffer, "%d", i_value );
	return  ini_set_value(sec,key,buffer);
}
// end of added,2008-12-10

//////////////////////////////////////////////////////////////////////////
// set value if exitst will be replace
// [10/20/2005]
bool ini_set_value(const char *sec,const char *key,  const char *value)
{
	return _set_value(sec,key,value,"",REPLACE);	
}
//////////////////////////////////////////////////////////////////////////
// [10/20/2005]
bool _set_value(const char *sec,const char *key,  const char *value,const char *comment,REPLACE_FLAG flag)
{
	w_flag = flag;
	error_msg = NULL;
	_append(sec,key,value,comment);
	if(error_msg == NULL)
		return false;
	else
		return true;
}

//////////////////////////////////////////////////////////////////////////
// get last error
// [10/20/2005]
char *ini_get_last_error()
{
	return error_msg;
}

/************************************************************************/
/* helper function section                                              */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// init list of ini file
//  [10/18/2005]
void init_content()
{
	ini_content = (content *)malloc(sizeof(content));
	
	if(ini_content == NULL)
	{
		error_msg = "cannot malloc memory !";
		return;
	}
	
	ini_content->ini_section_size = 0;	
	ini_content->first = NULL;
	ini_content->last = NULL;
}

//////////////////////////////////////////////////////////////////////////
// add section
//  [10/18/2005]
void ini_add_section(const char *sec,const char *comment)
{
	struct section *temp;
	temp = ini_get_section(sec);
	
	error_msg = NULL;

	if(temp == NULL)
	{
		temp = (struct section *)malloc(sizeof(struct section));
			
		if(temp == NULL)
		{
			error_msg = "cannot malloc memory !";
			return;
		}
		
		// for section name
		strcpy(temp->name,sec);

		if((comment[0] != '#' || comment[0] != ';') && (strlen(comment) > 0))
			sprintf(temp->comments,"%s",comment);
		else
			strcpy(temp->comments,comment);

		// for data link
		temp->data_first = NULL;
		temp->data_last = NULL;
		temp->next = NULL;
		temp->data_size = 0;

		// increment section size
		ini_content->ini_section_size++;

		// for content link
		if (ini_content->first == NULL)
		{
			ini_content->first = temp;
			ini_content->last  = temp;
		}
		else
		{
			ini_content->last->next = temp;
			ini_content->last = temp;
		}	
	}
	else if(w_flag == REPLACE)
	{
		
		
		strcpy(temp->name,sec);
		if((comment[0] != '#' || comment[0] != ';') && (strlen(comment) > 0))
			sprintf(temp->comments,"%s",comment);
		else
			strcpy(temp->comments,comment);
	}
}

//////////////////////////////////////////////////////////////////////////
// append list
//  [10/18/2005]
void _append(const char *sec,const char *key,const char *value,const char *comment)
{
	struct section *tmp_sec;
	struct record *temp;	
	
	// find section
	tmp_sec = ini_get_section(sec);

	if(tmp_sec != NULL)
	{
		temp = ini_get_record(tmp_sec,key);
		if(temp == NULL)
		{
			temp = (struct record *)malloc(sizeof(struct record));
			if(temp == NULL)
			{
				error_msg = "cannot malloc memory !";
				return;
			}
			temp->next = NULL;	
			
			if((comment[0] != '#' || comment[0] != ';') && (strlen(comment) > 0))
				sprintf(temp->comments,"%s",comment);
			else
				strcpy(temp->comments,comment);
			strcpy(temp->key,key);
			strcpy(temp->value,value);			
			tmp_sec->data_size++;

			if (tmp_sec->data_first == NULL)
			{
				tmp_sec->data_first = temp;
				tmp_sec->data_last  = temp;
			}
			else
			{
				tmp_sec->data_last->next = temp;
				tmp_sec->data_last = temp;
			}			
		}
		else if(w_flag == REPLACE)
		{
			if((comment[0] != '#' || comment[0] != ';') && (strlen(comment) > 0))
				sprintf(temp->comments,"%s",comment);
			else
				strcpy(temp->comments,comment);
			
			strcpy(temp->key,key);
			strcpy(temp->value,value);
		}
		
	}
	else
	{
		ini_add_section(sec,"");
		_append(sec,key,value,comment);
	}
}


//////////////////////////////////////////////////////////////////////////
// search and get section
//  [10/18/2005]
struct section *ini_get_section(const char *sec)
{
	bool found = false;
	struct section *esection = ini_content->first;
	while (esection != NULL)
	{	
		if(strcmp(esection->name,sec) == 0)
		{
			found = true;
			break;
		}		
		esection = esection->next;
	}

	if(found == true)
		return esection;
	else
		return NULL;
};

//////////////////////////////////////////////////////////////////////////
// search and get record
struct record *ini_get_record(struct section *sec,const char *key)
{
	bool found = false;
	struct record *tmp;
	
	if(sec==NULL) return NULL;
	tmp = sec->data_first;

	while(tmp != NULL)
	{
		if(strcmp(key,tmp->key) == 0)
		{
			found = true;
			break;
		}
		tmp = tmp->next;
	}

	if(found == true)
	{
		return tmp;
	}
	else
	{
		return NULL;
	}
};

//////////////////////////////////////////////////////////////////////////
// remove list //return num of remove 0 nothing to remove 1 is success
//  [10/18/2005]
int _remove(const char *sec,const char *key)
{	
	struct section *temp_sec = ini_get_section(sec);
	struct record *tmp,*tmp2;
	int	remove = 0;
	
	if(temp_sec == NULL)
		return 0;

	tmp = temp_sec->data_first;
	
	if(tmp == NULL)
		return 0;
	
	if(strcmp(key,tmp->key) == 0)
	{
		temp_sec->data_first = tmp->next;
		temp_sec->data_size--;
		free(tmp);
		return 1;
	}
	
	while(tmp != NULL)
	{
		if(tmp->next != NULL)
		{
			if(strcmp(key,tmp->next->key) == 0)
			{	
				tmp2 = tmp->next;				
				tmp->next = tmp->next->next;
				temp_sec->data_size--;
				free(tmp2);				
				remove = 1;
				break;
			}
		}		
		tmp = tmp->next;
	}		
	return remove;
}

//////////////////////////////////////////////////////////////////////////
// remove all record
// [10/18/2005]
int int_remove_all(const char *sec)
{
	struct section *temp_sec = ini_get_section(sec);
	struct record *tmp;
	int remove = 0;
	
	if(temp_sec == NULL)
		return 0;

	tmp = temp_sec->data_first;
	while(tmp != NULL)
	{
		temp_sec->data_first = tmp->next;
		temp_sec->data_size--;
		free(tmp);
		remove++;
		tmp = temp_sec->data_first;
	}
	return remove;
}
//////////////////////////////////////////////////////////////////////////
// remove selection record
int ini_remove_sel(const char *sec,char *key)
{
	return _remove(sec,key);
}

//////////////////////////////////////////////////////////////////////////
// remove all record
// [10/18/2005]
int  _remove_all(struct section *sec)
{
	struct record *tmp;
	int remove = 0;

	if(sec == NULL)
		return 0;

	tmp = sec->data_first;
	while(tmp != NULL)
	{
		sec->data_first = tmp->next;
		sec->data_size--;
		free(tmp);
		remove++;
		tmp = sec->data_first;
	}
	return remove;
}

//////////////////////////////////////////////////////////////////////////
// remove section
// [10/18/2005]
int  ini_remove_section(char *sec)
{
	struct section *esection = ini_content->first,*temp;
	int remove = 0;

	if(esection == NULL)
		return 0;
	
	if(strcmp(sec,esection->name) == 0)
	{
		_remove_all(esection);
		ini_content->first = esection->next;
		ini_content->ini_section_size--;
		free(esection);
		return 1;
	}
	
	while (esection != NULL)
	{	
		if(strcmp(esection->next->name,sec) == 0)
		{
			_remove_all(esection->next);
			temp = esection->next;				
			esection->next = esection->next->next;
			ini_content->ini_section_size--;
			free(temp);				
			break;
		}		
		esection = esection->next;
	}		
	return remove;
}

//////////////////////////////////////////////////////////////////////////
// ini_clear all content
//  [10/18/2005]
void ini_clear()
{
	struct section *tmp;
	if(ini_content == NULL)
		return;

	tmp = ini_content->first;
	while(tmp != NULL)
	{
		ini_content->first = tmp->next;
		ini_content->ini_section_size--;
		int_remove_all(tmp->name);
		free(tmp);
		tmp = ini_content->first;
	}
	free(ini_content);
}

//////////////////////////////////////////////////////////////////////////
// print all content
//  [10/18/2005]
void ini_print_content()
{
	struct section *sec = ini_content->first;
	while (sec != NULL)
	{
		if(strlen(sec->comments) != 0)
		{
//			printf("%s\n",sec->comments);		
		}
//		printf("[%s]\n",sec->name);
		_print_allrecord(sec->data_first);			
		sec = sec->next;
	}
//	printf("\n");
}

//////////////////////////////////////////////////////////////////////////
// print all record in section
//  [10/18/2005]
void _print_allrecord(struct record *sec)
{
	struct record *temp = sec;
	while(temp != NULL)
	{
		if(strlen(temp->comments) != 0)
		{
//			printf("%s\n",temp->comments);
		
		}
//		printf("%s=%s\n",temp->key,temp->value);
		temp = temp->next;
	}
}

//////////////////////////////////////////////////////////////////////////
// get size of content (number of section
//  [10/18/2005]
int ini_content_size()
{
	return ini_content->ini_section_size;
}

//////////////////////////////////////////////////////////////////////////
// get size of selection section
//  [10/18/2005]
int ini_section_size(char *sec)
{
	struct section *temp = ini_get_section(sec);
	if(temp == NULL)
		return 0;
	return temp->data_size;
}

//////////////////////////////////////////////////////////////////////////
// trime ' ' \n\t\r
//  [10/19/2005]
void ini_trim(char *buffer)
{
	if(buffer[strlen(buffer)-1] == '\n')
		buffer[strlen(buffer)-1] = '\0';
	//char temp[1024];
	//if(buffer[0] )
}
