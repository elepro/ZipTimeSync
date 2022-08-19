// ZipTimeSync.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"


//#define _UNICODE
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include "zconf.h"
#include "zlib.h"
#include "unzip.h"

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hf=0;
	WIN32_FIND_DATA fd;
	unzFile unzfile;


/*	int a;

	_putts(GetCommandLine());

/*	for(a=0 ; a<argc ; a++)
	{
		_putts(argv[a]);
	}
*/
	if(argc < 2)
	{
		_putts("Specify files you want to change timestamp.");
		return EXIT_SUCCESS;
	}
	else if(argc == 2)
	{
		//�t�@�C�������N���A
		ZeroMemory(&fd,sizeof(fd));

		//�����Ŏw�肳�ꂽ�����̃t�@�C����T��
		hf=FindFirstFile(argv[1],&fd);

		if( hf == INVALID_HANDLE_VALUE )
		{
			//�t�@�C����������Ȃ������ꍇ�͏I��
			_putts("Not found file.");
			return EXIT_SUCCESS;
		}
		//�w�肳�ꂽ�����̃t�@�C�������������ꍇ�̏���
		//���ɓ��̈�ԐV�����t�@�C���̓��t���擾���A���ɂ̓���������������
		else
		{
			char drive[_MAX_DRIVE],path[_MAX_DIR],FileName[_MAX_FNAME],ext[_MAX_EXT];
			char fullpathFileName[_MAX_PATH];
			unz_file_info fileInfo,tmpInfo;


			//�w�肳�ꂽ�����̃t�@�C����������Ȃ��Ȃ�܂ŌJ��Ԃ�
			while(TRUE)
			{
				SYSTEMTIME st;
				FILETIME ft,lft;
				HANDLE cf;

				//�t�@�C�������o��	���s�����Ȃ��悤��_tprintf���g��
				_tprintf(fd.cFileName);

				//�t�H���_�̎��͏I�����Ď��̃t�@�C���Ɉړ�
				if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				{
					_putts("\tDirectory");
					//break;
				}
				else
				{
					//���������t�@�C���̃t���p�X���擾
					_tsplitpath(argv[1],drive,path,FileName,ext);
					_tmakepath(fullpathFileName,drive,path,fd.cFileName,NULL);

					//���ɂ��J��
					unzfile=unzOpen(fullpathFileName);

					//���ɂ��J���Ȃ�������I��
					if(unzfile == NULL)
					{
						_putts("\tOpen archive file error.");
						//break;
					}
					//���ɂ��J�����ꍇ�́A���ɓ��̈�ԐV�����t�@�C���������擾����
					else
					{

						//�t�@�C�������N���A
						ZeroMemory(&tmpInfo,sizeof(tmpInfo));

						//�ŏ��̃t�@�C���Ɉړ�
						if(unzGoToFirstFile(unzfile) == UNZ_OK)
						{
							//���ɓ��̂��ׂẴt�@�C���ɑ΂��Ē��ׂ�
							while(TRUE)
							{
								//���ɓ��̃t�@�C����������Ȃǂ̏��擾
								int i;
								char pathBuffer[MAX_PATH];
								i = unzGetCurrentFileInfo(unzfile, &fileInfo, pathBuffer, sizeof(pathBuffer), NULL, 0, NULL, 0);

								if(i == UNZ_OK)
								{
									//���V���������������炻����擾
									if(tmpInfo.dosDate < fileInfo.dosDate)
									{
										CopyMemory(&tmpInfo,&fileInfo,sizeof(fileInfo));
									}

								}
								//���̃t�@�C���Ɉړ�
								if(unzGoToNextFile(unzfile) != UNZ_OK)
								{
									//���̃t�@�C���Ɉړ��ł��Ȃ������炻�̏��ɏI��
									unzClose(unzfile);
									break;
								}
							}
							//�^�C���X�^���v�X�V
							ZeroMemory(&st,sizeof(st));

							st.wYear=(WORD)tmpInfo.tmu_date.tm_year;
							st.wMonth=(WORD)tmpInfo.tmu_date.tm_mon+1;
							st.wDay=(WORD)tmpInfo.tmu_date.tm_mday;
							st.wHour=(WORD)tmpInfo.tmu_date.tm_hour;
							st.wMinute=(WORD)tmpInfo.tmu_date.tm_min;
							st.wSecond=(WORD)tmpInfo.tmu_date.tm_sec;

							//SystemTime��FileTime�ɕϊ�����
							if(!SystemTimeToFileTime(&st,&ft))
							{
								_putts("\tSystemTimeToFileTime error");

							}
							//�������C�����邽�߁ALocalFileTime��FileTime�ɕϊ�����
							if(!LocalFileTimeToFileTime(&ft,&lft))
							{
								_putts("\tLocalFileTimeToFileTime error");
							}

							//SetFileTime���邽�߂ɁAGENERIC_WRITE����������CreateFile������K�v������
							cf = CreateFile(fullpathFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if(cf == INVALID_HANDLE_VALUE)
							{
								_putts("\tCreateFile error");
							}

							if(!SetFileTime(cf,&lft,&lft,&lft))
							{
								_putts("\tSetFileTime error");
							}

							CloseHandle(cf);

							//�X�V����������\��
							_tprintf("\t%04d/%02d/%02d %02d:%02d:%02d\r\n",tmpInfo.tmu_date.tm_year,
								tmpInfo.tmu_date.tm_mon+1,tmpInfo.tmu_date.tm_mday,tmpInfo.tmu_date.tm_hour,
								tmpInfo.tmu_date.tm_min,tmpInfo.tmu_date.tm_sec);
						
						}

						//�ŏ��̃t�@�C���Ɉړ��o���Ȃ������炻�̏��ɂ͏I��
						else
						{
							_putts("\tNot find file in archive.");
							unzClose(unzfile);
						}



					}
				}
				//�t�@�C�������N���A
				ZeroMemory(&fd,sizeof(fd));

				if(!FindNextFile(hf,&fd))
				{
					break;
				}
			}

		}
	}
	else
	{
		_putts("too many arguments");
	}
	_putts("Done.");
	return EXIT_SUCCESS;
}
