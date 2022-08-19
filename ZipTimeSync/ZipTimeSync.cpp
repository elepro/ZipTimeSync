// ZipTimeSync.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
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
		//ファイル情報をクリア
		ZeroMemory(&fd,sizeof(fd));

		//引数で指定された条件のファイルを探す
		hf=FindFirstFile(argv[1],&fd);

		if( hf == INVALID_HANDLE_VALUE )
		{
			//ファイルが見つからなかった場合は終了
			_putts("Not found file.");
			return EXIT_SUCCESS;
		}
		//指定された条件のファイルが見つかった場合の処理
		//書庫内の一番新しいファイルの日付を取得し、書庫の日時を書き換える
		else
		{
			char drive[_MAX_DRIVE],path[_MAX_DIR],FileName[_MAX_FNAME],ext[_MAX_EXT];
			char fullpathFileName[_MAX_PATH];
			unz_file_info fileInfo,tmpInfo;


			//指定された条件のファイルが見つからなくなるまで繰り返す
			while(TRUE)
			{
				SYSTEMTIME st;
				FILETIME ft,lft;
				HANDLE cf;

				//ファイル名を出力	改行を入れないように_tprintfを使う
				_tprintf(fd.cFileName);

				//フォルダの時は終了して次のファイルに移動
				if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				{
					_putts("\tDirectory");
					//break;
				}
				else
				{
					//見つかったファイルのフルパスを取得
					_tsplitpath(argv[1],drive,path,FileName,ext);
					_tmakepath(fullpathFileName,drive,path,fd.cFileName,NULL);

					//書庫を開く
					unzfile=unzOpen(fullpathFileName);

					//書庫が開けなかったら終了
					if(unzfile == NULL)
					{
						_putts("\tOpen archive file error.");
						//break;
					}
					//書庫が開けた場合は、書庫内の一番新しいファイル日時を取得する
					else
					{

						//ファイル情報をクリア
						ZeroMemory(&tmpInfo,sizeof(tmpInfo));

						//最初のファイルに移動
						if(unzGoToFirstFile(unzfile) == UNZ_OK)
						{
							//書庫内のすべてのファイルに対して調べる
							while(TRUE)
							{
								//書庫内のファイルから日時などの情報取得
								int i;
								char pathBuffer[MAX_PATH];
								i = unzGetCurrentFileInfo(unzfile, &fileInfo, pathBuffer, sizeof(pathBuffer), NULL, 0, NULL, 0);

								if(i == UNZ_OK)
								{
									//より新しい日時だったらそれを取得
									if(tmpInfo.dosDate < fileInfo.dosDate)
									{
										CopyMemory(&tmpInfo,&fileInfo,sizeof(fileInfo));
									}

								}
								//次のファイルに移動
								if(unzGoToNextFile(unzfile) != UNZ_OK)
								{
									//次のファイルに移動できなかったらその書庫終了
									unzClose(unzfile);
									break;
								}
							}
							//タイムスタンプ更新
							ZeroMemory(&st,sizeof(st));

							st.wYear=(WORD)tmpInfo.tmu_date.tm_year;
							st.wMonth=(WORD)tmpInfo.tmu_date.tm_mon+1;
							st.wDay=(WORD)tmpInfo.tmu_date.tm_mday;
							st.wHour=(WORD)tmpInfo.tmu_date.tm_hour;
							st.wMinute=(WORD)tmpInfo.tmu_date.tm_min;
							st.wSecond=(WORD)tmpInfo.tmu_date.tm_sec;

							//SystemTimeをFileTimeに変換する
							if(!SystemTimeToFileTime(&st,&ft))
							{
								_putts("\tSystemTimeToFileTime error");

							}
							//時差を修正するため、LocalFileTimeをFileTimeに変換する
							if(!LocalFileTimeToFileTime(&ft,&lft))
							{
								_putts("\tLocalFileTimeToFileTime error");
							}

							//SetFileTimeするために、GENERIC_WRITE権限をつけてCreateFileをする必要がある
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

							//更新した日時を表示
							_tprintf("\t%04d/%02d/%02d %02d:%02d:%02d\r\n",tmpInfo.tmu_date.tm_year,
								tmpInfo.tmu_date.tm_mon+1,tmpInfo.tmu_date.tm_mday,tmpInfo.tmu_date.tm_hour,
								tmpInfo.tmu_date.tm_min,tmpInfo.tmu_date.tm_sec);
						
						}

						//最初のファイルに移動出来なかったらその書庫は終了
						else
						{
							_putts("\tNot find file in archive.");
							unzClose(unzfile);
						}



					}
				}
				//ファイル情報をクリア
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
