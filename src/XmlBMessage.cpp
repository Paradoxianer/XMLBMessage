/*
 * Copyright 2016 All Rights Reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Matthias Lindner
 */


#include <String.h>
#include <File.h>
#include <Path.h>
#include <Entry.h>
#include <Mime.h>
#include <NodeInfo.h>

#include <getopt.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "MessageXmlWriter.h"

#include "MessageXmlReader.h"

#include "XmlBMessage.h"



XmlBMessageApp::XmlBMessageApp(void)
	: BApplication("application/x-vnd.xmlbmessage"),
	fPrintUsage(false),
	fToMessage(false),
	fShow(false),
	fOverWrite(false),
	fArguments(0)
{

}


XmlBMessageApp::~XmlBMessageApp()
{
}


void
XmlBMessageApp::ArgvReceived(int32 argc, char **argv)
{
	status_t status = B_OK;

	static struct option const kLongOptions[] = {
		{"show", no_argument, 0, 's'},
		{"force", no_argument, 0, 'f'},
		{"help",no_argument, 0,'h'},
		{NULL}
	};

	int c;
	while ((c = getopt_long(argc, argv, "l:s:o:c:ripdh", kLongOptions, NULL))
		!= -1) {
		switch (c) {
			case 's':
				fShow = true;
				fArguments++;
				break;
			case 'f':
				fOverWrite = true;
				fArguments++;
				break;
			default:
				fPrintUsage = true;
				fArguments++;
				break;
		}
	}
	
	BPath inputfile		= BPath();
	BPath outputfile	= BPath();
	
	inputfile.SetTo(argv[fArguments+1]);
	if (DetectFileType(inputfile.Path()) == XML_FILE_TYPE)
		fToMessage=true;
	
	
	if ((fArguments+1) < argc)
		if (fToMessage){
			BString tmpStr=inputfile.Leaf();
			tmpStr << ".message";
			outputfile.SetTo(tmpStr);
		}
		else {
			BString tmpStr=inputfile.Leaf();
			tmpStr << ".xml";
			outputfile.SetTo(tmpStr);
		}
	else
		outputfile.SetTo(argv[fArguments+2]);
	
	if (fToMessage)
		status = ToMessage(inputfile.Path(), outputfile.Path());
	else
		status = ToXml(inputfile.Path(), outputfile.Path());
	if (status < B_OK)
		exit(-1);
}


void
XmlBMessageApp::ReadyToRun(void)
{
	if (fPrintUsage == true)
		PrintUsage();

	PostMessage(B_QUIT_REQUESTED);
}


void
XmlBMessageApp::PrintUsage(void)
{
	printf("usage: xmlbmessage [--toMessage] [INPUT FILE] [OUTPUT FILE]  \n");
}



status_t
XmlBMessageApp::ToXml(const char *inPath, const char *outPath)
{
	BMessage	tmpMessage = BMessage();
	BFile		tmpFile(inPath, B_READ_ONLY);
	status_t	status;
	status = tmpMessage.Unflatten(&tmpFile);
	if (status == B_OK) {
		MessageXmlWriter xmlWrite = MessageXmlWriter(outPath);
		status = xmlWrite.Write(tmpMessage);
	}
	return status;
}


status_t
XmlBMessageApp::ToMessage(const char *inPath, const char *outPath)
{
	MessageXmlReader xmlRead = MessageXmlReader(BString(inPath));
	BMessage	*tmpMessage=xmlRead.Read();
	uint32		openMode = B_WRITE_ONLY |B_CREATE_FILE;
	if (fOverWrite)
		openMode = openMode  | B_ERASE_FILE;
	BFile		*tmpFile = new BFile(outPath, openMode);
	status_t	status;
	ssize_t		numBytes;
	status = tmpMessage->Flatten(tmpFile,&numBytes);
	return status;
	
}

file_Type
XmlBMessageApp::DetectFileType(const char *filePath) 
{
	BFile		tmpFile(filePath, B_READ_ONLY);
	BMessage	tmpMessage	= BMessage();
	//@todo check if the file was initalized correctly
	status_t	status		= tmpMessage.Unflatten(&tmpFile);
	if (status != B_OK)
		return XML_FILE_TYPE;
	// @todo check also if the file is a XML File.
	else
		return MESSAGE_FILE_TYPE;
}
//	#pragma mark -


int
main()
{
	new XmlBMessageApp();
	be_app->Run();

	return B_OK;
}

