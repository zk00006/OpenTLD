Format the files with astyle:
```
astyle --style=allman --indent=spaces=4 --break-blocks --pad-oper --unpad-paren --convert-tabs --align-pointer=name myfile.cpp
```
(Examples are taken from [astyle](http://astyle.sourceforge.net/astyle.html)

__Bracket Style__
_--style=allman_  
Allman brackset stlye

```c++
int Foo(bool isBar)
{
    if (isBar)
    {
        bar();
        return 1;
    }
    else
        return 0;
}
```

__Indent__
_--indent=spaces=4_  
indent with 4 spaces

__Padding Options__
_--break-blocks_  
Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...).
```c++
isFoo = true;
if(isFoo)
{
    bar();
}
else
{
    anotherBar();
}
isBar = false;

//becomes:

isFoo = true;

if(isFoo)
{
    bar();
}
else
{
    anotherBar();
}

isBar = false;
```

_--pad-oper_  
Insert space padding around operators. Any end of line comments will remain in the original column, if possible.
```c++
a=bar((b-c)*a,d--);

//becomes:

a = bar((b - c) * a, d--);
```

_--unpad-paren_  
Remove extra space padding around parenthesis on the inside and outside. Any end of line comments will remain in the original column, if possible.
```c++
bar ( a, b );

//becomes:

bar(a, b);
```

_--convert-tabs_  
Converts tabs into spaces in the non-indentation part of the line. The number of spaces inserted will maintain the spacing of the tab. The current setting for spaces per tab is used. It may not produce the expected results if convert-tabs is used when changing spaces per tab. Tabs are not replaced in quotes.

_--align-pointer=name_  
Attach a pointer or reference operator (* or &) to the variable name (right).
```c++
char * foo1;
char& foo2;

//becomes:
char *foo1;
char &foo2;
```

