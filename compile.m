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
    % Tested on MacOs 10.6.7
    % with matlab R2010a maci64
    % with g++ (GCC) 4.0.1 (Apple Inc. build 5494)
    %
    % brew install opencv
    %
    prefix = '/usr/local/'; % /opt/local -> /usr/local
    include = [' -I' prefix 'include/' ' -I' prefix 'include/opencv/']; 
    libpath = [' -L' prefix 'lib/'];
    lib = ' -lopencv_core -lopencv_imgproc -lopencv_video';
    
    eval(['mex -O lk.cpp' include libpath lib]);
    mex -O -c tld.cpp
    mex -O fern.cpp tld.o
    mex -O linkagemex.cpp
    mex -O bb_overlap.cpp
    mex -O warp.cpp
    mex -O distance.cpp
elseif isunix
    disp('Unix');
    % Tested on Ubuntu maverick (10.10)
    % with matlab R2010a glnx86
    % with gcc (Ubuntu/Linaro 4.4.4-14ubuntu5) 4.4.5
    %
    % apt-get install libcv-dev libhighgui-dev
    %
    include = ' -I/usr/include/opencv/';
    lib = ' -lcxcore -lcv';

    eval(['mex lk.cpp -O' include lib]);
    mex -O -c tld.cpp
    mex -O fern.cpp tld.o
    mex -O linkagemex.cpp
    mex -O bb_overlap.cpp
    mex -O warp.cpp
    mex -O distance.cpp
end


cd ..
disp('Compilation finished.');

