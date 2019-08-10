/*
 Option parser - An embedded program option parser (POSIX/GNU style) written in C++ for Windows platform. 
 https://github.com/fshb/optparser/
 Copyright (c) 2019 Sun Hongbo (Felix)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this Software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "opt.h"
#include <windows.h>

#include <stdlib.h>

str KSTR(int kind)
{
	str s;
	switch (kind)
	{
	case option::unknown:
		s = _T("unknown");
		break;
	case option::operand:
		s = _T("operand");
		break;
	case option::no_argument:
		s = _T("no_argument");
		break;
	case option::required_argument:
		s = _T("required_argument");
		break;
	case option::optional_argument:
		s = _T("optional_argument");
		break;
	default:
		s = _T("");
		break;
	}
	return s;
}
int main()
{
	option::definition optdefs[] = {
		{_T("--input"), 'i', option::required_argument, NULL},
		{_T("--output"), 'o', option::required_argument, NULL},
		{_T("--help"), 'h', option::no_argument, NULL},
		{_T("--verbose"), 'v', option::no_argument, NULL},
		{_T("--check"), 'c', option::optional_argument, NULL},
		{_T("--status"), 1234, option::optional_argument, NULL},
		option::definition::nullopt()
	};
	const TCHAR *argv[] =
	{
		_T(".\\main.exe"),
		_T("-iabc"),
		_T("-vh"),
		_T("--output=abc"),
		_T("-o"),
		_T("-"),
		_T("-c"),
		_T("--output"),
		_T("--help"),
		_T("-cabc"),
		_T("--help=abc"),
		_T("-?ch"),
		_T("-hcsabc"),
		//_T("--input")
		_T("--"),
		_T("-iabc"),
		_T("-"),
		_T("--input")
	};

	int argc = _countof(argv);

	_tprintf(_T("TEST: main.exe"));

	for (int i = 1; i < argc; i++)
	{
		_tprintf(_T(" %s"), argv[i]);
	}
	_tprintf(_T("\n\n"));


	option opt(argc, argv, optdefs);
	_tprintf(_T("\n\nParsing result:\n"));
	while (!opt.is_end())
	{
		_tprintf(_T("Option: %s, \tValue: %c, \tKind: %s, \tArg: %s\n"), 
			opt.optname().c_str(), 
			opt.value(), 
			KSTR(opt.kind()).c_str(), 
			opt.argstr().c_str());
		opt.to_next();
	}

	system("pause");
	return 0;
}
