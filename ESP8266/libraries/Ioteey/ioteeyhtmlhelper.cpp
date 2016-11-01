#include "ioteeyhtmlhelper.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

String ioteeyhtmlhelper::startFORM(const char* name, const char* action) {
	//char* tmp = (char*) malloc((strlen(name)+sizeof(action)+3000)*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<form method='POST' action='");
	strcat(tmp,action);
	strcat(tmp,"' style='font-size:30px' name='");
	strcat(tmp,name);
	strcat(tmp,"'>\n");
	return String(tmp);
};

String ioteeyhtmlhelper::endFORM() {
	return String("</form>\n");
};

String ioteeyhtmlhelper::writeLink(const char* label, const char* url) {
	//char* tmp = (char*) malloc((3000+strlen(label)+strlen(url))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' colspan='3' nowrap>");
	strcat(tmp, "<a style='font-size:40px' href='");
	strcat(tmp,url);
	strcat(tmp,"'>");
	strcat(tmp,label);
	strcat(tmp,"</a>");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);
};

String ioteeyhtmlhelper::writeButtonRECORD(const char* name, const char* value, const char* size) {
	//char* tmp = (char*) malloc((3000+strlen(value)+strlen(name)+strlen(size))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' colspan='3' nowrap>");
	strcat(tmp, "<input style='font-size:40px' type='");
	strcat(tmp,"submit");
	strcat(tmp,"' size='");
	strcat(tmp,size);
	strcat(tmp,"' name='");
	strcat(tmp,name);
	strcat(tmp,"' value='");
	strcat(tmp,value);
	strcat(tmp,"'>");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);
};

String ioteeyhtmlhelper::writeRECORD(const char* label, const char* value) {
	//char* tmp = (char*) malloc((3000+strlen(label)+strlen(value))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp, "<th bgcolor='#FFFFFF' align='left' width='40%' height='32' nowrap>");
	strcat(tmp, label);
	strcat(tmp,":</th>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='right' width='18' height='32' nowrap></td>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' nowrap>");
	strcat(tmp, value);
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);
};



String ioteeyhtmlhelper::renderParameter(String code, String sensororactuator, int sensororactuatorid, int parameternumber, String parameterid, String parametername, String parametertype, String parametervalue) {
	//For now all parameters are input boxes. Change that to render things differently based on the parameter type
	//char* tmp = (char*) malloc((3000+strlen(parametervalue.c_str())+strlen(parametername.c_str())+strlen(code.c_str())+strlen(parametertype.c_str()))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp, "<th bgcolor='#FFFFFF' align='left' width='40%' height='32' nowrap>");
	strcat(tmp, parametername.c_str());
	strcat(tmp,":</th>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='right' width='18' height='32' nowrap></td>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' nowrap>");
	strcat(tmp, "<input style='font-size:30px' type='");
	strcat(tmp,"text");
	strcat(tmp,"' size='");
	strcat(tmp,"100%");
	strcat(tmp,"' name='");
	strcat(tmp,code.c_str());
	strcat(tmp,"' value='");
	strcat(tmp,parametervalue.c_str());
	strcat(tmp,"'>");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);	
};


String ioteeyhtmlhelper::writeTextInput(const char* label, const char* name, const char* value, const char* size, const char* type) {
	//char* tmp = (char*) malloc((3000+strlen(label)+strlen(value)+strlen(name)+strlen(size)+strlen(type))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp, "<th bgcolor='#FFFFFF' align='left' width='40%' height='32' nowrap>");
	strcat(tmp, label);
	strcat(tmp,":</th>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='right' width='18' height='32' nowrap></td>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' nowrap>");
	strcat(tmp, "<input style='font-size:30px' type='");
	strcat(tmp,type);
	strcat(tmp,"' size='");
	strcat(tmp,size);
	strcat(tmp,"' name='");
	strcat(tmp,name);
	strcat(tmp,"' value='");
	strcat(tmp,value);
	strcat(tmp,"'>");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);
};

String ioteeyhtmlhelper::writeSelectionBox(const char* label, const char* name, int totalitems, const char labels[][10], const char values[][10], const char* size) {
	//char* tmp = (char*) malloc((3000+strlen(label)+strlen(name)+strlen(size))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp, "<th bgcolor='#FFFFFF' align='left' width='40%' height='32' nowrap>");
	strcat(tmp, label);
	strcat(tmp,":</th>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='right' width='18' height='32' nowrap></td>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' width='#' height='32' nowrap>");
	strcat(tmp, "<select");
	strcat(tmp," name='");
	strcat(tmp,name);
	strcat(tmp,"'>");
	for(int i=0;i<totalitems;i++) {
		strcat(tmp,"<option value='");
		strcat(tmp,values[i]);
		strcat(tmp,"'>");
		strcat(tmp,labels[i]);
		strcat(tmp,"</option>");
	};
	strcat(tmp,"</select>");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");		
	
	return String(tmp);
};

String ioteeyhtmlhelper::startTABLE(const char* width, const char* height, const char* border, const char* cellspacing, const char* cellpadding) {
	//char* tmp = (char*) malloc(300+(strlen(width)+strlen(height)+strlen(border)+strlen(cellspacing)+strlen(cellpadding))*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<table style='font-size:30px;padding-left:20px;padding-right:20px' width='");
	strcat(tmp,width); 
	strcat(tmp,"' height='");
	strcat(tmp,height); 
	strcat(tmp,"' border='");
	strcat(tmp,border);
	strcat(tmp,"' cellspacing='");
	strcat(tmp,cellspacing); 
	strcat(tmp,"' cellpadding='");
	strcat(tmp,cellpadding);
	strcat(tmp,"'>\n");

	return String(tmp);
};

String ioteeyhtmlhelper::endTABLE() {
	return String("</table>\n");
};

String ioteeyhtmlhelper::startROW() {
	return String("<tr>\n");
};

String ioteeyhtmlhelper::endROW() {
	return String("</tr>\n");
};

String ioteeyhtmlhelper::writeHeading() {
	//char* tmp = (char*) malloc(500*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<table style='font-size:30px' width='100%' border='0' cellspacing='0' cellpadding='0'>\n");
	strcat(tmp,"<tr>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' align='left' valign='top' width='10%' height='76' nowrap>\n");
	strcat(tmp,"<h2 style='padding-top:25px;padding-left:10px'>IOTEEY CONFIGURATION</h2>\n");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");								
	strcat(tmp,"</table>\n");
	return String(tmp);
}

String ioteeyhtmlhelper::writeRecordHeading(const char* label) {
	//char* tmp = (char*) malloc((strlen(label)+2000)*sizeof(char));
	char tmp[3000];
	strcpy(tmp,"<tr valign='middle'>\n");
	strcat(tmp,"<th bgcolor='#FFFFFF' align='left' width='100%' style='padding-top:10px;padding-bottom:0px;margin-bottom:0px' colspan='3' nowrap><h3 style='padding-bottom:0px;margin-top:20px;margin-bottom:0px'>\n");
	strcat(tmp,label);
	strcat(tmp,"</h3></th>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);
};

String ioteeyhtmlhelper::writeLine() {
	char tmp[3000];
	//char* tmp = (char*) malloc((500)*sizeof(char));
	strcpy(tmp,"<tr>\n");
	strcat(tmp,"<td bgcolor='#FFFFFF' colspan='3'>\n");
	strcat(tmp,"<hr size='1'>\n");
	strcat(tmp,"</td>\n");
	strcat(tmp,"</tr>\n");
	return String(tmp);	
}

String ioteeyhtmlhelper::startCOLUMN(const char* bgcolor, const char* align, const char* valign, const char* width, const char* height, const char* colspan, bool nowrap) {
	int size = (strlen(bgcolor)+strlen(align)+strlen(valign)+strlen(width)+strlen(height)+strlen(colspan)+500)*sizeof(char);
	if(nowrap) {
		size = size + 6;
	};
	char tmp[3000];
	strcpy(tmp,"<td bgcolor='");
	strcat(tmp,bgcolor);
	strcat(tmp,"' align='");
	strcat(tmp,align);
	strcat(tmp,"' valign='");
	strcat(tmp,valign);
	strcat(tmp,"' width='");
	strcat(tmp,width);
	strcat(tmp,"' height='");
	strcat(tmp,height);
	strcat(tmp,"' ");
	if(nowrap) {
		strcat(tmp,"nowrap");
	};
	strcat(tmp,">\n");
	return String(tmp);
};

String ioteeyhtmlhelper::endCOLUMN() {
	return String("</td>\n");
};

String ioteeyhtmlhelper::writeH2(const char* text) {

};

String ioteeyhtmlhelper::writeH3(const char* text) {
};

String ioteeyhtmlhelper::startFONT(const char* size) {

}; 

String ioteeyhtmlhelper::endFONT() {
	return String("</font>\n");
};

String ioteeyhtmlhelper::writeHR(const char* size) {

};

String ioteeyhtmlhelper::writeTEXT(const char* text) {

};

String ioteeyhtmlhelper::writeSpaces(int spaces) {
	for(int i=0;i<spaces;i++) {
		
	}
};
