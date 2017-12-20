#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

BOOL SHODeleteFile(LPCTSTR pPath) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_ALLOWUNDO |   //允许放入回收站
		FOF_NOCONFIRMATION; //不需要确认
	FileOp.pFrom = pPath;
	FileOp.pTo = NULL; 
	FileOp.wFunc = FO_DELETE; 
	return SHFileOperation(&FileOp) == 0;
}


BOOL SHOCopyFile(LPCTSTR pTo,LPCTSTR pFrom) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR ; //自动创建文件夹
	FileOp.pFrom = pFrom;
	FileOp.pTo = pTo;
	FileOp.wFunc = FO_COPY;
	return SHFileOperation(&FileOp) == 0;
}


BOOL SHOMoveFile(LPCTSTR pTo,LPCTSTR pFrom) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR ;
	FileOp.pFrom = pFrom;
	FileOp.pTo = pTo;
	FileOp.wFunc = FO_MOVE;
	return SHFileOperation(&FileOp) == 0;   
}

BOOL SHOReNameFile(LPCTSTR pTo,LPCTSTR pFrom) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_NOCONFIRMATION;   //不出现确认对话框
	FileOp.pFrom = pFrom;
	FileOp.pTo = pTo;
	FileOp.wFunc = FO_RENAME;
	return SHFileOperation(&FileOp) == 0;   
}

int main(int argc, char *argv[]) {
	// TODO
	return 0;
}