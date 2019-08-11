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

#ifdef USAGE //usage
void Usage()
{ /*TODO: Code...*/
}
void Verbose()
{ /*TODO: Code...*/
}
void Check(str zArg)
{ /*TODO: Code...*/
}
void SetStatus(str zArg)
{ /*TODO: Code...*/
}
void GetInput(str input)
{ /*TODO: Code...*/
}
void GetOutput(str output)
{ /*TODO: Code...*/
}

int main(int argc, TCHAR *argv[])
{
	option::definition optdefs[] = {
		{_T("--input"), 'i', option::required_argument, NULL},
		{_T("--output"), 'o', option::required_argument, NULL},
		{_T("--help"), 'h', option::no_argument, NULL},
		{_T("--verbose"), 'v', option::no_argument, NULL},
		{_T("--check"), 'c', option::optional_argument, NULL},
		{_T("--status"), 1234, option::optional_argument, NULL},
		option::definition::nullopt()};

	option opt(argc, argv, optdefs);

	while (!opt.is_end())
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

		case 'h':
		case '?':
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

#endif



#include <stdarg.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string>

using str = std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>>;

class option
{
public:
	enum : int
	{
		unknown = -1,
		operand,
		no_argument,
		required_argument,
		optional_argument
	};

	struct definition
	{
		str optname;
		int value;
		int kind;
		str argstr;

		definition() : optname(_T("")), value(0), kind(0), argstr(_T("")){};
		definition(const TCHAR *zOptName, int nValue, int nKind, const TCHAR *zArgStr)
		{
			optname = zOptName == NULL ? _T("") : zOptName;
			value = nValue;
			kind = nKind;
			argstr = zArgStr == NULL ? _T("") : zArgStr;
		};

		static const definition &nullopt()
		{
			definition *nil = new definition;
			return *nil;
		};
		bool operator==(const definition &r)
		{
			if (value != r.value || kind != r.kind)
				return false;

			if (optname != _T(""))
				return false;

			if (argstr != _T(""))
				return false;

			return true;
		};

		bool operator!=(const definition &r)
		{
			return !(*this == r);
		};

		definition &operator=(const definition &v)
		{
			optname = v.optname;
			argstr = v.argstr;
			value = v.value;
			kind = v.kind;

			return *this;
		};
	};

private:
	str _zProgramName; //program name (program name only)

	int _nErrCount; //error count

	int _iOpt; //option index
	std::vector<definition> _optlist;

	const definition nullopt;

private:
	void parse_program_name(str &optstr)
	{
		int i = optstr.length() - 1;
		while (i >= 0)
		{
			if (optstr[i] == '\\')
			{
				_zProgramName = optstr.substr(i + 1);
				break;
			}
			i--;
		}
	};

	void parse_operand(definition &opt, str optstr)
	{
		opt.optname = optstr;
		opt.kind = operand;
		opt.value = 0;
		opt.argstr = _T("");
	};

	bool parse_short_option(definition &shortopt, int value, definition optdefs[])
	{
		definition *def = optdefs;

		while (*def != nullopt)
		{
			if (value == def->value)
			{
				shortopt = *def;
				return true;
			}
			def++;
		}
		return false;
	};

	bool parse_long_option(definition &longopt, str optstr, definition optdefs[])
	{
		bool hasArg = false;
		int i = 0;
		while (optstr[i] != '\0')
		{
			if (optstr[i] == '=')
			{
				hasArg = true;
				break;
			}
			i++;
		}

		definition *def = optdefs;
		while (*def != nullopt)
		{
			if (def->optname == optstr.substr(0, i))
			{
				if (hasArg)
				{
					if (def->kind == required_argument || def->kind == optional_argument)
					{
						longopt = *def;
						longopt.argstr = optstr.substr(i + 1);
						return true;
					}
					else if (def->kind == no_argument)
					{
						err(_T("%s ==> %s: cannot take any argument! ==> %s: illegal argument\n"),
							optstr.c_str(), def->optname.c_str(), optstr.substr(i + 1).c_str());
						return false;
					}
				}
				else
				{
					longopt = *def;
					return true;
				}
			}
			def++;
		}
		return false;
	};
	void parse_option_argument(definition &opt, str optstr)
	{
		opt.argstr = optstr;
	};

	void err(const TCHAR *fmt, ...)
	{
		if (_nErrCount == 0)
			_ftprintf(stderr, _T("%s: error(s) in option(s):\n"), _zProgramName.c_str());

		va_list ap;
		va_start(ap, fmt);
		_vftprintf(stderr, fmt, ap);
		va_end(ap);

		definition opt;
		opt.value = '?';
		opt.kind = unknown;
		_optlist.push_back(opt);

		_nErrCount++;
	};

public:
	option(int argc, const TCHAR *argv[], option::definition optdefs[])
	{
		_nErrCount = 0; //initialize error count
		int i = 0;
		size_t len = 0;
		str zArg;
		bool isOperandOnly = false;
		definition opt;

		zArg = argv[0];
		parse_program_name(zArg);
		for (i = 1; i < argc; i++)
		{
			zArg = argv[i];
			len = zArg.length();

			if (isOperandOnly)
			{
				parse_operand(opt, zArg);
				_optlist.push_back(opt);
			}
			else if (zArg[0] == '-')
			{
				if (len == 1) //"-" option
				{
					parse_operand(opt, _T("-"));
					_optlist.push_back(opt);
				}
				else if (zArg[1] != '-')
				{
					size_t k = 1;
					while (k < len)
					{
						if (!parse_short_option(opt, zArg[k], optdefs))
						{
							err(_T("%s ==> -%c: unknown option!\n"),
								zArg.c_str(), zArg[k]);
						}
						else
						{
							if (opt.kind == required_argument || opt.kind == optional_argument)
							{
								if (k == 1)
								{
									if (len > 2)
									{
										parse_option_argument(opt, zArg.substr(k + 1));
										_optlist.push_back(opt);
										break;
									}
									else if (opt.kind == required_argument)
									{
										i++;
										if (i >= argc)
										{
											err(_T("%s ==> %s: requires an argument!"),
												zArg.c_str(), opt.optname.c_str());
										}
										else
										{
											zArg = argv[i];
											parse_option_argument(opt, zArg);
											_optlist.push_back(opt);
											break;
										}
									}
								} //END if(k == 1)
								else
								{
									err(_T("%s ==> -%c: attempt to take argument in compressed option format! \n\t==> %s: illegal argument!\n"),
										zArg.c_str(), zArg[k], zArg.substr(k + 1).c_str());
								}
							} //END if(opt.kind == required_argument || opt.kind == optional_argument)
							else
							{
								_optlist.push_back(opt);
							}
						} //END else
						k++;
					} //END while (k < len)
				} //END else if(zArg[1] != '-')
				else
				{
					if (zArg[2] == '\0') //"--" delimeter
					{
						isOperandOnly = true;
					}
					else
					{
						if (!parse_long_option(opt, zArg, optdefs))
						{
							err(_T("%s ==> %s: unknown option!\n"),
								zArg.c_str(), zArg.c_str());
						}
						else
						{
							if (opt.kind == required_argument && opt.argstr == _T(""))
							{
								i++;
								if (i >= argc)
								{
									err(_T("%s ==> %s: requires an argument!"),
										zArg.c_str(), opt.optname.c_str());
								}
								else
								{
									zArg = argv[i];
									parse_option_argument(opt, zArg);
									_optlist.push_back(opt);
								}
							}
							else
							{
								_optlist.push_back(opt);
							}
						} //END else
					} //END else
				} //END else
			} //END if(zArg[0] == '-')
			else
			{
				parse_operand(opt, zArg);
				_optlist.push_back(opt);
			}
		} //END for (i = 1; i < argc; i++)
		_iOpt = 0;
	};
	~option(){};

public:
	option &to_previous()
	{
		_iOpt--;
		return *this;
	};
	option &to_next()
	{
		_iOpt++;
		return *this;
	};
	option &to_begin()
	{
		_iOpt = 0;
		return *this;
	};
	option &to_end()
	{
		_iOpt = _optlist.size() - 1;
		return *this;
	};

	bool is_begin()
	{
		return _iOpt < 0;
	};
	bool is_end()
	{
		return _iOpt >= (int)_optlist.size();
	};

	int value()
	{
		return _optlist[_iOpt].value;
	};
	int kind()
	{
		return _optlist[_iOpt].kind;
	};

	str optname()
	{
		return _optlist[_iOpt].optname;
	};
	str argstr()
	{
		return _optlist[_iOpt].argstr;
	};
};


#ifdef TEST
//test code, put below code to an individule .cpp file

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
		option::definition::nullopt()};
	const TCHAR *argv[] = {
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
		_T("--input")};

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

#endif
