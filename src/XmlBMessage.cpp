/*
 * Copyright 2017 All Rights Reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Matthias Lindner
 *		Pete Goodeve
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
			case 'h':
				fPrintUsage = true;
				fArguments++;
				break;
			default:
				fPrintUsage = true;
				fArguments++;
				break;
		}
	}
	if (fPrintUsage != true){
		BPath inputfile		= BPath();
		BPath outputfile	= BPath();
	
		inputfile.SetTo(argv[fArguments+1]);
		if (DetectFileType(inputfile.Path()) == XML_FILE_TYPE)
			fToMessage=true;
	
		//check if there is beside the number of arguments one argv for the app name one for the input and a third one for the outpug
		if (argc < (fArguments+3)  )
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
	}
	if (status != B_OK){
		fPrintUsage=true;
		exit(-1);
	}
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
	printf("usage: xmlbmessage [--force] [--show] [--help] [INPUT FILE] [OUTPUT FILE]  \n\n"
		   "--force\t overwrite an existing outputfile\n"
		   "--show\t shows the content of the loaded and written file\n\n"
		   "--help\t shows this text\n\n"
		   "INPUT FILE should be either a xml file in BMessage format\n"
		   "Or should be a File with flatterned BMessages in it.\n\n"
		   "If you dont specify OUTPUT FILE\n\n"
		   "\tWhen the input file is detected as a BMessage File the OUTPUT FILE is set to\n"
		   "\tINPUT FILE + .xml\n"
		   "\tit is like\n"
		   "\txmlbmessage [INPUT FILE] [INPUT FILE].xml \n\n"
		   "\tWhen the input file is detected as a XML file the OUTPUT FILE is set to\n"
		   "\tINPUT FILE +.message \n"
		   "\tit is like\n"
		   "\txmlbmessage [INPUT FILE] [INPUT FILE].message\n\n");
}



status_t
XmlBMessageApp::ToXml(const char *inPath, const char *outPath)
{
	BFile		tmpFile(inPath, B_READ_ONLY);
	status_t	status;
	off_t		fileSize;
	if (!tmpFile.IsReadable()) {
		printf("cannot open \"%s\" for reading\n", inPath);
		return 2;
	}

	if ((status = tmpFile.GetSize(&fileSize)) != B_OK) {
		printf("cannot determine size of file \"%s\"\n", inPath);
		return 3;
	}
	if (fShow == true)
			printf("reading from: %s: \n",inPath);
		
	for (int i = 1; tmpFile.Position() < fileSize; ++i) {
		BMessage	tmpMessage;
		status	 = tmpMessage.Unflatten(&tmpFile);
		if (fShow == true)
			tmpMessage.PrintToStream();
		if (status != B_OK) {
			printf("failed to unflatten message: %s\n", strerror(status));
			return 4;
		}
		else {
			MessageXmlWriter xmlWrite = MessageXmlWriter(outPath);
			status = xmlWrite.Write(tmpMessage,fShow);
		}
		
	}
	return status;
}


status_t
XmlBMessageApp::ToMessage(const char *inPath, const char *outPath)
{
	MessageXmlReader xmlRead = MessageXmlReader(BString(inPath));
	if (fShow == true)
			printf("reading from: %s: \n",inPath);
	BMessage	*tmpMessage=xmlRead.Read(fShow);
	if (tmpMessage == NULL) {
		printf("Source '%s' is neither a file with BMessages nor valid XML!\n", inPath);
		return B_BAD_DATA;
	}
	uint32		openMode = B_WRITE_ONLY |B_CREATE_FILE;
	if (fOverWrite)
		openMode = openMode  | B_ERASE_FILE;
	BFile		*tmpFile = new BFile(outPath, openMode);
	status_t	status;
	ssize_t		numBytes;
	if (fShow == true){
			printf("writing to: %s: \n",outPath);
			tmpMessage->PrintToStream();
	}
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
	if (status == B_OK )
		return MESSAGE_FILE_TYPE;
	// @todo check also if the file is a XML File.
	else
		return XML_FILE_TYPE;
}
//	#pragma mark -


int
main()
{
	new XmlBMessageApp();
	be_app->Run();

	return B_OK;
}

