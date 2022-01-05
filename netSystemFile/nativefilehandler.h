
#ifndef NATIVE_FILE_HANDLER_H__
#define NATIVE_FILE_HANDLER_H__

#include "fileEngine.h"
#include <list>

class NativeFileHandler : public FileEngine
{
private:
	FILE *m_fPointer;
	uint8_t *m_mappedptr;
	uint64_t m_mappedsize;

public:
	explicit NativeFileHandler(const std::string &filename = {});
	virtual ~NativeFileHandler() override;

private:
	std::list<char> checkFilemode(int mode);

	virtual bool openfile(int openmode) override;
	virtual bool closefile() override;

	virtual bool readfile(uint8_t *pData, uint32_t length) override;
	virtual bool writefile(const uint8_t *pData, uint32_t length) override;
};

#endif
