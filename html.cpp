#include <iostream>
#include <string>
#include <fstream> 
#include "html.h"
using namespace std;
int main()
{
	FILE* pfile = fopen("About - Opera Software.html","rb");
	int len= 0;
	if(pfile)
	{
		fseek(pfile,0,SEEK_END);
		len = ftell(pfile);
		fseek(pfile,0,SEEK_SET);		
	}
	else 
	{
		cout<<"open file error, please put the file together with exe and retry."<<endl;
		return 0;
	}
	char* htmlbuf = new char[len+1];
	if(htmlbuf==NULL)
	{
		cout<<"new buffer error"<<endl;
		return 0;
	}
	
	htmlbuf[len] = '\0';
	fread(htmlbuf,len,1,pfile);
	fclose(pfile);
	
	Html test;
	test.parseHtml(htmlbuf,htmlbuf+len);
	delete []htmlbuf;
	ofstream os("level.txt",ios_base::out|ios_base::binary);
	os<<test;
	//cout<<endl<<"done!"<<endl;
	return 0;
}
