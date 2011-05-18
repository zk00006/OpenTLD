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

% BEFORE EXECUTION:
% Check prefix and files lines if they correspond to your system.
% (Un)comment as necessary.

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
	% [bilderbuchi] Please test this again. Also, octave if possible
	%
    % Tested on MacOs 10.6.7
    % with matlab R2010a maci64
    % with g++ (GCC) 4.0.1 (Apple Inc. build 5494)
    %
    % brew install opencv
    %

	% prefix = '/opt/local/'; %zdenek's prefix (opencv2.2?)
    prefix = '/usr/local/'; % /opt/local -> /usr/local
    include = [' -I' prefix 'include/' ' -I' prefix 'include/opencv/']; 
    libpath = [prefix 'lib/'];
    
    files = dir([libpath 'libopencv*.dylib']);
    
    lib = [];

    if exist('OCTAVE_VERSION','builtin')
        disp('octave');
        %for i = 1:length(files), %parser not working I think. look in the unix section.
        %    file = files(i).name;
        %    file = substr(file, 4, length(file) - 9);
        %    lib = [lib ' -l' file];
        %end
		lib = ' -lopencv_core -lopencv_imgproc -lopencv_video'; %Is this opencv 2.2?

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
		%lib = ' -lopencv_core -lopencv_imgproc -lopencv_video';

        eval(['mex lk.cpp -O' include ' -L' libpath lib]);
        mex -O -c tld.cpp
        mex -O fern.cpp tld.o
        mex -O linkagemex.cpp
        mex -O bb_overlap.cpp
        mex -O warp.cpp
        mex -O distance.cpp
    end

elseif isunix
    disp('Unix');
    % Tested on Ubuntu maverick (10.10)
    % with matlab R2010a glnx86
    % with gcc (Ubuntu/Linaro 4.4.4-14ubuntu5) 4.4.5
    %
    % apt-get install libcv-dev libhighgui-dev
    %
	%[bilderbuchi]:
    % Also tested on Ubuntu maverick (10.10)
    % with matlab R20009a glnx86 (replaced ~ with dummy variables, don't expect performance impact)
	% with gcc 4:4.4.4-1ubuntu2
	% with opencv 2.1.0-2
	%
	% Also runs with (self-compile) octave 3.4 (possibly with 3.2 now, too)
	% Tracking doesn't work, though

    prefix = '/usr/'; %OpenCV 2.1 libraries
    %prefix = '/usr/local'; %OpenCV 2.2 libraries?
    include = [' -I' prefix 'include/opencv/' ' -I' prefix 'include/']; %OpenCV 2.1 libraries
	libpath = [prefix 'lib/']; %OpenCV 2.1 libraries
    files = [dir([libpath 'libcv.so']) dir([libpath 'libcxcore.so'])]; %OpenCV 2.1 libraries
    %files = dir([libpath 'libopencv*.so.2.2']); %OpenCV 2.2 libraries?

    lib = [];
    if exist('OCTAVE_VERSION','builtin')
        disp('octave'); 
        %mkoctfile has a more picky syntax than matlab-mex concerning included libraries
        for i = 1:length(files)
            file = files(i).name;
            ind=index(file,'.'); %Find first occurence of . to correctly parse filenames like libopencv.so.2.2
            file = substr(file, 4, ind-4);
            lib = [lib ' -l' file];
        end
%         lib=' -lcv -lcxcore'; %hard-coded library arguments
        disp(lib);

        eval(['mex -v lk.cpp ' include ' -L' libpath lib]);
        mex  -c tld.cpp
        mex  fern.cpp tld.o
        mex  linkagemex.cpp
        mex  bb_overlap.cpp
        mex  warp.cpp
        mex  distance.cpp
    else
        for i = 1:length(files),
            lib = [lib ' ' libpath files(i).name];
        end
		disp(lib);

		% -O is default option for mex in Matlab
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

