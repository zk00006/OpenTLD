% Copyright 2011 Zdenek Kalal
%
% This file is part of TLD.
%
% TLD is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% TLD is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with TLD.  If not, see <http://www.gnu.org/licenses/>.

% Compiles mex files
clc; clear all; cd mex;

% "if exist('OCTAVE_VERSION')" is better after all. No global vars needed.
% %get a version string to discriminate between Matlab and Octave code
% v=ver(); 
% isoctave=0;
% ismatlab=0;
% for k=1:length(v)
%    if strcmpi(v(k).Name,'octave')
%    	isoctave=1;
% 		disp(sprintf('This is %s, Version %s',v(k).Name, v(k).Version))
%    elseif strcmp(v(k).Name, 'MATLAB')
%     ismatlab=1;
%     disp(sprintf('This is %s, Version %s',v(k).Name, v(k).Version))
%    end
% end

if ispc
    disp('PC');
    include = ' -Ic:\OpenCV2.2\include\opencv\ -Ic:\OpenCV2.2\include\';
    libpath = 'c:\OpenCV2.2\lib\';
    files = dir([libpath '*.lib']);
    
    lib = [];
    for i = 1:length(files),
        lib = [lib ' ' libpath files(i).name];
    end
    
    eval(['mex lk.cpp -O' include lib]);
    mex -O -c tld.cpp
    mex -O fern.cpp tld.obj
    mex -O linkagemex.cpp
    mex -O bb_overlap.cpp
    mex -O warp.cpp
    mex -O distance.cpp

elseif ismac
    disp('Mac');
    prefix = '/opt/local/'; % /opt/local -> /usr/local 
    include = [' -I' prefix 'include/opencv/' ' -I' prefix 'include/'];
    libpath = [prefix 'lib/'];
    
    files = dir([libpath 'libopencv*.dylib']);
    
    lib = [];

    if exist('OCTAVE_VERSION')
        disp('octave');
        for i = 1:length(files),
            file = files(i).name;
            file = substr(file, 4, length(file) - 9);
            lib = [lib ' -l' file];
        end

        eval(['mex lk.cpp' include ' -L' libpath lib]);
        mex -c tld.cpp
        mex fern.cpp tld.o
        mex linkagemex.cpp
        mex bb_overlap.cpp
        mex warp.cpp
        mex distance.cpp
    else
		disp('matlab')
        for i = 1:length(files),
            lib = [lib ' ' libpath files(i).name];
        end

        eval(['mex lk.cpp -O' include lib]);
        mex -O -c tld.cpp
        mex -O fern.cpp tld.o
        mex -O linkagemex.cpp
        mex -O bb_overlap.cpp
        mex -O warp.cpp
        mex -O distance.cpp
    end
else
    disp('Unix');
	% The following code could be prettier (see the mac section above). For some
	% reason I couldn't get the prefix version to work :-(

    include = ' -I/usr/include/opencv/ -I/usr/include/'; % /opt/local -> /usr/local
	libpath = '/usr/lib/'; % /opt/local -> /usr/local

    files = [dir([libpath 'libcv.so']) dir([libpath 'libcxcore.so'])];

    lib = [];
    if exist('OCTAVE_VERSION')
        disp('octave'); 
        %mkoctfile has a more picky syntax than matlab-mex concerning included libraries
        for i = 1:length(files)
            file = files(i).name;
            ind=rindex(file,'.');
            file = substr(file, 4, ind-4);
            lib = [lib ' -l' file];
        end
%         lib=' -lcv -lcxcore'; %hard-coded library arguments
        disp(lib);
        eval(['mex -v lk.cpp ' include ' -L' libpath lib]);
        mex  -v -c tld.cpp
        mex  -v fern.cpp tld.o
        mex  -v linkagemex.cpp
        mex  -v bb_overlap.cpp
        mex  -v warp.cpp
        mex  -v distance.cpp
    else
        for i = 1:length(files),
            lib = [lib ' ' libpath files(i).name];
            disp(lib);
        end
        eval(['mex lk.cpp ' include lib]);
        mex  -c tld.cpp
        mex  fern.cpp tld.o
        mex  linkagemex.cpp
        mex  bb_overlap.cpp
        mex  warp.cpp
        mex  distance.cpp
    end
end


cd ..
disp('Compilation finished.');

