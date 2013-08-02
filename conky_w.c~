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
1
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
	char date[11];
	char temp_max[4];
	char temp_min[4];
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

void print_element_names(xmlNode * a_node, char *find_s, int n_day)
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


xmlDocPtr copy_xml(char *buf,int l,int day)
{
	int c;
	xmlDocPtr prr;
	xmlNode *root_element;
	prr=xmlReadMemory(buf, l, "123.xml", NULL, 0);
	root_element = xmlDocGetRootElement(prr);
	//print_element_names(root_element,"temp_C");
	//printf("[[[[[[%s]]]]]]]",temp);
	strcpy(ww[0].date,"");
	for(c=0;c<day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"date",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c+1].date,temp);
		temp=NULL;
		}
	nn_day=0;
		print_element_names(root_element,"temp_C",0);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[0].temp_max,temp);
		strcpy(ww[0].temp_min,temp);
		temp=NULL;
	for(c=0;c<day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"tempMaxC",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c+1].temp_max,temp);
		temp=NULL;
		}
	for(c=0;c<day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"tempMinC",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c+1].temp_min,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"weatherCode",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].code,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"weatherIconUrl",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].url,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"weatherDesc",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].desc,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"windspeedKmph",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].wind_speed,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"winddir16Point",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].wind_dir,temp);
		temp=NULL;
		}
	for(c=0;c<=day;c++) 
		{  
		nn_day=0;
		print_element_names(root_element,"precipMM",c);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[c].precip,temp);
		temp=NULL;
		}
	
		nn_day=0;
		print_element_names(root_element,"humidity",0);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[0].humidity,temp);
		temp=NULL;
	
		nn_day=0;
		print_element_names(root_element,"visibility",0);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[0].visibility,temp);
		temp=NULL;

		nn_day=0;
		print_element_names(root_element,"pressure",0);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[0].pressure,temp);
		temp=NULL;

		nn_day=0;
		print_element_names(root_element,"cloudcover",0);
		printf("[[[[[[%s]]]]]]]",temp);
		strcpy(ww[0].cloudcover,temp);
		temp=NULL;
		
	return prr;
}

int check_arg(int ac, char *av[], char *s)
{
	int u_day=1;
	int c;
	char *v;
	//printf("%d",strlen(s));
	for(c=1;c<ac;c++)
		if(!strncmp(av[c],s,strlen(s)))
			{
		//		printf("%s",&av[c][strlen(s)]);
				v=&av[c][strlen(s)+1];
				//if(!(u_day=strtol(v,NULL, 10))) printf("Неправильный агрумент %s",s);
				u_day=strtol(v,NULL, 10);
				printf("%d",u_day);
			}
	return  u_day;
}

void err_rep(int n)
{
	char err_r[][256]=
	{
		"Значение day не может быть больше nday\n",
		"Значение nday не может быть больше 7\n",
		"Ошибка инициализации curl\n",
		"Ошибка получения погоды.\n",
		"Ошибка при распределении памяти\n"
	};
	printf("%s",err_r[n]);
	free(ww);
	exit(0);
}

int main(int argc, char *argv[])
{
	CURL *url;
	CURLcode cerr;
	int url_day=1;
	int day=0;
	int c;
	char cr_url[200];
	w_index = 0;
	if(argc>1)
		{
			if((url_day=check_arg(argc, argv,"--nday"))>7) err_rep(1);
			day=check_arg(argc, argv,"--day");
		}
	if((ww = malloc((url_day+1)*sizeof(struct weather)))==NULL) err_rep(4);
    if(!(url=curl_easy_init())) err_rep(2);
	sprintf(cr_url,"http://api.worldweatheronline.com/free/v1/weather.ashx?q=Tambov&format=xml&num_of_days=\%d&key=brskzv3ts8ma9jqwgrzz35at",url_day);
	curl_easy_setopt( url, CURLOPT_URL, cr_url );
	curl_easy_setopt( url, CURLOPT_WRITEFUNCTION, parse_weather );
	cerr = curl_easy_perform( url );
	if(cerr) err_rep(3);
	//printf("cerr=%d {%s}",cerr,w_buf);
	curl_easy_cleanup( url );
	if(day>url_day) err_rep(0);
	pr=copy_xml(&w_buf[0],sizeof(w_buf),day);
	for(c=0;c<=day;c++) printf("\n%s %s %s %s %s %s %s %s %s %s %s %s %s",ww[c].date,ww[c].temp_max,ww[c].temp_min,ww[c].code,ww[c].url,ww[c].desc,ww[c].wind_speed,ww[c].wind_dir,ww[c].precip,ww[c].humidity,ww[c].visibility,ww[c].pressure,ww[c].cloudcover);
	printf("\n");
	free(ww);
	return 0;
}

