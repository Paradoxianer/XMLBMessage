#include "MessageXmlWriter.h"
#include <storage/File.h>
#include <storage/Path.h>
#include <storage/Entry.h>
#include <support/Debug.h>


MessageXmlWriter::MessageXmlWriter(){
    filePath=new BString("");
}

MessageXmlWriter::MessageXmlWriter(const BString &fileName){
	filePath=new BString("");
    SetTo(fileName);
}

MessageXmlWriter::~MessageXmlWriter(){
}

status_t MessageXmlWriter::InitCheck(){
    BFile   file (filePath->String(),B_WRITE_ONLY);
    return file.InitCheck();
}

void MessageXmlWriter::SetTo(const BString &fileName){
    if (fileName!=NULL){
        filePath->SetTo(fileName);
    }
    
}

status_t MessageXmlWriter::Write(BMessage &message){
	TiXmlDocument	doc;
	//add the Comment
  	TiXmlComment	comment("Generated by MessageXmlWriter");
	doc.InsertEndChild(comment);
  	doc.InsertEndChild(ProcessMessage(&message));
    if ( doc.SaveFile(filePath->String()) == true)
    	return B_OK;
    else
    	return B_ERROR;
}

status_t MessageXmlWriter::WriteFile(const BString &fileName,const BMessage &message){
	TiXmlDocument	doc;
	//add the Comment
	TiXmlDeclaration declare =  TiXmlDeclaration("1.0","UTF-8","yes");
  	TiXmlComment	comment("Generated by MessageXmlWriter");
	doc.InsertEndChild(declare);
	doc.InsertEndChild(comment);
  	doc.InsertEndChild(ProcessMessage((BMessage*)&message));
    if ( doc.SaveFile(fileName.String()) == true)
    	return B_OK;
    else
    	return B_ERROR;
}

TiXmlElement  MessageXmlWriter::ProcessMessage( BMessage *msg){
	TiXmlElement	xmlNode("BMessage");
	xmlNode.SetAttribute("type","BMessage");
	xmlNode.SetAttribute("what",(int32)(msg->what));
	char		*name;
	uint32	    type;
	int32		count;
	int32		i =0;
	TRACE();
		while (msg->GetInfo(B_ANY_TYPE,i ,(char **)&name, &type, &count) == B_OK) {
		printf("%zu",type);
		TRACE();
		TiXmlElement	xmlSubNode("Data");
		xmlSubNode.SetAttribute("name",name);
		switch(type){
			case B_MESSAGE_TYPE:{
				BMessage tmpMessage;
				xmlSubNode.SetAttribute("type","BMessage");
				if (msg->FindMessage(name, count-1, &tmpMessage)== B_OK){
					xmlSubNode.InsertEndChild(ProcessMessage(&tmpMessage));
				}
				break;
			}
			case B_BOOL_TYPE:{
		        //FindBool()
		        bool tmpBool;
		        xmlSubNode.SetAttribute("type","bool");
		        if (msg->FindBool(name, count-1, &tmpBool) == B_OK){
					if  (tmpBool == true)
						xmlSubNode.SetAttribute("value","true");
					else
						xmlSubNode.SetAttribute("value","false");
				}
				break;
			}
			case B_INT8_TYPE:{
		        //FindInt8()	an int8 or uint8
		        int8 tmpInt8;
				xmlSubNode.SetAttribute("type","int8");
				 if (msg->FindInt8(name, count-1, &tmpInt8) == B_OK){
					xmlSubNode.SetAttribute("value",tmpInt8);
				}
				break;
			}
			case B_INT16_TYPE:{
		        //FindInt16()	an int16 or uint16
		        int16 tmpInt16;
				xmlSubNode.SetAttribute("type","int16");
				if (msg->FindInt16(name, count-1, &tmpInt16) == B_OK){
					xmlSubNode.SetAttribute("value",tmpInt16);
				}
				break;
			}
			case B_INT32_TYPE:{
				xmlSubNode.SetAttribute("type","int32");
				int32 tmpInt32;
				if (msg->FindInt32(name, count-1, &tmpInt32) == B_OK){
					xmlSubNode.SetAttribute("value",tmpInt32);
				}
				break;
			}
			case B_INT64_TYPE:{
				int64 tmpInt64;
				xmlSubNode.SetAttribute("type","int64");
				 if (msg->FindInt64(name, count-1, &tmpInt64) == B_OK){
					xmlSubNode.SetAttribute("value",tmpInt64);
				}
				break;
			}
			case B_FLOAT_TYPE:{
		        float	tmpFloat;
				xmlSubNode.SetAttribute("type","float");
				if (msg->FindFloat(name, count-1, &tmpFloat) == B_OK){
					xmlSubNode.SetDoubleAttribute("value",tmpFloat);
				}
				break;
			}
			case B_DOUBLE_TYPE:{
				double tmpDouble;
				xmlSubNode.SetAttribute("type","double");
				if (msg->FindDouble(name, count-1, &tmpDouble) == B_OK){
					xmlSubNode.SetDoubleAttribute("value", tmpDouble);
				}
				break;
			}
			case B_STRING_TYPE:{
				xmlSubNode.SetAttribute("type","string");
				const char *tmpChar;
				 if (msg->FindString(name, count-1, &tmpChar) == B_OK){
					xmlSubNode.SetAttribute("value",tmpChar);
				}
				break;
			}
			case B_POINT_TYPE:{
				BPoint tmpPoint;
				xmlSubNode.SetAttribute("type","BPoint");
				if (msg->FindPoint(name, count-1, &tmpPoint) == B_OK){
					xmlSubNode.SetDoubleAttribute("x",tmpPoint.x);
					xmlSubNode.SetDoubleAttribute("y",tmpPoint.y);
				}
				break;
			}
			case B_RECT_TYPE:{
				TRACE();
				BRect tmpRect;
				if (msg->FindRect(name, count-1, &tmpRect) == B_OK){
					xmlSubNode.SetAttribute("type","BRect");
					xmlSubNode.SetDoubleAttribute("top",tmpRect.top);
					xmlSubNode.SetDoubleAttribute("left",tmpRect.left);
					xmlSubNode.SetDoubleAttribute("bottom",tmpRect.bottom);
					xmlSubNode.SetDoubleAttribute("right",tmpRect.right);
				}
				break;
			}
			case B_REF_TYPE:{
				entry_ref tmpRef;
				if (msg->FindRef(name, count-1, &tmpRef) == B_OK){
					BPath *path= new BPath(&tmpRef);
					xmlSubNode.SetAttribute("type","B_REF_TYPE");
					xmlSubNode.SetAttribute("path",path->Path());
					delete path;
				}
				break;
			}
	/*		case B_MESSENGER_TYPE:{
				if (msg->FindData(name, B_ANY_TYPE, i, (const void **)&data, &numBytes) == B_OK){
					BMessage *messengerArchive	= new BMessage();
					((BMessenger*)data)->Archive(messengerArchive , true);
					xmlSubNode.SetAttribute("type","B_MESSENGER_TYPE");
					xmlSubNode.InsertEndChild(ProcessMessage(messengerArchive));
				}
			}*/
			case B_POINTER_TYPE:{
				TRACE();
				void *tmpPointer;
				if (msg->FindPointer(name, count-1, &tmpPointer) == B_OK){
					xmlSubNode.SetAttribute("type","B_POINTER_TYPE");
					xmlSubNode.SetDoubleAttribute("value",(int32)tmpPointer);
				}
				break;
			}
			default:{
					char *code=new char[5];
					sprintf(code, "%c%c%c%c", (uint8)(type), (uint8)(type >> 8), (uint8)(type >> 16), (uint8)(type >> 24));
					xmlSubNode.SetAttribute("type",code);
					void *data;
					ssize_t size;
					if (msg->FindData(name, B_ANY_TYPE,i, (const void **)&data, &size)){
				/**		@ToDo adding data encode   b64_encode()
						xmlSubNode.SetValue();*/
					}
			}
		}
		xmlNode.InsertEndChild(xmlSubNode);
		i++;
	}
	return xmlNode;
}
