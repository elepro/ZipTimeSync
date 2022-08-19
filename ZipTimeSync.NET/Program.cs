using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZipTimeSync
{
    internal class Program
    {
        static int Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Specify files you want to change timestamp.");
                return 0;
            }
            else if (args.Length == 1)
            {
                //フォルダ名取得
                System.IO.DirectoryInfo folder = System.IO.Directory.GetParent(args[0]);
                //ファイル名（検索条件）取得
                string filter = System.IO.Path.GetFileName(args[0]);
                //引数で指定された条件のファイルを探す
                IEnumerable<string> files = System.IO.Directory.EnumerateFiles(folder.FullName, filter);

                if (files.Count() == 0)
                {
                    //ファイルが見つからなかった場合は終了
                    Console.WriteLine("Not found file.");
                    return 0;
                }
                else
                {
                    //指定された条件のファイルが見つかった場合の処理
                    //書庫内の一番新しいファイルの日付を取得し、書庫の日時を書き換える
                    //対象のファイル全てに対して処理
                    foreach (var file in files)
                    {
                        //ファイル名を出力	改行を入れない
                        Console.Write(System.IO.Path.GetFileName(file));

                        //見つかったファイルのフルパスを取得
                        string fullpathFileName = System.IO.Path.GetFullPath(file);
                        System.IO.Compression.ZipArchive zipfile;
                        //書庫を開く
                        try
                        {
                            zipfile = System.IO.Compression.ZipFile.OpenRead(fullpathFileName);

                            //書庫が開けた場合は、書庫内の一番新しいファイル日時を取得する
                            //ファイル情報をクリア
                            DateTimeOffset latestDateTimeOffset = DateTimeOffset.MinValue;
                            if (zipfile.Entries.Count == 0)
                            {
                                zipfile.Dispose();
                                //書庫内にファイル存在しなかったらその書庫は終了
                                Console.WriteLine("\tNot find file in archive.");
                            }
                            else
                            {
                                //全てのファイルを調べる
                                foreach (var entry in zipfile.Entries)
                                {
                                    //書庫内のファイルから日時などの情報取得
                                    DateTimeOffset dateTimeOffset = entry.LastWriteTime;
                                    //より新しい日時だったらそれを取得
                                    if (latestDateTimeOffset < dateTimeOffset)
                                    {
                                        latestDateTimeOffset = dateTimeOffset;
                                    }
                                }
                                zipfile.Dispose();
                                //書き込み日時を更新
                                System.IO.FileInfo fileinfo = new System.IO.FileInfo(fullpathFileName)
                                {
                                    LastWriteTime = latestDateTimeOffset.DateTime
                                };
                                //更新した日時を表示
                                Console.WriteLine("\t{0}", fileinfo.LastWriteTime.ToString("yyyy/MM/dd HH:mm:ss"));
                            }
                        }
                        catch
                        {
                            //書庫が開けなかったら終了
                            Console.WriteLine("\tOpen archive file error.");
                            continue;
                        }
                    }
                }
            }
            else
            {
                Console.WriteLine("too many arguments");
            }
            Console.WriteLine("Done.");
            return 0;
        }
    }
}
