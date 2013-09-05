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
#define conky_w_conf "/home/smike/conky_w/conky_w.rc"
#define MAX_BUF	6000

#define _XOPEN_SOURCE
#include <stdio.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <locale.h>

struct cmd
{
	char name[255];
	char value[4];
	char *buf;
};

struct cmd_p
{
	char datatype[3];
	int day;
	char date_format[256];
	char *buf;
	int hide_u;
};

struct weather_cur
{
	char time[11];
	char temp[4];
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
};


struct weather * ww;
struct weather_cur w;
int w_index=0;
char w_buf[MAX_BUF+1];
xmlDocPtr pr;
char *temp;
int nn_day;

void err_rep(int n)
{
	char *err_r[]=
	{
		"Значение day не может быть больше nday\n",
		"Значение nday не может быть больше 7\n",
		"Ошибка инициализации curl\n",
		"Ошибка получения погоды.\n",
		"Ошибка при распределении памяти\n",
		"Не выбран ни один параметр\n",
		"Не удаётся отрыть файл.\n",
		"Ошибка получение атрибутов файла конфигурации\n",
		"Ошибка при распределении памяти\n"
	};
	printf("%s",err_r[n]);
	free(ww);
	exit(1);
}

void print_WI(int day)
{
	FILE *f;
	CURL *url;
	CURLcode cerr;
	if(day) f=fopen(ww[day].code,"w+b");
	else f=fopen(w.code,"w+b");
	if(!(url=curl_easy_init())) err_rep(2);
	if(day) curl_easy_setopt( url, CURLOPT_URL, ww[day].url );
	else curl_easy_setopt( url, CURLOPT_URL, w.url );
	curl_easy_setopt( url, CURLOPT_WRITEDATA, f);
	curl_easy_setopt( url, CURLOPT_WRITEFUNCTION, NULL );
	cerr = curl_easy_perform( url );
//3	printf("[%s](%d)",ww[day].url,day);
	if(cerr) err_rep(3);
	if(day) printf("~/conky_w/%s",ww[day].code);
	else printf("~/conky_w/%s",w.code);
	curl_easy_cleanup( url );
	fclose(f);
	return;
}

struct cmd read_cmd(char *buf)
{
	struct cmd cc;
	int c=0,fl=0,cv=0;;
	memset(&cc,'\0',sizeof(struct cmd));
	//cc.name[0]='\0';
	//cc.value[0]='\0';
	for(;*buf!=']';buf++)
	{
		//cc.name[c+1]='\0';
		//cc.value[cv+1]='\0';
		if(*buf==' ') {cc.buf=buf; return cc;}
		if(!fl) {if(*buf!='=') cc.name[c++]=*buf;
			else fl=1;
		}
		else if(*buf!=' ') cc.value[cv++]=*buf;
		
	}
	cc.buf=buf;
	return cc;
}
struct cmd_p parse_cmd(char *buf)
{
	struct cmd cc;
	struct cmd_p cp;
	//cp.datatype[0]='\0';
	//cp.day=0;
	memset(&cp,0,sizeof(struct cmd_p));
	for(;*buf!=']';buf++)
	{
		if((*buf=='-')&&(*(buf+1)=='-'))
		{
			cc=read_cmd(buf);
			cc.buf=buf;
			if(!strcmp(cc.name,"--datatype")) strcpy(cp.datatype,cc.value);
			else if(!strcmp(cc.name,"--startday")) cp.day=atoi(cc.value);
				else if(!strcmp(cc.name,"--dateformat")) strcpy(cp.date_format,cc.value);
					else if(!strcmp(cc.name,"--hideunits")) cp.hide_u=1;
					
		}
	}
	cp.buf=buf;
	return cp;
}

void search_dir(char *buf)
{
	struct cmd_p c;
	struct tm tm;
	char date_b[255];
	for(;*buf;buf++)
	{
		if(*buf=='[') 
		{
			c=parse_cmd(buf);
			buf=c.buf;
			if (!strcmp(c.datatype,"WI")) print_WI(c.day);
				else if (!strcmp(c.datatype,"CT")) if(c.hide_u) printf("%s",w.temp);
													else printf("%s °C",w.temp);
					else if (!strcmp(c.datatype,"WS")) printf("%s км/ч",w.wind_speed);
						else if (!strcmp(c.datatype,"HT")) if(c.hide_u) printf("%s",ww[c.day].temp_max);
															else printf("%s °C",ww[c.day].temp_max);
							else if (!strcmp(c.datatype,"LT")) if(c.hide_u) printf("%s",ww[c.day].temp_min);
																else printf("%s °C",ww[c.day].temp_min);
								else if (!strcmp(c.datatype,"DW"))
									{
										memset(&tm, 0, sizeof(struct tm));						
       								strptime(ww[c.day].date,"%Y-%m-%d", &tm);
									//	printf("(%s)",c.date_format);
									strftime(date_b, sizeof(date_b), c.date_format, &tm);
										 printf("%s",date_b);
									}
		}
		else putchar(*buf);
	}
	return;
}

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

void xml_to_cur(xmlNode *root_element, char *s, int n, char *cw)
{
	nn_day=0;
		print_element_names(root_element,s,n);
		strcpy(cw,temp);
		temp=NULL;
	return;
}

xmlDocPtr copy_xml(char *buf,int l,int day)
{
	int c;
	xmlDocPtr prr;
	xmlNode *root_element;
	prr=xmlReadMemory(buf, l, "123.xml", NULL, 0);
	root_element = xmlDocGetRootElement(prr);
	xml_to_cur(root_element,"observation_time",0,w.time);
	xml_to_cur(root_element,"temp_C",0,w.temp);
	xml_to_cur(root_element,"weatherCode",0,w.code);
	sprintf(w.url,"http://cdn.worldweatheronline.net/images/weather/large/%s_day_lg.png",w.code);
	xml_to_cur(root_element,"weatherDesc",0,w.desc);
	xml_to_cur(root_element,"windspeedKmph",0,w.wind_speed);
	xml_to_cur(root_element,"winddir16Point",0,w.wind_dir);
	xml_to_cur(root_element,"precipMM",0,w.precip);
	xml_to_cur(root_element,"humidity",0,w.humidity);
	xml_to_cur(root_element,"visibility",0,w.visibility);
	xml_to_cur(root_element,"pressure",0,w.pressure);
	xml_to_cur(root_element,"cloudcover",0,w.cloudcover);
	for(c=0;c<day;c++) 
		{  
			xml_to_cur(root_element,"date",c,ww[c].date);
			xml_to_cur(root_element,"tempMaxC",c,ww[c].temp_max);
			xml_to_cur(root_element,"tempMinC",c,ww[c].temp_min);
			xml_to_cur(root_element,"weatherCode",c+1,ww[c].code);
			sprintf(ww[c].url,"http://cdn.worldweatheronline.net/images/weather/large/%s_day_lg.png",ww[c].code);
			xml_to_cur(root_element,"weatherDesc",c+1,ww[c].desc);
			xml_to_cur(root_element,"windspeedKmph",c+1,ww[c].wind_speed);
			xml_to_cur(root_element,"winddir16Point",c+1,ww[c].wind_dir);
			xml_to_cur(root_element,"precipMM",c+1,ww[c].precip);
	
		}
	return prr;
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

int main(int argc, char *argv[])
{
	FILE *f;
	char *buf_conf;
	int url_day=4;
	CURL *url;
	CURLcode cerr;
	char cr_url[200];
	int day=4;
	struct stat sb;
	setlocale(LC_ALL, "");
	if(stat(conky_w_conf,&sb)==-1) err_rep(7);
	if(!(buf_conf=calloc(sb.st_size,sizeof(char)))) err_rep(8);
	if((f=fopen(conky_w_conf, "rb"))==NULL) err_rep(6);
	fread(buf_conf, sizeof(char),sb.st_size, f);
	fclose(f);
	if((ww = malloc(url_day*sizeof(struct weather)))==NULL) err_rep(4);
    if(!(url=curl_easy_init())) err_rep(2);
	sprintf(cr_url,"http://api.worldweatheronline.com/free/v1/weather.ashx?q=Tambov&format=xml&num_of_days=\%d&key=brskzv3ts8ma9jqwgrzz35at",url_day);
	curl_easy_setopt( url, CURLOPT_URL, cr_url );
	curl_easy_setopt( url, CURLOPT_WRITEFUNCTION, parse_weather );
	cerr = curl_easy_perform( url );
	if(cerr) err_rep(3);
//	curl_easy_getinfo(url,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&uu);
//	printf("%d",uu);
	//printf("cerr=%d {%s}",cerr,w_buf);
	curl_easy_cleanup( url );
//	if(day>url_day) err_rep(0);
	pr=copy_xml(&w_buf[0],sizeof(w_buf),day);
	search_dir(buf_conf);
	free(ww);
	free(buf_conf);
	return 0;
}

