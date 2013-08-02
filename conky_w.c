/*
 * main.c
 * Copyright (C) 2013 smike <smike@nm.ru>
 * 
 * conky-weather is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * conky-weather is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#define MAX_BUF	6000

char w_buf[MAX_BUF+1];
int w_index;
int nn_day;
xmlDocPtr pr;
char *temp;
struct weather
{
	char date[33];
	char temp_max[10];
	char temp_min[10];
	char code[10];
	char url[255];
	char desc[100];
	char wind_speed[10];
	char wind_dir[10];
	char precip[10];
	char humidity[10];
	char visibility[10];
	char pressure[10];
	char cloudcover[10];
};

struct weather * ww;

void 
print_element_names(xmlNode * a_node, char *find_s, int n_day)
{
    xmlNode *cur_node = NULL;
	xmlNode *temp_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
        if (cur_node->type == XML_ELEMENT_NODE) 
			if(!strcmp((char*)cur_node->name,find_s)) 
				{
					temp_node=cur_node->children;
					if(nn_day==n_day) temp=(char*)temp_node->content;
					nn_day++;	
					return;
				}
        if (!temp) print_element_names(cur_node->children,find_s,n_day);
   	}
	return;
}

size_t parse_weather( void *buffer, size_t size, size_t nmemb, void *userp )
{
 int segsize = size * nmemb;
 if ( w_index + segsize > MAX_BUF )
	{
	*(int *)userp = 1;
	return 0;
	}
  memcpy( (void *)&w_buf[w_index], buffer, (size_t)segsize );
 w_index += segsize;
 w_buf[w_index] = 0;
 return segsize;
}


xmlDocPtr copy_xml(char *buf,int l)
{
	xmlDocPtr prr;
	xmlNode *root_element;
	prr=xmlReadMemory(buf, l, "123.xml", NULL, 0);
	root_element = xmlDocGetRootElement(prr);
	//print_element_names(root_element,"temp_C");
	//printf("[[[[[[%s]]]]]]]",temp);
	nn_day=0;
	print_element_names(root_element,"windspeedKmph",0);
	printf("[[[[[[%s]]]]]]]",temp);
	temp=NULL;
	nn_day=0;
	print_element_names(root_element,"windspeedMiles",0);
	printf("[[[[[[%s]]]]]]]",temp);
	temp=NULL;
	nn_day=0;
	print_element_names(root_element,"precipMM",0);
	printf("[[[[[[%s]]]]]]]",temp);
/*	printf("node type: Element, name: %s,%s\n", root_element->name,root_element->content);
	cur_node =root_element->children;
	printf("node type: Element, name: %s,%s\n", cur_node->name,cur_node->content);
	cur_node =cur_node->next;
	printf("node type: Element, name: %s,%s\n", cur_node->name,cur_node->content);
	cur_node =cur_node->children;
	printf("node type: Element, name: %s,%s\n", cur_node->name,cur_node->content);
	cur_node =cur_node->next;
	printf("node type: Element, name: %s,%s\n", cur_node->name,cur_node->content);
	cur_node =cur_node->children;
	printf("node type: Element, name: %s,%s\n", cur_node->name,cur_node->content);*/
	return prr;
}

int main(int argc, char *argv[])
{
	CURL *url;
	CURLcode cerr;
	w_index = 0;
    if(!(url=curl_easy_init()))
	{
		printf ("Ошибка инициализации curl");
		return (1);
	}
	curl_easy_setopt( url, CURLOPT_URL, "http://api.worldweatheronline.com/free/v1/weather.ashx?q=Tambov&format=xml&num_of_days=2&key=brskzv3ts8ma9jqwgrzz35at" );
	curl_easy_setopt( url, CURLOPT_WRITEFUNCTION, parse_weather );
	cerr = curl_easy_perform( url );
	if(cerr)
		{
			printf("Ошибка получения погоды.");
			return (0);
		}
	printf("cerr=%d {%s}",cerr,w_buf);
	curl_easy_cleanup( url );
	pr=copy_xml(&w_buf[0],sizeof(w_buf));
	return (0);
}

