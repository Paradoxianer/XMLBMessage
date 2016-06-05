#ifndef XML_MESSAGE_H
#define XML_MESSAGE_H

#include <Application.h>

enum file_Type{
	MESSAGE_FILE_TYPE		= 1,
	XML_FILE_TYPE			= 2,
	UNSUPPORTED_FILE_TYPE	= 512
};


class XmlBMessageApp : public BApplication {
	public:
				XmlBMessageApp(void);
		virtual ~XmlBMessageApp();

		virtual void ReadyToRun(void);
		virtual void ArgvReceived(int32 argc, char **argv);

	private:
		void		PrintUsage(void);
		file_Type	DetectFileType(const char *filePath);
		status_t	ToXml(const char *inPath, const char *outPath);
		status_t	ToMessage(const char *inPath, const char *outPath);
		
		bool	fPrintUsage;
		bool	fToMessage;
		bool	fShow;
		bool	fOverWrite;
		int32	fArguments;
		
};

#endif 
