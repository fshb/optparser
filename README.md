# optparser
Option parser - An embedded program option parser (POSIX/GNU style) written in C++ for Windows platform. 

https://github.com/fshb/optparser/

Copyright (c) 2019 Sun Hongbo (Felix)

# License
>Permission is hereby granted, free of charge, to any person obtaining a copy of this Software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,  erge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

>***THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.***

# Usage
```CPP
#include "opt.h"//include optparser header

void Error(){ /*TODO: Code...*/}
void Usage() { /*TODO: Code...*/}
void Verbose(){ /*TODO: Code...*/}
void Check(str argstr){ /*TODO: Code...*/}
void SetStatus(str argstr){ /*TODO: Code...*/}
void GetInput(str input){ /*TODO: Code...*/}
void GetOutput(str output){ /*TODO: Code...*/}

int main(int argc, TCHAR *argv[])
{
	option::definition optdefs[] = {
		{_T("--input"), 'i', option::required_argument},
		{_T("--output"), 'o', option::required_argument},
		{_T("--help"), 'h', option::no_argument},
		{_T("--verbose"), 'v', option::no_argument},
		{_T("--check"), 'c', option::optional_argument},
		{_T("--status"), 1234, option::optional_argument},
		option::definition::nullopt()};

	option opt(argc, argv, optdefs);//parse options

	while (!opt.is_end())//iterate options
	{
		switch (opt.value())
		{
		case 'i':
			GetInput(opt.optname());
			break;

		case 'o':
			GetOutput(opt.optname());
			break;

		case 'c':
			Check(opt.argstr());
			break;
		case 1234:
			SetStatus(opt.argstr());
			break;

		case '?': //will generate a "?" as an error indicator
			if (opt.kind() == option::error)//error happened
			{
				Error();
				return 0;
			}
		case 'h':
			Usage();
			return 0;

		case 'v':
			Verbose();
			return 0;

		default:
			break;
		}

		opt.to_next(); //to next option
	}

	return 0;
}
```