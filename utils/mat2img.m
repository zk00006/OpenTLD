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

function img = mat2img(data,no_row)
% 'Data' contains square images stored in columns.

if ~exist('RATIO','var')
    RATIO = 1;
end


if isempty(data)
    img = []; return;
end

[M,N] = size(data);

sM = sqrt(M);
if ceil(sM) ~= sM
    img = []; disp('Wrong input!'); return;
end

W     = sM;
H     = sM;

if no_row > N, no_row = N; end
no_col = ceil(N/no_row);
%no_row = ceil(N/no_col);
img    = zeros(no_row*H,no_col*W);

for i = 1:N
   
    [I, J] = ind2sub([no_row, no_col], i);

    row = 1+(I-1)*H:(I-1)*H+H;
    col = 1+(J-1)*W:(J-1)*W+W;

    img0 = reshape(data(:,i),sM,sM);
    img0 = (img0 - min(img0(:))) / (max(img0(:)) - min(img0(:)));
    img(row, col) = img0;
    
end



