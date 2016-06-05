#ifndef MESSAGEXMLREADER_H_INCLUDED
#define MESSAGEXMLREADER_H_INCLUDED



class XmlBMessageApp : public BApplication {
	public:
				XxmlBMessageApp(void);
		virtual ~XmlBMessageApp();

		virtual void ReadyToRun(void);
		virtual void ArgvReceived(int32 argc, char **argv);

	private:
		void		PrintUsage(void);
		uint32		DetectFileType(BFile *file);
		status_t	ToXml(const char *inPath, const char *outPath);
		status_t	ToMessage(const char *inPath, const char *outPath);
		
		bool	fPrintUsage;
		bool	fToMessage;
		bool	fOverWrite;
		int32	fArguments;
		
};

#endif
