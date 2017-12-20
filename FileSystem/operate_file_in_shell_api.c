#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

BOOL SHODeleteFile(LPCTSTR pPath) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_ALLOWUNDO |   //����������վ
		FOF_NOCONFIRMATION; //����Ҫȷ��
	FileOp.pFrom = pPath;
	FileOp.pTo = NULL; 
	FileOp.wFunc = FO_DELETE; 
	return SHFileOperation(&FileOp) == 0;
}


BOOL SHOCopyFile(LPCTSTR pTo,LPCTSTR pFrom) {
	SHFILEOPSTRUCT FileOp={0};
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR ; //�Զ������ļ���
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
	FileOp.fFlags = FOF_NOCONFIRMATION;   //������ȷ�϶Ի���
	FileOp.pFrom = pFrom;
	FileOp.pTo = pTo;
	FileOp.wFunc = FO_RENAME;
	return SHFileOperation(&FileOp) == 0;   
}

int main(int argc, char *argv[]) {
	// TODO
	return 0;
}