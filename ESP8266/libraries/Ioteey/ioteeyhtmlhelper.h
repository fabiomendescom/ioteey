#include <WString.h>

class ioteeyhtmlhelper {
	private:
	
	public:
		String startFORM(const char* name, const char* action);
		String endFORM();
		String startTABLE(const char* width, const char* height, const char* border, const char* cellspacing, const char* cellpadding);
		String endTABLE();
		String startROW();
		String endROW();
		String startCOLUMN(const char* bgcolor, const char* align, const char* valign, const char* width, const char* height, const char* colspan, bool nowrap);
		String endCOLUMN();
		String writeH2(const char* text);
		String writeH3(const char* text);
		String startFONT(const char* size);
		String endFONT();
		String writeHR(const char* size);
		String writeTEXT(const char* text);
		String writeSpaces(int spaces);
		String writeRECORD(const char* label, const char* value);
		String writeTextInput(const char* label, const char* name, const char* value, const char* size, const char* type);
		String renderParameter(String code, String sensororactuator, int sensororactuatorid, int parameternumber, String parameterid, String parametername, String parametertype, String parametervalue);
		String writeLine();
		String writeRecordHeading(const char* label);
		String writeHeading(); 	
		String writeSelectionBox(const char* label, const char* name, int totalitems, const char labels[][10], const char values[][10], const char* size);	
		String writeButtonRECORD(const char* name, const char* value, const char* size);
		String writeLink(const char* label, const char* url);
};
