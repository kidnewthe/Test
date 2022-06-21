#include <ply-runtime/Base.h>
#include <iostream>
using namespace ply;
int main(int argc, char* argv[]) {


     Tuple<String, TextFormat> filedata = FileSystem::native()->loadTextAutodetect(
            NativePath::join(PLY_BUILD_FOLDER,"file/Strdictionary.txt"));
    if (FileSystem::native()->lastResult() != FSResult::OK)
	{
         OutStream stdOut = StdOut::text();

         stdOut << FileSystem::native()->getWorkingDirectory();
         return 0;
    }
		


    ViewInStream strViewReader{filedata.first};
    for (;;) {
        StringView line = strViewReader.readView<fmt::Line>();
        if (line.isEmpty())
            break;
        auto array = line.splitByte('\t');
        OutStream stdOut = StdOut::text();
        stdOut.format("{}:{}", array[0], array[1]);

        }
    
    return 0;
}