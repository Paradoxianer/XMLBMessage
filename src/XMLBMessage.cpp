/*
 * Copyright 2016 All Rights Reserved.
 * Distributed under the terms of the MIT license.
 *
 * Authors:
 *		Matthias Lindner
 */


#include <Application.h>
#include <String.h>
#include <File.h>
#include <Path.h>
#include <Entry.h>
#include <Mime.h>
#include <NodeInfo.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "XmlBMessageApp"
#include "MessageXmlReader.h"
#include "MessageXmlWriter.h"



XmlBMessageApp::XmlBMessageApp(void)
	: BApplication("application/x-vnd.xmlbmessage"),
	fPrintUsage(false),
	fToMessage(false),
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
	/*if (!strcmp(argv[2], "-f")|| !strcmp(argv[1], "-f"))
		fArguments++;
		fOverWrite = true;
		return;*/

	if (!strcmp(argv[1], "--toMessage")) {
		fArguments++;
		fToMessage = true;
	}

	if (argc < (fArguments+2)) {
		fPrintUsage = true;
		return;
	}
	BString inputfile("");
	BString outputfile("");
	
	inputfile << argv[fArguments+1];
	
	if ((fArguments+1) < argc)
		if (fToMessage)
			outputfile << argv[fArguments+1] << ".message";
		else
			outputfile << argv[fArguments+1] << ".xml";
	else
		outputfile << argv[fArguments+2];
	
	status_t status;
	if (fToMessage)
		status = ToMessage(inputfile.String(), outputfile.String());
	else
		status = ToXML(inputfile.String(), outputfile.String());
	if (status < B_OK)
		exit(-1);
}


void
XmlBMessageApp::ReadyToRun(void)
{
	if (fPrintUsage == false)
		PrintUsage();

	PostMessage(B_QUIT_REQUESTED);
}


void
XmlBMessageApp::PrintUsage(void)
{
	printf("usage: xmlbmessage [--toMessage] [INPUT FILE] [OUTPUT FILE]  \n");
}



status_t
XmlBMessageApp::ToXML(const char *inPath, const char *outPath)
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

//	#pragma mark -


int
main()
{
	new XmlBMessageApp();
	be_app->Run();

	return B_OK;
}
